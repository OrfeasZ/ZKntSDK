#pragma once

#include <cstdint>

#include "imgui.h"

#include "Common.hpp"

#include "Glacier/ZMath.hpp"

namespace zknt {
    struct ImGuiTexture {
        std::uint64_t m_Id = 0;
        std::uint32_t m_Width = 0;
        std::uint32_t m_Height = 0;
    };

    class IImGuiRenderer {
      public:
        virtual ~IImGuiRenderer() = default;

        virtual bool IsVisible() const = 0;

        virtual ImGuiContext* GetContext() const = 0;

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
