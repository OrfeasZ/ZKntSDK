#include "RenderingHost.hpp"

namespace knt::rendering {
    RenderingHost& RenderingHost::Instance() {
        static RenderingHost s_Instance;
        return s_Instance;
    }

    void RenderingHost::Startup() {
        m_Services.RegisterRenderingCallbacks = &RenderingHost::RegisterCallbacksThunk;
        m_Services.UnregisterRenderingCallbacks = &RenderingHost::UnregisterCallbacksThunk;
        m_Services.IsEngineInitialized = &RenderingHost::IsEngineInitializedThunk;
        m_Services.SetEngineInitialized = &RenderingHost::SetEngineInitializedThunk;
    }

    void RenderingHost::Shutdown() {
        UnregisterCallbacks();

        if (auto* s_SwapChain = m_ActiveSwapChain.exchange(nullptr)) {
            s_SwapChain->Release();
        }
        if (auto* s_Queue = m_ActiveCommandQueue.exchange(nullptr)) {
            s_Queue->Release();
        }
    }

    void RenderingHost::SetActiveSwapChain(IDXGISwapChain4* p_SwapChain) {
        if (p_SwapChain) {
            p_SwapChain->AddRef();
        }
        if (auto* s_Previous = m_ActiveSwapChain.exchange(p_SwapChain)) {
            s_Previous->Release();
        }
    }

    void RenderingHost::SetActiveCommandQueue(ID3D12CommandQueue* p_CommandQueue) {
        if (p_CommandQueue) {
            p_CommandQueue->AddRef();
        }
        if (auto* s_Previous = m_ActiveCommandQueue.exchange(p_CommandQueue)) {
            s_Previous->Release();
        }
    }

    void RenderingHost::DispatchOnPresent(IDXGISwapChain4* p_SwapChain) {
        auto s_Ticket = m_Gate.TryEnter();
        if (!s_Ticket) {
            return;
        }
        const auto* s_Callbacks = m_Callbacks.load(std::memory_order_acquire);
        if (s_Callbacks && s_Callbacks->OnPresent) {
            s_Callbacks->OnPresent(p_SwapChain);
        }
    }

    void RenderingHost::DispatchPostPresent(IDXGISwapChain4* p_SwapChain, HRESULT p_Result) {
        auto s_Ticket = m_Gate.TryEnter();
        if (!s_Ticket) {
            return;
        }
        const auto* s_Callbacks = m_Callbacks.load(std::memory_order_acquire);
        if (s_Callbacks && s_Callbacks->PostPresent) {
            s_Callbacks->PostPresent(p_SwapChain, p_Result);
        }
    }

    void RenderingHost::DispatchOnReset(IDXGISwapChain4* p_SwapChain) {
        auto s_Ticket = m_Gate.TryEnter();
        if (!s_Ticket) {
            return;
        }
        const auto* s_Callbacks = m_Callbacks.load(std::memory_order_acquire);
        if (s_Callbacks && s_Callbacks->OnReset) {
            s_Callbacks->OnReset(p_SwapChain);
        }
    }

    void RenderingHost::DispatchPostReset(IDXGISwapChain4* p_SwapChain) {
        auto s_Ticket = m_Gate.TryEnter();
        if (!s_Ticket) {
            return;
        }
        const auto* s_Callbacks = m_Callbacks.load(std::memory_order_acquire);
        if (s_Callbacks && s_Callbacks->PostReset) {
            s_Callbacks->PostReset(p_SwapChain);
        }
    }

    knt::host::WndProcResult RenderingHost::DispatchOnWndProc(HWND p_Hwnd, UINT p_Msg, WPARAM p_Wparam, LPARAM p_Lparam) {
        auto s_Ticket = m_Gate.TryEnter();
        if (!s_Ticket) {
            return {false, 0};
        }
        const auto* s_Callbacks = m_Callbacks.load(std::memory_order_acquire);
        if (!s_Callbacks || !s_Callbacks->OnWndProc) {
            return {false, 0};
        }
        return s_Callbacks->OnWndProc(p_Hwnd, p_Msg, p_Wparam, p_Lparam);
    }

    void RenderingHost::NotifyNewSwapChain(IDXGISwapChain4* p_SwapChain, IUnknown* p_DeviceOrQueue) {
        SetActiveSwapChain(p_SwapChain);

        // CreateSwapChain* takes an ID3D12CommandQueue (D3D12 path) or an
        // ID3D11Device (D3D11). Try the queue; remember it if it succeeds.
        ID3D12CommandQueue* s_Queue = nullptr;
        if (p_DeviceOrQueue) {
            p_DeviceOrQueue->QueryInterface(IID_PPV_ARGS(&s_Queue));
        }
        if (s_Queue) {
            SetActiveCommandQueue(s_Queue);
            s_Queue->Release();
        }

        // If an SDK is already registered, replay set-swap-chain/set-queue so
        // it doesn't have to wait for the next Present cycle.
        auto s_Ticket = m_Gate.TryEnter();
        if (!s_Ticket) {
            return;
        }
        const auto* s_Callbacks = m_Callbacks.load(std::memory_order_acquire);
        if (!s_Callbacks) {
            return;
        }
        if (s_Callbacks->SetSwapChain) {
            s_Callbacks->SetSwapChain(p_SwapChain);
        }
        if (s_Callbacks->SetCommandQueue) {
            if (auto* s_ActiveQueue = m_ActiveCommandQueue.load(std::memory_order_acquire)) {
                s_Callbacks->SetCommandQueue(s_ActiveQueue);
            }
        }
    }

    void RenderingHost::RegisterCallbacksThunk(const knt::host::RenderingCallbacks* p_Callbacks) {
        Instance().RegisterCallbacks(p_Callbacks);
    }

    void RenderingHost::UnregisterCallbacksThunk() {
        Instance().UnregisterCallbacks();
    }

    bool RenderingHost::IsEngineInitializedThunk() {
        return Instance().IsEngineInitialized();
    }

    void RenderingHost::SetEngineInitializedThunk(bool p_Initialized) {
        Instance().SetEngineInitialized(p_Initialized);
    }

    void RenderingHost::RegisterCallbacks(const knt::host::RenderingCallbacks* p_Callbacks) {
        // Defensive: if something else is still registered, drain it first.
        if (m_Callbacks.load(std::memory_order_acquire) != nullptr) {
            UnregisterCallbacks();
        }

        m_Gate.Reset();
        m_Callbacks.store(p_Callbacks, std::memory_order_release);

        // Replay the current swap chain / queue so the SDK doesn't have to
        // wait for the next Create or Present cycle.
        if (!p_Callbacks) {
            return;
        }
        if (auto* s_SwapChain = m_ActiveSwapChain.load(std::memory_order_acquire); s_SwapChain && p_Callbacks->SetSwapChain) {
            p_Callbacks->SetSwapChain(s_SwapChain);
        }
        if (auto* s_Queue = m_ActiveCommandQueue.load(std::memory_order_acquire); s_Queue && p_Callbacks->SetCommandQueue) {
            p_Callbacks->SetCommandQueue(s_Queue);
        }
    }

    void RenderingHost::UnregisterCallbacks() {
        // Stop accepting new dispatches and wait for in-flight ones to finish.
        // Leave the gate closed; the next Register() resets it.
        m_Gate.BeginReload();
        m_Callbacks.store(nullptr, std::memory_order_release);
    }

    bool RenderingHost::IsEngineInitialized() const {
        return m_EngineInitialized.load(std::memory_order_acquire);
    }

    void RenderingHost::SetEngineInitialized(bool p_Initialized) {
        m_EngineInitialized.store(p_Initialized, std::memory_order_release);
    }
}
