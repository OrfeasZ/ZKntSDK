#pragma once

#include <directx/d3d12.h>

#include <cstdint>

#include "imgui.h"

#include <implot.h>

#include "Common.hpp"

#include "Glacier/ZMath.hpp"

namespace zknt {
    struct ImGuiTexture {
        std::uint64_t m_Id = 0;
        std::uint32_t m_Width = 0;
        std::uint32_t m_Height = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE m_SRVCPUDescriptor{};
        D3D12_GPU_DESCRIPTOR_HANDLE m_SRVGPUDescriptor{};
    };

    class IImGuiRenderer {
      public:
        virtual ~IImGuiRenderer() = default;

        virtual bool IsVisible() const = 0;

        virtual void SetFocus(bool p_HasFocus) = 0;

        virtual ImGuiContext* GetContext() const = 0;
        virtual ImPlotContext* GetImPlotContext() const = 0;

        virtual ImGuiMemAllocFunc GetMemAlloc() const = 0;
        virtual ImGuiMemFreeFunc GetMemFree() const = 0;
        virtual void* GetAllocatorUserData() const = 0;

        virtual ImFont* GetLightFont() const = 0;
        virtual ImFont* GetRegularFont() const = 0;
        virtual ImFont* GetMediumFont() const = 0;
        virtual ImFont* GetBoldFont() const = 0;
        virtual ImFont* GetBlackFont() const = 0;
    };
}
