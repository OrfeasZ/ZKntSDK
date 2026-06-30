#include "ImGuiRenderer.hpp"

#include "../ModSDK.hpp"
#include "Logging.hpp"
#include "Fonts.hpp"

#include <directx/d3d12.h>
#include <imgui_impl_dx12.h>
#include <windowsx.h>

#include <IconsMaterialDesign.h>

#include <Glacier/ZComponent.hpp>

namespace zknt::rendering {
    ImGuiRenderer::ImGuiRenderer() {
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_TicksPerSecond));
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_Time));

        IMGUI_CHECKVERSION();
        m_ImGuiContext = ImGui::CreateContext();

        ImGuiIO& s_ImGuiIO = ImGui::GetIO();
        s_ImGuiIO.IniFilename = nullptr;
        s_ImGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        s_ImGuiIO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        s_ImGuiIO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        s_ImGuiIO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        s_ImGuiIO.BackendPlatformName = "imgui_impl_zknt_win32";
        s_ImGuiIO.BackendRendererName = "imgui_impl_dx12";

        // Here we merge the material icon glyphs into each of our other fonts.
        ImFontConfig s_IconsConfig{};
        s_IconsConfig.MergeMode = true;
        s_IconsConfig.GlyphOffset = {0.f, 6.f};

        // Unicode ranges used by ImGui font
        static constexpr ImWchar c_TextRanges[] = {
            0x0020, 0x00FF, // Basic Latin + Latin-1 Supplement
            0x2010, 0x2027, // Punctuation
            0
        };
        static constexpr ImWchar c_IconRanges[] = {ICON_MIN_MD, ICON_MAX_16_MD, 0};

        m_FontLight =
            s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoLight_compressed_data, RobotoLight_compressed_size, 28.f, nullptr, c_TextRanges);
        s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
            MaterialIconsRegular_compressed_data, MaterialIconsRegular_compressed_size, 28.f, &s_IconsConfig, c_IconRanges
        );

        m_FontRegular = s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
            RobotoRegular_compressed_data, RobotoRegular_compressed_size, 28.f, nullptr, c_TextRanges
        );
        s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
            MaterialIconsRegular_compressed_data, MaterialIconsRegular_compressed_size, 28.f, &s_IconsConfig, c_IconRanges
        );

        m_FontMedium =
            s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium_compressed_data, RobotoMedium_compressed_size, 28.f, nullptr, c_TextRanges);
        s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
            MaterialIconsRegular_compressed_data, MaterialIconsRegular_compressed_size, 28.f, &s_IconsConfig, c_IconRanges
        );

        m_FontBold =
            s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoBold_compressed_data, RobotoBold_compressed_size, 28.f, nullptr, c_TextRanges);
        s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
            MaterialIconsRegular_compressed_data, MaterialIconsRegular_compressed_size, 28.f, &s_IconsConfig, c_IconRanges
        );

        m_FontBlack =
            s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoBlack_compressed_data, RobotoBlack_compressed_size, 28.f, nullptr, c_TextRanges);
        s_ImGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
            MaterialIconsRegular_compressed_data, MaterialIconsRegular_compressed_size, 28.f, &s_IconsConfig, c_IconRanges
        );

        s_ImGuiIO.FontDefault = m_FontRegular;

        SetupStyles();
    }

    ImGuiRenderer::~ImGuiRenderer() {
        TeardownRenderer();
        ImGui::DestroyContext(m_ImGuiContext);
        m_ImGuiContext = nullptr;
    }

    void ImGuiRenderer::SetupStyles() {
        auto& s_Style = ImGui::GetStyle();

        s_Style.ChildRounding = 0.f;
        s_Style.FrameRounding = 0.f;
        s_Style.GrabRounding = 0.f;
        s_Style.PopupRounding = 0.f;
        s_Style.ScrollbarRounding = 0.f;
        s_Style.TabRounding = 0.f;
        s_Style.WindowRounding = 0.f;
        s_Style.WindowBorderSize = 0.f;

        s_Style.WindowPadding = ImVec2(12.f, 12.f);
        s_Style.FramePadding = ImVec2(6.f, 6.f);
        s_Style.CellPadding = ImVec2(6.f, 3.f);
        s_Style.ItemSpacing = ImVec2(10.f, 6.f);
        s_Style.ItemInnerSpacing = ImVec2(10.f, 10.f);
        s_Style.TouchExtraPadding = ImVec2(0.f, 0.f);
        s_Style.IndentSpacing = 10.f;
        s_Style.ScrollbarSize = 12.f;
        s_Style.GrabMinSize = 12.f;

        s_Style.WindowBorderSize = 0.f;
        s_Style.ChildBorderSize = 0.f;
        s_Style.PopupBorderSize = 0.f;
        s_Style.FrameBorderSize = 0.f;
        s_Style.TabBorderSize = 0.f;

        // Accent color: #DEBE80 (RGB 222, 190, 128).
        const ImVec4 s_Accent = ImVec4(0.871f, 0.745f, 0.502f, 1.000f);
        const ImVec4 s_AccentHover = ImVec4(0.937f, 0.812f, 0.561f, 1.000f);
        const ImVec4 s_AccentActive = ImVec4(0.776f, 0.651f, 0.404f, 1.000f);
        const ImVec4 s_AccentSoft = ImVec4(s_Accent.x, s_Accent.y, s_Accent.z, 0.400f);
        const ImVec4 s_AccentFaint = ImVec4(s_Accent.x, s_Accent.y, s_Accent.z, 0.200f);

        ImVec4* s_Colors = s_Style.Colors;
        s_Colors[ImGuiCol_CheckMark] = s_Accent;
        s_Colors[ImGuiCol_SliderGrab] = s_Accent;
        s_Colors[ImGuiCol_SliderGrabActive] = s_AccentActive;
        s_Colors[ImGuiCol_Button] = s_AccentSoft;
        s_Colors[ImGuiCol_ButtonHovered] = s_AccentHover;
        s_Colors[ImGuiCol_ButtonActive] = s_AccentActive;
        s_Colors[ImGuiCol_Header] = s_AccentSoft;
        s_Colors[ImGuiCol_HeaderHovered] = s_AccentHover;
        s_Colors[ImGuiCol_HeaderActive] = s_AccentActive;
        s_Colors[ImGuiCol_Tab] = s_AccentSoft;
        s_Colors[ImGuiCol_TabHovered] = s_AccentHover;
        s_Colors[ImGuiCol_TabSelected] = s_Accent;
        s_Colors[ImGuiCol_TabDimmed] = s_AccentFaint;
        s_Colors[ImGuiCol_TabDimmedSelected] = s_AccentSoft;
        s_Colors[ImGuiCol_ResizeGrip] = s_AccentSoft;
        s_Colors[ImGuiCol_ResizeGripHovered] = s_AccentHover;
        s_Colors[ImGuiCol_ResizeGripActive] = s_AccentActive;
        s_Colors[ImGuiCol_Separator] = s_AccentSoft;
        s_Colors[ImGuiCol_SeparatorHovered] = s_AccentHover;
        s_Colors[ImGuiCol_SeparatorActive] = s_AccentActive;
        s_Colors[ImGuiCol_TextLink] = s_Accent;
        s_Colors[ImGuiCol_TextSelectedBg] = s_AccentSoft;
        s_Colors[ImGuiCol_DragDropTarget] = s_Accent;
        s_Colors[ImGuiCol_NavCursor] = s_Accent;
        s_Colors[ImGuiCol_PlotLinesHovered] = s_Accent;
        s_Colors[ImGuiCol_PlotHistogramHovered] = s_Accent;

        s_Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        s_Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

        s_Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
        s_Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        s_Colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.16f, 0.18f, 0.98f);

        s_Colors[ImGuiCol_Border] = ImVec4(0.32f, 0.33f, 0.36f, 0.60f);
        s_Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        s_Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.19f, 0.22f, 1.00f);
        s_Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.26f, 0.30f, 1.00f);
        s_Colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.31f, 0.35f, 1.00f);

        s_Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
        // s_Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
        s_Colors[ImGuiCol_TitleBgActive] = s_AccentSoft;
        s_Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.11f, 0.13f, 0.90f);

        s_Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.15f, 0.17f, 1.00f);

        s_Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
        s_Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.31f, 0.35f, 1.00f);
        s_Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.41f, 0.45f, 1.00f);
        s_Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.51f, 0.55f, 1.00f);

        s_Colors[ImGuiCol_TabActive] = s_AccentActive;
        s_Colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.19f, 0.22f, 1.00f);
        s_Colors[ImGuiCol_TabUnfocusedActive] = s_AccentSoft;

        s_Colors[ImGuiCol_PlotLines] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
        s_Colors[ImGuiCol_PlotHistogram] = s_Accent;

        s_Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.19f, 0.22f, 1.00f);
        s_Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.35f, 0.36f, 0.40f, 1.00f);
        s_Colors[ImGuiCol_TableBorderLight] = ImVec4(0.25f, 0.26f, 0.29f, 1.00f);
        s_Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        s_Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);

        s_Colors[ImGuiCol_NavHighlight] = s_Accent;
        s_Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        s_Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);
        s_Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
    }

    void ImGuiRenderer::SetSwapChain(IDXGISwapChain3* p_SwapChain) {
        if (p_SwapChain == m_SwapChain.m_Ref) {
            return;
        }
        m_SwapChain = p_SwapChain;
    }

    void ImGuiRenderer::SetCommandQueue(ID3D12CommandQueue* p_CommandQueue) {
        if (p_CommandQueue == m_CommandQueue.m_Ref) {
            return;
        }
        m_CommandQueue = p_CommandQueue;
    }

    void ImGuiRenderer::OnPresent(IDXGISwapChain3* p_SwapChain) {
        if (!m_CommandQueue) {
            return;
        }
        if (!SetupRenderer(p_SwapChain)) {
            Logger::Error("[ImGuiRenderer] Failed to set up renderer.");
            return;
        }
        if (!m_ImguiVisible) {
            return;
        }

        Draw();
        ImGui::Render();

        const auto s_NextFrame = (++m_FrameCounter) % m_FrameContext.size();
        auto& s_FrameCtx = m_FrameContext[s_NextFrame];

        const std::uint64_t s_PendingValue = s_FrameCtx.m_FenceValue.load(std::memory_order_acquire);
        if (s_PendingValue != 0 && s_PendingValue > m_Fence->GetCompletedValue()) {
            BreakIfFailed(m_Fence->SetEventOnCompletion(s_PendingValue, m_FenceEvent.m_Handle));
            WaitForSingleObject(m_FenceEvent.m_Handle, INFINITE);
        }

        const auto s_BackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

        s_FrameCtx.m_CommandAllocator->Reset();
        BreakIfFailed(m_CommandList->Reset(s_FrameCtx.m_CommandAllocator, nullptr));

        D3D12_RESOURCE_BARRIER s_RtBarrier{};
        s_RtBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        s_RtBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        s_RtBarrier.Transition.pResource = m_BackBuffers[s_BackBufferIndex];
        s_RtBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        s_RtBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        s_RtBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        m_CommandList->ResourceBarrier(1, &s_RtBarrier);

        const auto s_RtvHandle = m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        const D3D12_CPU_DESCRIPTOR_HANDLE s_RtvDescriptor{s_RtvHandle.ptr + s_BackBufferIndex * m_RtvDescriptorSize};

        m_CommandList->OMSetRenderTargets(1, &s_RtvDescriptor, FALSE, nullptr);
        m_CommandList->SetDescriptorHeaps(1, &m_SrvDescriptorHeap.m_Ref);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList);

        D3D12_RESOURCE_BARRIER s_PresentBarrier{};
        s_PresentBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        s_PresentBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        s_PresentBarrier.Transition.pResource = m_BackBuffers[s_BackBufferIndex];
        s_PresentBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        s_PresentBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        s_PresentBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        m_CommandList->ResourceBarrier(1, &s_PresentBarrier);
        BreakIfFailed(m_CommandList->Close());

        ID3D12CommandList* s_Lists[] = {m_CommandList.m_Ref};
        m_CommandQueue->ExecuteCommandLists(1, s_Lists);
    }

    void ImGuiRenderer::PostPresent(IDXGISwapChain3*, HRESULT p_PresentResult) {
        if (!m_CommandQueue || !m_RendererSetup) {
            return;
        }

        if (p_PresentResult == DXGI_ERROR_DEVICE_REMOVED || p_PresentResult == DXGI_ERROR_DEVICE_RESET) {
            Logger::Error("[ImGuiRenderer] Device lost after Present (hr={:#x}).", static_cast<std::uint32_t>(p_PresentResult));
            return;
        }

        auto& s_FrameCtx = m_FrameContext[m_FrameCounter.load(std::memory_order_acquire) % c_MaxRenderedFrames];
        const std::uint64_t s_NewFence = ++m_FenceValue;
        s_FrameCtx.m_FenceValue.store(s_NewFence, std::memory_order_release);
        BreakIfFailed(m_CommandQueue->Signal(m_Fence, s_NewFence));
    }

    void ImGuiRenderer::WaitForCurrentFrameToFinish() const {
        const std::uint64_t s_Fence = m_FenceValue.load(std::memory_order_acquire);
        if (s_Fence != 0 && s_Fence > m_Fence->GetCompletedValue()) {
            BreakIfFailed(m_Fence->SetEventOnCompletion(s_Fence, m_FenceEvent.m_Handle));
            WaitForSingleObject(m_FenceEvent.m_Handle, INFINITE);
        }
    }

    bool ImGuiRenderer::SetupRenderer(IDXGISwapChain3* p_SwapChain) {
        if (m_RendererSetup) {
            return true;
        }

        ScopedD3DRef<ID3D12Device> s_Device;
        if (p_SwapChain->GetDevice(REF_IID_PPV_ARGS(s_Device)) != S_OK) {
            return false;
        }

        DXGI_SWAP_CHAIN_DESC1 s_Desc{};
        if (p_SwapChain->GetDesc1(&s_Desc) != S_OK) {
            return false;
        }

        m_SwapChain = p_SwapChain;
        const auto s_BufferCount = s_Desc.BufferCount;

        {
            D3D12_DESCRIPTOR_HEAP_DESC s_RtvHeapDesc{};
            s_RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            s_RtvHeapDesc.NumDescriptors = s_BufferCount;
            s_RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            if (s_Device->CreateDescriptorHeap(&s_RtvHeapDesc, IID_PPV_ARGS(m_RtvDescriptorHeap.ReleaseAndGetPtr())) != S_OK) {
                return false;
            }
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC s_SrvHeapDesc{};
            s_SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            s_SrvHeapDesc.NumDescriptors = c_MaxSRVDescriptors;
            s_SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (s_Device->CreateDescriptorHeap(&s_SrvHeapDesc, IID_PPV_ARGS(m_SrvDescriptorHeap.ReleaseAndGetPtr())) != S_OK) {
                return false;
            }
        }

        for (UINT i = 0; i < c_MaxRenderedFrames; ++i) {
            auto& s_Frame = m_FrameContext[i];
            if (s_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(s_Frame.m_CommandAllocator.ReleaseAndGetPtr()))
                != S_OK) {
                return false;
            }
            s_Frame.m_FenceValue.store(0);
        }

        m_BackBuffers.clear();
        m_BackBuffers.resize(s_BufferCount);
        m_RtvDescriptorSize = s_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        const auto s_RtvHandle = m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < s_BufferCount; ++i) {
            if (p_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffers[i].ReleaseAndGetPtr())) != S_OK) {
                return false;
            }
            const D3D12_CPU_DESCRIPTOR_HANDLE s_Descriptor{s_RtvHandle.ptr + i * m_RtvDescriptorSize};
            s_Device->CreateRenderTargetView(m_BackBuffers[i], nullptr, s_Descriptor);
        }

        if (s_Device->CreateCommandList(
                0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameContext[0].m_CommandAllocator, nullptr, IID_PPV_ARGS(m_CommandList.ReleaseAndGetPtr())
            ) != S_OK
            || m_CommandList->Close() != S_OK) {
            return false;
        }

        if (s_Device->CreateFence(m_FenceValue.load(), D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.ReleaseAndGetPtr())) != S_OK) {
            return false;
        }
        m_FenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!m_FenceEvent) {
            return false;
        }

        if (p_SwapChain->GetHwnd(&m_Hwnd) != S_OK) {
            return false;
        }

        ImGui_ImplDX12_InitInfo s_InitInfo{};
        s_InitInfo.Device = s_Device;
        s_InitInfo.CommandQueue = m_CommandQueue;
        s_InitInfo.NumFramesInFlight = c_MaxRenderedFrames;
        s_InitInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        s_InitInfo.SrvDescriptorHeap = m_SrvDescriptorHeap;
        s_InitInfo.LegacySingleSrvCpuDescriptor = m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        s_InitInfo.LegacySingleSrvGpuDescriptor = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        if (!ImGui_ImplDX12_Init(&s_InitInfo)) {
            Logger::Error("[ImGuiRenderer] ImGui_ImplDX12_Init failed.");
            return false;
        }
        if (!ImGui_ImplDX12_CreateDeviceObjects()) {
            Logger::Error("[ImGuiRenderer] ImGui_ImplDX12_CreateDeviceObjects failed.");
            return false;
        }

        ImGuiIO& s_Io = ImGui::GetIO();
        RECT s_Rect{};
        GetClientRect(m_Hwnd, &s_Rect);
        s_Io.DisplaySize = ImVec2(static_cast<float>(s_Rect.right - s_Rect.left), static_cast<float>(s_Rect.bottom - s_Rect.top));
        s_Io.FontGlobalScale = (s_Io.DisplaySize.y / 1800.f);
        ImGui::GetMainViewport()->PlatformHandleRaw = m_Hwnd;

        m_RendererSetup = true;
        Logger::Info("[ImGuiRenderer] Renderer ready (hwnd={}).", static_cast<void*>(m_Hwnd));
        return true;
    }

    void ImGuiRenderer::TeardownRenderer() {
        if (!m_RendererSetup) {
            return;
        }
        WaitForCurrentFrameToFinish();
        ImGui_ImplDX12_Shutdown();

        m_BackBuffers.clear();
        m_CommandList.Reset();
        m_Fence.Reset();
        m_FenceEvent.Reset();
        m_RtvDescriptorHeap.Reset();
        m_SrvDescriptorHeap.Reset();
        m_SwapChain.Reset();
        m_CommandQueue.Reset();
        m_RendererSetup = false;
    }

    void ImGuiRenderer::OnReset(IDXGISwapChain3*) {
        if (!m_RendererSetup) {
            return;
        }
        WaitForCurrentFrameToFinish();
        for (auto& s_Frame : m_FrameContext) {
            s_Frame.m_FenceValue.store(m_FenceValue.load(std::memory_order_acquire));
        }
        m_BackBuffers.clear();
        ImGui_ImplDX12_InvalidateDeviceObjects();
    }

    void ImGuiRenderer::PostReset(IDXGISwapChain3* p_SwapChain) {
        if (!m_RendererSetup) {
            return;
        }
        DXGI_SWAP_CHAIN_DESC1 s_Desc{};
        if (p_SwapChain->GetDesc1(&s_Desc) != S_OK) {
            return;
        }
        ScopedD3DRef<ID3D12Device> s_Device;
        if (p_SwapChain->GetDevice(REF_IID_PPV_ARGS(s_Device)) != S_OK) {
            return;
        }

        m_BackBuffers.resize(s_Desc.BufferCount);
        m_RtvDescriptorSize = s_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        const auto s_RtvHandle = m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < m_BackBuffers.size(); ++i) {
            if (p_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_BackBuffers[i].ReleaseAndGetPtr())) != S_OK) {
                return;
            }
            const D3D12_CPU_DESCRIPTOR_HANDLE s_Descriptor{s_RtvHandle.ptr + i * m_RtvDescriptorSize};
            s_Device->CreateRenderTargetView(m_BackBuffers[i], nullptr, s_Descriptor);
        }
        ImGui_ImplDX12_CreateDeviceObjects();

        ImGuiIO& s_Io = ImGui::GetIO();
        RECT s_Rect{};
        GetClientRect(m_Hwnd, &s_Rect);
        s_Io.DisplaySize = ImVec2(static_cast<float>(s_Rect.right - s_Rect.left), static_cast<float>(s_Rect.bottom - s_Rect.top));
    }

    void ImGuiRenderer::Draw() {
        ImGui_ImplDX12_NewFrame();

        ImGuiIO& s_Io = ImGui::GetIO();

        std::int64_t s_Now = 0;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&s_Now));
        s_Io.DeltaTime = static_cast<float>(s_Now - m_Time) / m_TicksPerSecond;
        m_Time = s_Now;

        UpdateMouseData(s_Io);
        ProcessKeyEventsWorkarounds(s_Io);

        ImGui::NewFrame();
        ImGui::GetStyle().Alpha = m_ImguiHasFocus ? 1.f : 0.3f;

        ModSDK::GetInstance()->InvokeUiCallbacks(m_ImguiHasFocus.load(std::memory_order_acquire));
    }

    ImGuiMouseSource ImGuiRenderer::GetMouseSourceFromMessageExtraInfo() {
        const auto s_ExtraInfo = ::GetMessageExtraInfo();
        if ((s_ExtraInfo & 0xFFFFFF80) == 0xFF515700) {
            return ImGuiMouseSource_Pen;
        }
        if ((s_ExtraInfo & 0xFFFFFF80) == 0xFF515780) {
            return ImGuiMouseSource_TouchScreen;
        }
        return ImGuiMouseSource_Mouse;
    }

    bool ImGuiRenderer::IsVkDown(int p_Vk) {
        return (::GetKeyState(p_Vk) & 0x8000) != 0;
    }

    void ImGuiRenderer::UpdateKeyModifiers(ImGuiIO& p_Io) {
        p_Io.AddKeyEvent(ImGuiMod_Ctrl, IsVkDown(VK_CONTROL));
        p_Io.AddKeyEvent(ImGuiMod_Shift, IsVkDown(VK_SHIFT));
        p_Io.AddKeyEvent(ImGuiMod_Alt, IsVkDown(VK_MENU));
        p_Io.AddKeyEvent(ImGuiMod_Super, IsVkDown(VK_LWIN) || IsVkDown(VK_RWIN));
    }

    void ImGuiRenderer::UpdateKeyboardCodePage() {
        const auto s_KeyboardLayout = ::GetKeyboardLayout(0);
        const auto s_KeyboardLcid = MAKELCID(HIWORD(s_KeyboardLayout), SORT_DEFAULT);
        if (::GetLocaleInfoA(
                s_KeyboardLcid, LOCALE_RETURN_NUMBER | LOCALE_IDEFAULTANSICODEPAGE, reinterpret_cast<LPSTR>(&m_KeyboardCodePage),
                sizeof(m_KeyboardCodePage)
            )
            == 0) {
            m_KeyboardCodePage = CP_ACP;
        }
    }

    ImGuiKey ImGuiRenderer::KeyEventToImGuiKey(WPARAM p_Wparam, LPARAM p_Lparam) {
        if (p_Wparam == VK_RETURN && (HIWORD(p_Lparam) & KF_EXTENDED)) {
            return ImGuiKey_KeypadEnter;
        }
        const int s_Scancode = LOBYTE(HIWORD(p_Lparam));
        switch (p_Wparam) {
        case VK_TAB:
            return ImGuiKey_Tab;
        case VK_LEFT:
            return ImGuiKey_LeftArrow;
        case VK_RIGHT:
            return ImGuiKey_RightArrow;
        case VK_UP:
            return ImGuiKey_UpArrow;
        case VK_DOWN:
            return ImGuiKey_DownArrow;
        case VK_PRIOR:
            return ImGuiKey_PageUp;
        case VK_NEXT:
            return ImGuiKey_PageDown;
        case VK_HOME:
            return ImGuiKey_Home;
        case VK_END:
            return ImGuiKey_End;
        case VK_INSERT:
            return ImGuiKey_Insert;
        case VK_DELETE:
            return ImGuiKey_Delete;
        case VK_BACK:
            return ImGuiKey_Backspace;
        case VK_SPACE:
            return ImGuiKey_Space;
        case VK_RETURN:
            return ImGuiKey_Enter;
        case VK_ESCAPE:
            return ImGuiKey_Escape;
        case VK_OEM_COMMA:
            return ImGuiKey_Comma;
        case VK_OEM_PERIOD:
            return ImGuiKey_Period;
        case VK_CAPITAL:
            return ImGuiKey_CapsLock;
        case VK_SCROLL:
            return ImGuiKey_ScrollLock;
        case VK_NUMLOCK:
            return ImGuiKey_NumLock;
        case VK_SNAPSHOT:
            return ImGuiKey_PrintScreen;
        case VK_PAUSE:
            return ImGuiKey_Pause;
        case VK_NUMPAD0:
            return ImGuiKey_Keypad0;
        case VK_NUMPAD1:
            return ImGuiKey_Keypad1;
        case VK_NUMPAD2:
            return ImGuiKey_Keypad2;
        case VK_NUMPAD3:
            return ImGuiKey_Keypad3;
        case VK_NUMPAD4:
            return ImGuiKey_Keypad4;
        case VK_NUMPAD5:
            return ImGuiKey_Keypad5;
        case VK_NUMPAD6:
            return ImGuiKey_Keypad6;
        case VK_NUMPAD7:
            return ImGuiKey_Keypad7;
        case VK_NUMPAD8:
            return ImGuiKey_Keypad8;
        case VK_NUMPAD9:
            return ImGuiKey_Keypad9;
        case VK_DECIMAL:
            return ImGuiKey_KeypadDecimal;
        case VK_DIVIDE:
            return ImGuiKey_KeypadDivide;
        case VK_MULTIPLY:
            return ImGuiKey_KeypadMultiply;
        case VK_SUBTRACT:
            return ImGuiKey_KeypadSubtract;
        case VK_ADD:
            return ImGuiKey_KeypadAdd;
        case VK_LSHIFT:
            return ImGuiKey_LeftShift;
        case VK_LCONTROL:
            return ImGuiKey_LeftCtrl;
        case VK_LMENU:
            return ImGuiKey_LeftAlt;
        case VK_LWIN:
            return ImGuiKey_LeftSuper;
        case VK_RSHIFT:
            return ImGuiKey_RightShift;
        case VK_RCONTROL:
            return ImGuiKey_RightCtrl;
        case VK_RMENU:
            return ImGuiKey_RightAlt;
        case VK_RWIN:
            return ImGuiKey_RightSuper;
        case VK_APPS:
            return ImGuiKey_Menu;
        case '0':
            return ImGuiKey_0;
        case '1':
            return ImGuiKey_1;
        case '2':
            return ImGuiKey_2;
        case '3':
            return ImGuiKey_3;
        case '4':
            return ImGuiKey_4;
        case '5':
            return ImGuiKey_5;
        case '6':
            return ImGuiKey_6;
        case '7':
            return ImGuiKey_7;
        case '8':
            return ImGuiKey_8;
        case '9':
            return ImGuiKey_9;
        case 'A':
            return ImGuiKey_A;
        case 'B':
            return ImGuiKey_B;
        case 'C':
            return ImGuiKey_C;
        case 'D':
            return ImGuiKey_D;
        case 'E':
            return ImGuiKey_E;
        case 'F':
            return ImGuiKey_F;
        case 'G':
            return ImGuiKey_G;
        case 'H':
            return ImGuiKey_H;
        case 'I':
            return ImGuiKey_I;
        case 'J':
            return ImGuiKey_J;
        case 'K':
            return ImGuiKey_K;
        case 'L':
            return ImGuiKey_L;
        case 'M':
            return ImGuiKey_M;
        case 'N':
            return ImGuiKey_N;
        case 'O':
            return ImGuiKey_O;
        case 'P':
            return ImGuiKey_P;
        case 'Q':
            return ImGuiKey_Q;
        case 'R':
            return ImGuiKey_R;
        case 'S':
            return ImGuiKey_S;
        case 'T':
            return ImGuiKey_T;
        case 'U':
            return ImGuiKey_U;
        case 'V':
            return ImGuiKey_V;
        case 'W':
            return ImGuiKey_W;
        case 'X':
            return ImGuiKey_X;
        case 'Y':
            return ImGuiKey_Y;
        case 'Z':
            return ImGuiKey_Z;
        case VK_F1:
            return ImGuiKey_F1;
        case VK_F2:
            return ImGuiKey_F2;
        case VK_F3:
            return ImGuiKey_F3;
        case VK_F4:
            return ImGuiKey_F4;
        case VK_F5:
            return ImGuiKey_F5;
        case VK_F6:
            return ImGuiKey_F6;
        case VK_F7:
            return ImGuiKey_F7;
        case VK_F8:
            return ImGuiKey_F8;
        case VK_F9:
            return ImGuiKey_F9;
        case VK_F10:
            return ImGuiKey_F10;
        case VK_F11:
            return ImGuiKey_F11;
        case VK_F12:
            return ImGuiKey_F12;
        case VK_BROWSER_BACK:
            return ImGuiKey_AppBack;
        case VK_BROWSER_FORWARD:
            return ImGuiKey_AppForward;
        default:
            break;
        }
        switch (s_Scancode) {
        case 41:
            return ImGuiKey_GraveAccent;
        case 12:
            return ImGuiKey_Minus;
        case 13:
            return ImGuiKey_Equal;
        case 26:
            return ImGuiKey_LeftBracket;
        case 27:
            return ImGuiKey_RightBracket;
        case 86:
            return ImGuiKey_Oem102;
        case 43:
            return ImGuiKey_Backslash;
        case 39:
            return ImGuiKey_Semicolon;
        case 40:
            return ImGuiKey_Apostrophe;
        case 51:
            return ImGuiKey_Comma;
        case 52:
            return ImGuiKey_Period;
        case 53:
            return ImGuiKey_Slash;
        default:
            break;
        }
        return ImGuiKey_None;
    }

    void ImGuiRenderer::AddKeyEvent(ImGuiIO& p_Io, ImGuiKey p_Key, bool p_Down, int p_NativeKeycode, int p_NativeScancode) {
        p_Io.AddKeyEvent(p_Key, p_Down);
        p_Io.SetKeyEventNativeData(p_Key, p_NativeKeycode, p_NativeScancode);
    }

    void ImGuiRenderer::UpdateMouseData(ImGuiIO& p_Io) {
        const auto s_Focused = ::GetForegroundWindow();
        const bool s_IsAppFocused = (s_Focused == m_Hwnd);
        if (!s_IsAppFocused) {
            return;
        }
        if (p_Io.WantSetMousePos) {
            POINT s_Pos = {static_cast<int>(p_Io.MousePos.x), static_cast<int>(p_Io.MousePos.y)};
            if (::ClientToScreen(m_Hwnd, &s_Pos)) {
                ::SetCursorPos(s_Pos.x, s_Pos.y);
            }
        }
        if (!p_Io.WantSetMousePos && m_MouseTrackedArea == 0) {
            POINT s_Pos;
            if (::GetCursorPos(&s_Pos) && ::ScreenToClient(m_Hwnd, &s_Pos)) {
                p_Io.AddMousePosEvent(static_cast<float>(s_Pos.x), static_cast<float>(s_Pos.y));
            }
        }
    }

    void ImGuiRenderer::ProcessKeyEventsWorkarounds(ImGuiIO& p_Io) {
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && !IsVkDown(VK_LSHIFT)) {
            AddKeyEvent(p_Io, ImGuiKey_LeftShift, false, VK_LSHIFT);
        }
        if (ImGui::IsKeyDown(ImGuiKey_RightShift) && !IsVkDown(VK_RSHIFT)) {
            AddKeyEvent(p_Io, ImGuiKey_RightShift, false, VK_RSHIFT);
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && !IsVkDown(VK_LWIN)) {
            AddKeyEvent(p_Io, ImGuiKey_LeftSuper, false, VK_LWIN);
        }
        if (ImGui::IsKeyDown(ImGuiKey_RightSuper) && !IsVkDown(VK_RWIN)) {
            AddKeyEvent(p_Io, ImGuiKey_RightSuper, false, VK_RWIN);
        }
    }

    ImGuiRenderer::WndProcResult ImGuiRenderer::OnWndProc(HWND p_Hwnd, UINT p_Msg, WPARAM p_Wparam, LPARAM p_Lparam) {
        if (ImGui::GetCurrentContext() == nullptr) {
            return {false, 0};
        }

        // Layout-independent tilde toggle: compare the hardware scancode
        // (lParam bits 16-23) rather than VK_OEM_3, which moves per layout.
        const uint8_t s_ScanCode = static_cast<uint8_t>(p_Lparam >> 16);
        if ((p_Msg == WM_KEYDOWN || p_Msg == WM_SYSKEYDOWN) && s_ScanCode == 0x29) {
            const bool s_NewFocus = !m_ImguiHasFocus.load(std::memory_order_acquire);
            m_ImguiHasFocus.store(s_NewFocus, std::memory_order_release);
            if (s_NewFocus) {
                m_ImguiVisible.store(true, std::memory_order_release);
            }
        }

        (*SDK()->Globals()->ComponentManager)->m_pApplication->SetOption("DisableHardwareInput", m_ImguiHasFocus ? "1" : "0");

        if (!m_ImguiHasFocus) {
            return {false, 0};
        }

        if (p_Msg == WM_QUIT || p_Msg == WM_DESTROY || p_Msg == WM_NCDESTROY || p_Msg == WM_CLOSE) {
            m_ImguiHasFocus.store(false, std::memory_order_release);
            return {false, 0};
        }
        if (p_Msg == WM_SIZE) {
            return {false, 0};
        }

        ImGuiIO& s_Io = ImGui::GetIO();

        switch (p_Msg) {
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE: {
            const auto s_MouseSource = GetMouseSourceFromMessageExtraInfo();
            const int s_Area = (p_Msg == WM_MOUSEMOVE) ? 1 : 2;
            m_MouseHwnd = p_Hwnd;
            if (m_MouseTrackedArea != s_Area) {
                TRACKMOUSEEVENT s_Cancel = {sizeof(s_Cancel), TME_CANCEL, p_Hwnd, 0};
                TRACKMOUSEEVENT s_Track = {sizeof(s_Track), static_cast<DWORD>(s_Area == 2 ? (TME_LEAVE | TME_NONCLIENT) : TME_LEAVE), p_Hwnd, 0};
                if (m_MouseTrackedArea != 0) {
                    ::TrackMouseEvent(&s_Cancel);
                }
                ::TrackMouseEvent(&s_Track);
                m_MouseTrackedArea = s_Area;
            }
            POINT s_Pos = {static_cast<LONG>(GET_X_LPARAM(p_Lparam)), static_cast<LONG>(GET_Y_LPARAM(p_Lparam))};
            if (p_Msg == WM_NCMOUSEMOVE && ::ScreenToClient(p_Hwnd, &s_Pos) == FALSE) {
                break;
            }
            s_Io.AddMouseSourceEvent(s_MouseSource);
            s_Io.AddMousePosEvent(static_cast<float>(s_Pos.x), static_cast<float>(s_Pos.y));
            break;
        }
        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE: {
            if (const int s_Area = (p_Msg == WM_MOUSELEAVE) ? 1 : 2; m_MouseTrackedArea == s_Area) {
                if (m_MouseHwnd == p_Hwnd) {
                    m_MouseHwnd = nullptr;
                }
                m_MouseTrackedArea = 0;
                s_Io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
            }
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK: {
            const auto s_MouseSource = GetMouseSourceFromMessageExtraInfo();
            int s_Button = 0;
            if (p_Msg == WM_LBUTTONDOWN || p_Msg == WM_LBUTTONDBLCLK) {
                s_Button = 0;
            }
            if (p_Msg == WM_RBUTTONDOWN || p_Msg == WM_RBUTTONDBLCLK) {
                s_Button = 1;
            }
            if (p_Msg == WM_MBUTTONDOWN || p_Msg == WM_MBUTTONDBLCLK) {
                s_Button = 2;
            }
            if (p_Msg == WM_XBUTTONDOWN || p_Msg == WM_XBUTTONDBLCLK) {
                s_Button = (GET_XBUTTON_WPARAM(p_Wparam) == XBUTTON1) ? 3 : 4;
            }
            HWND s_Capture = ::GetCapture();
            if (m_MouseButtonsDown != 0 && s_Capture != p_Hwnd) {
                m_MouseButtonsDown = 0;
            }
            if (m_MouseButtonsDown == 0 && s_Capture == nullptr) {
                ::SetCapture(p_Hwnd);
            }
            m_MouseButtonsDown |= 1 << s_Button;
            s_Io.AddMouseSourceEvent(s_MouseSource);
            s_Io.AddMouseButtonEvent(s_Button, true);
            break;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP: {
            const auto s_MouseSource = GetMouseSourceFromMessageExtraInfo();
            int s_Button = 0;
            if (p_Msg == WM_LBUTTONUP) {
                s_Button = 0;
            }
            if (p_Msg == WM_RBUTTONUP) {
                s_Button = 1;
            }
            if (p_Msg == WM_MBUTTONUP) {
                s_Button = 2;
            }
            if (p_Msg == WM_XBUTTONUP) {
                s_Button = (GET_XBUTTON_WPARAM(p_Wparam) == XBUTTON1) ? 3 : 4;
            }
            m_MouseButtonsDown &= ~(1 << s_Button);
            if (m_MouseButtonsDown == 0 && ::GetCapture() == p_Hwnd) {
                ::ReleaseCapture();
            }
            s_Io.AddMouseSourceEvent(s_MouseSource);
            s_Io.AddMouseButtonEvent(s_Button, false);
            break;
        }
        case WM_MOUSEWHEEL:
            s_Io.AddMouseWheelEvent(0.0f, static_cast<float>(GET_WHEEL_DELTA_WPARAM(p_Wparam)) / static_cast<float>(WHEEL_DELTA));
            break;
        case WM_MOUSEHWHEEL:
            s_Io.AddMouseWheelEvent(-static_cast<float>(GET_WHEEL_DELTA_WPARAM(p_Wparam)) / static_cast<float>(WHEEL_DELTA), 0.0f);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP: {
            const bool s_IsKeyDown = (p_Msg == WM_KEYDOWN || p_Msg == WM_SYSKEYDOWN);
            if (p_Wparam < 256) {
                UpdateKeyModifiers(s_Io);
                const ImGuiKey s_Key = KeyEventToImGuiKey(p_Wparam, p_Lparam);
                const int s_Vk = static_cast<int>(p_Wparam);
                const int s_Scancode = LOBYTE(HIWORD(p_Lparam));
                if (s_Key == ImGuiKey_PrintScreen && !s_IsKeyDown) {
                    AddKeyEvent(s_Io, s_Key, true, s_Vk, s_Scancode);
                }
                if (s_Key != ImGuiKey_None) {
                    AddKeyEvent(s_Io, s_Key, s_IsKeyDown, s_Vk, s_Scancode);
                }
                if (s_Vk == VK_SHIFT) {
                    if (IsVkDown(VK_LSHIFT) == s_IsKeyDown) {
                        AddKeyEvent(s_Io, ImGuiKey_LeftShift, s_IsKeyDown, VK_LSHIFT, s_Scancode);
                    }
                    if (IsVkDown(VK_RSHIFT) == s_IsKeyDown) {
                        AddKeyEvent(s_Io, ImGuiKey_RightShift, s_IsKeyDown, VK_RSHIFT, s_Scancode);
                    }
                }
                else if (s_Vk == VK_CONTROL) {
                    if (IsVkDown(VK_LCONTROL) == s_IsKeyDown) {
                        AddKeyEvent(s_Io, ImGuiKey_LeftCtrl, s_IsKeyDown, VK_LCONTROL, s_Scancode);
                    }
                    if (IsVkDown(VK_RCONTROL) == s_IsKeyDown) {
                        AddKeyEvent(s_Io, ImGuiKey_RightCtrl, s_IsKeyDown, VK_RCONTROL, s_Scancode);
                    }
                }
                else if (s_Vk == VK_MENU) {
                    if (IsVkDown(VK_LMENU) == s_IsKeyDown) {
                        AddKeyEvent(s_Io, ImGuiKey_LeftAlt, s_IsKeyDown, VK_LMENU, s_Scancode);
                    }
                    if (IsVkDown(VK_RMENU) == s_IsKeyDown) {
                        AddKeyEvent(s_Io, ImGuiKey_RightAlt, s_IsKeyDown, VK_RMENU, s_Scancode);
                    }
                }
            }
            break;
        }
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            s_Io.AddFocusEvent(p_Msg == WM_SETFOCUS);
            break;
        case WM_INPUTLANGCHANGE:
            UpdateKeyboardCodePage();
            break;
        case WM_CHAR:
            if (::IsWindowUnicode(p_Hwnd)) {
                if (p_Wparam > 0 && p_Wparam < 0x10000) {
                    s_Io.AddInputCharacterUTF16(static_cast<unsigned short>(p_Wparam));
                }
            }
            else {
                wchar_t s_Wch = 0;
                ::MultiByteToWideChar(m_KeyboardCodePage, MB_PRECOMPOSED, reinterpret_cast<char*>(&p_Wparam), 1, &s_Wch, 1);
                s_Io.AddInputCharacter(s_Wch);
            }
            break;
        }

        // Eat the message: don't pass it down to the game.
        return {true, DefWindowProcW(p_Hwnd, p_Msg, p_Wparam, p_Lparam)};
    }
}
