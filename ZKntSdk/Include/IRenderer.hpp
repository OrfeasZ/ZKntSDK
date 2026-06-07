#pragma once

#include "imgui.h"

#include "Common.hpp"

#include <cstdint>

struct ImFont;
struct ImGuiContext;

namespace zknt {
    struct ImGuiTexture {
        std::uint64_t m_Id = 0;
        std::uint32_t m_Width = 0;
        std::uint32_t m_Height = 0;
    };

    class IRenderer {
      public:
        virtual ~IRenderer() = default;

        virtual bool IsVisible() const = 0;
        virtual ImGuiContext* GetImGuiContext() const = 0;
        virtual ImFont* GetLightFont() const = 0;
        virtual ImFont* GetRegularFont() const = 0;
        virtual ImFont* GetMediumFont() const = 0;
        virtual ImFont* GetBoldFont() const = 0;
        virtual ImFont* GetBlackFont() const = 0;
    };
}
