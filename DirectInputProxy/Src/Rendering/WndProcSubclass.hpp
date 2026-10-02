#pragma once

#include <Windows.h>
#include <atomic>
#include <dxgi1_5.h>

namespace knt::rendering {
    // Subclasses the game window via SetWindowLongPtrW so SDK callbacks see
    // input before the game's WNDPROC does. SetWindowSubclass is avoided to
    // sit at the top of the chain and not pull in comctl32.
    class WndProcSubclass {
      public:
        static WndProcSubclass& Instance();

        // First call resolves the swap chain's HWND and installs the subclass;
        // subsequent calls are no-ops.
        void EnsureInstalled(IDXGISwapChain4* p_SwapChain);

        void Uninstall();

      private:
        WndProcSubclass() = default;

        static LRESULT CALLBACK SubclassThunk(HWND p_Hwnd, UINT p_Msg, WPARAM p_Wparam, LPARAM p_Lparam);

        std::atomic<HWND> m_Hwnd{nullptr};
        WNDPROC m_OriginalWndProc = nullptr;
    };
}
