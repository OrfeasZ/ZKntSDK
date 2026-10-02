#include "WndProcSubclass.hpp"

#include "RenderingHost.hpp"

namespace knt::rendering {
    WndProcSubclass& WndProcSubclass::Instance() {
        static WndProcSubclass s_Instance;
        return s_Instance;
    }

    void WndProcSubclass::EnsureInstalled(IDXGISwapChain4* p_SwapChain) {
        if (m_Hwnd.load(std::memory_order_acquire) != nullptr) {
            return;
        }

        HWND s_Hwnd = nullptr;
        if (FAILED(p_SwapChain->GetHwnd(&s_Hwnd)) || !s_Hwnd) {
            return;
        }

        // CAS so only one thread installs the subclass.
        HWND s_Expected = nullptr;
        if (!m_Hwnd.compare_exchange_strong(s_Expected, s_Hwnd, std::memory_order_acq_rel)) {
            return;
        }

        m_OriginalWndProc =
            reinterpret_cast<WNDPROC>(SetWindowLongPtrW(s_Hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProcSubclass::SubclassThunk)));
    }

    void WndProcSubclass::Uninstall() {
        HWND s_Hwnd = m_Hwnd.exchange(nullptr);
        if (!s_Hwnd || !m_OriginalWndProc) {
            return;
        }
        SetWindowLongPtrW(s_Hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_OriginalWndProc));
        m_OriginalWndProc = nullptr;
    }

    LRESULT CALLBACK WndProcSubclass::SubclassThunk(HWND p_Hwnd, UINT p_Msg, WPARAM p_Wparam, LPARAM p_Lparam) {
        auto& s_Self = Instance();

        // Ask the SDK first; fall through to the original WNDPROC if it
        // didn't claim the message.
        const knt::host::WndProcResult s_Result = RenderingHost::Instance().DispatchOnWndProc(p_Hwnd, p_Msg, p_Wparam, p_Lparam);
        if (s_Result.Handled) {
            return s_Result.Value;
        }

        if (s_Self.m_OriginalWndProc) {
            return CallWindowProcW(s_Self.m_OriginalWndProc, p_Hwnd, p_Msg, p_Wparam, p_Lparam);
        }
        return DefWindowProcW(p_Hwnd, p_Msg, p_Wparam, p_Lparam);
    }
}
