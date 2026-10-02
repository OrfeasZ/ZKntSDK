#pragma once

#include "HostServices.hpp"
#include "Rendering/ReloadGate.hpp"

#include <Windows.h>
#include <atomic>

namespace knt::rendering {
    // Proxy-side rendering state. Owns the HostServices table exposed to the
    // SDK via cr_plugin::userdata, the atomically-swappable RenderingCallbacks
    // table, and a ReloadGate that drains in-flight dispatches before the SDK
    // unmaps. Process-lifetime singleton.
    class RenderingHost {
      public:
        static RenderingHost& Instance();

        void Startup();
        void Shutdown();

        knt::host::HostServices* Services() {
            return &m_Services;
        }

        // Tracking the active swap chain / queue lets us replay them to a
        // freshly-loaded SDK without waiting for the next CreateSwapChain.
        void SetActiveSwapChain(IDXGISwapChain4* p_SwapChain);
        void SetActiveCommandQueue(ID3D12CommandQueue* p_CommandQueue);

        // Each Dispatch* acquires the ReloadGate; if the SDK is reloading the
        // call becomes a no-op (or passthrough, for WndProc).
        void DispatchOnPresent(IDXGISwapChain4* p_SwapChain);
        void DispatchPostPresent(IDXGISwapChain4* p_SwapChain, HRESULT p_Result);
        void DispatchOnReset(IDXGISwapChain4* p_SwapChain);
        void DispatchPostReset(IDXGISwapChain4* p_SwapChain);
        knt::host::WndProcResult DispatchOnWndProc(HWND p_Hwnd, UINT p_Msg, WPARAM p_Wparam, LPARAM p_Lparam);

        // Called from each CreateSwapChain* detour. Stores the swap chain and
        // (when available) its command queue so a future reload can resubmit.
        void NotifyNewSwapChain(IDXGISwapChain4* p_SwapChain, IUnknown* p_DeviceOrQueue);

      private:
        RenderingHost() = default;

        static void RegisterCallbacksThunk(const knt::host::RenderingCallbacks* p_Callbacks);
        static void UnregisterCallbacksThunk();
        static bool IsEngineInitializedThunk();
        static void SetEngineInitializedThunk(bool p_Initialized);

        void RegisterCallbacks(const knt::host::RenderingCallbacks* p_Callbacks);
        void UnregisterCallbacks();
        bool IsEngineInitialized() const;
        void SetEngineInitialized(bool p_Initialized);

        knt::host::HostServices m_Services{};
        std::atomic<const knt::host::RenderingCallbacks*> m_Callbacks{nullptr};
        ReloadGate m_Gate;
        std::atomic<IDXGISwapChain4*> m_ActiveSwapChain{nullptr};
        std::atomic<ID3D12CommandQueue*> m_ActiveCommandQueue{nullptr};
        std::atomic<bool> m_EngineInitialized{false};
    };
}
