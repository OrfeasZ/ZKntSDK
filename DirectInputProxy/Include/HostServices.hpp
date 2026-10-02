#pragma once

#include <Windows.h>
#include <dxgi1_5.h>
#include <directx/d3d12.h>

namespace knt::host {
    struct WndProcResult {
        bool Handled;
        LRESULT Value;
    };

    // POD callback table populated by the SDK and handed to the proxy. All
    // callbacks may run on the render or message thread; the proxy gates
    // dispatch with a ReloadGate so they never overlap a hot-reload.
    struct RenderingCallbacks {
        void (*SetSwapChain)(IDXGISwapChain4* p_SwapChain);
        void (*SetCommandQueue)(ID3D12CommandQueue* p_CommandQueue);
        void (*OnPresent)(IDXGISwapChain4* p_SwapChain);
        void (*PostPresent)(IDXGISwapChain4* p_SwapChain, HRESULT p_PresentResult);
        void (*OnReset)(IDXGISwapChain4* p_SwapChain);
        void (*PostReset)(IDXGISwapChain4* p_SwapChain);
        WndProcResult (*OnWndProc)(HWND p_Hwnd, UINT p_Message, WPARAM p_Wparam, LPARAM p_Lparam);
    };

    // POD service table the proxy exposes to the SDK via cr_plugin::userdata.
    // Both register/unregister calls block until any in-flight callback returns.
    struct HostServices {
        void (*RegisterRenderingCallbacks)(const RenderingCallbacks* p_Callbacks);
        void (*UnregisterRenderingCallbacks)();
        bool (*IsEngineInitialized)();
        void (*SetEngineInitialized)(bool p_Initialized);
    };
}
