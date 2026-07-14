#pragma once

#include "IImGuiRenderer.hpp"
#include "D3DUtils.hpp"

#include <array>
#include <atomic>
#include <directx/d3d12.h>
#include <dxgi1_4.h>
#include <vector>

#include <imgui.h>

namespace zknt::rendering {
    class ImGuiRenderer final : public IImGuiRenderer {
      public:
        struct FrameContext {
            ScopedD3DRef<ID3D12CommandAllocator> m_CommandAllocator;
            std::atomic<std::uint64_t> m_FenceValue{0};
        };

        ImGuiRenderer();
        ~ImGuiRenderer() override;

        ImGuiRenderer(const ImGuiRenderer&) = delete;
        ImGuiRenderer& operator=(const ImGuiRenderer&) = delete;

        // Called from proxy callbacks.
        void SetSwapChain(IDXGISwapChain3* p_SwapChain);
        void SetCommandQueue(ID3D12CommandQueue* p_CommandQueue);
        void OnPresent(IDXGISwapChain3* p_SwapChain);
        void PostPresent(IDXGISwapChain3* p_SwapChain, HRESULT p_PresentResult);
        void OnReset(IDXGISwapChain3* p_SwapChain);
        void PostReset(IDXGISwapChain3* p_SwapChain);

        // Returns {Handled = true, Value = lresult} when ImGui consumes the
        // message (game must not see it). Otherwise {false, 0}; game's
        // original WNDPROC runs.
        struct WndProcResult {
            bool m_Handled;
            LRESULT m_Value;
        };
        WndProcResult OnWndProc(HWND p_Hwnd, UINT p_Msg, WPARAM p_Wparam, LPARAM p_Lparam);

        // IRenderer
        bool IsVisible() const override {
            return m_ImguiVisible;
        }

        ImGuiContext* GetContext() const override {
            return m_ImGuiContext;
        }

        ImGuiMemAllocFunc GetMemAlloc() const override {
            ImGuiMemAllocFunc s_AllocFunc;
            ImGuiMemFreeFunc s_FreeFunc;
            void* s_UserData;
            ImGui::GetAllocatorFunctions(&s_AllocFunc, &s_FreeFunc, &s_UserData);

            return s_AllocFunc;
        }

        ImGuiMemFreeFunc GetMemFree() const override {
            ImGuiMemAllocFunc s_AllocFunc;
            ImGuiMemFreeFunc s_FreeFunc;
            void* s_UserData;
            ImGui::GetAllocatorFunctions(&s_AllocFunc, &s_FreeFunc, &s_UserData);

            return s_FreeFunc;
        }

        void* GetAllocatorUserData() const override {
            ImGuiMemAllocFunc s_AllocFunc;
            ImGuiMemFreeFunc s_FreeFunc;
            void* s_UserData;
            ImGui::GetAllocatorFunctions(&s_AllocFunc, &s_FreeFunc, &s_UserData);

            return s_UserData;
        }

        ImFont* GetLightFont() const override {
            return m_FontLight;
        }

        ImFont* GetRegularFont() const override {
            return m_FontRegular;
        }

        ImFont* GetMediumFont() const override {
            return m_FontMedium;
        }

        ImFont* GetBoldFont() const override {
            return m_FontBold;
        }

        ImFont* GetBlackFont() const override {
            return m_FontBlack;
        }

      private:
        bool SetupRenderer(IDXGISwapChain3* p_SwapChain);
        void TeardownRenderer();
        void Draw();
        void SetupStyles();
        void WaitForCurrentFrameToFinish() const;

        // Input plumbing helpers (ported from ZHM's WndProc handler).
        static ImGuiMouseSource GetMouseSourceFromMessageExtraInfo();
        static bool IsVkDown(int p_Vk);
        static void UpdateKeyModifiers(ImGuiIO& p_ImGuiIO);
        void UpdateKeyboardCodePage();
        static ImGuiKey KeyEventToImGuiKey(WPARAM p_Wparam, LPARAM p_Lparam);
        static void AddKeyEvent(ImGuiIO& p_ImGuiIO, ImGuiKey p_Key, bool p_Down, int p_NativeKeycode, int p_NativeScancode = -1);
        void UpdateMouseData(ImGuiIO& p_ImGuiIO);
        void ProcessKeyEventsWorkarounds(ImGuiIO& p_ImGuiIO);

        // Maximum number of SRV (Shader Resource View) descriptors that can
        // be allocated in the global CBV/SRV/UAV descriptor heap.
        static constexpr std::size_t c_MaxSRVDescriptors = 1024;
        // The maximum number of frames that can be buffered for render.
        static constexpr std::size_t c_MaxRenderedFrames = 4;

        bool m_RendererSetup = false;

        ScopedD3DRef<IDXGISwapChain3> m_SwapChain;
        ScopedD3DRef<ID3D12CommandQueue> m_CommandQueue;
        HWND m_Hwnd = nullptr;

        std::uint32_t m_RtvDescriptorSize = 0;
        ScopedD3DRef<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
        ScopedD3DRef<ID3D12DescriptorHeap> m_SrvDescriptorHeap;
        UINT m_NextSRVIndex = 1;

        std::array<FrameContext, c_MaxRenderedFrames> m_FrameContext{};
        std::vector<ScopedD3DRef<ID3D12Resource>> m_BackBuffers;
        ScopedD3DRef<ID3D12GraphicsCommandList> m_CommandList;

        ScopedD3DRef<ID3D12Fence> m_Fence;
        SafeHandle m_FenceEvent;

        std::atomic<std::uint32_t> m_FrameCounter{0};
        std::atomic<std::uint64_t> m_FenceValue{0};

        std::int64_t m_Time = 0;
        std::int64_t m_TicksPerSecond = 0;

        ImGuiContext* m_ImGuiContext = nullptr;

        ImFont* m_FontLight = nullptr;
        ImFont* m_FontRegular = nullptr;
        ImFont* m_FontMedium = nullptr;
        ImFont* m_FontBold = nullptr;
        ImFont* m_FontBlack = nullptr;

        std::atomic<bool> m_ImguiHasFocus{false};
        std::atomic<bool> m_ImguiVisible{true};

        // Input state for WndProc handler.
        HWND m_MouseHwnd = nullptr;
        int m_MouseTrackedArea = 0; // 0: not tracked, 1: client area, 2: non-client area
        int m_MouseButtonsDown = 0;
        UINT32 m_KeyboardCodePage = CP_ACP;
    };
}
