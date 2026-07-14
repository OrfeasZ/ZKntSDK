#include "MainMenu.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "ModLoader.hpp"
#include "ModSDK.hpp"
#include "ModSelector.hpp"
#include "IPluginInterface.hpp"

namespace zknt::ui {
    void MainMenu::Draw(bool p_HasFocus) {
        if (!p_HasFocus) {
            return;
        }

        const auto& s_ImGuiIO = ImGui::GetIO();

        ImGui::SetNextWindowSize(ImVec2(s_ImGuiIO.DisplaySize.x, 0.f), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.f, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.14f, 0.14f, 0.14f, 1.00f));

        ImGui::Begin(
            "#MainMenu", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                | ImGuiWindowFlags_HorizontalScrollbar
        );

        // Lay out children horizontally so each menu button sits next to
        // the previous one instead of stacking vertically.
        ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;

        ImGui::AlignTextToFramePadding();
        ImGui::Text("ZKnt SDK");

        ImGui::AlignTextToFramePadding();
        ImGui::Text("~ to toggle this menu");

        if (ImGui::Button("MODS")) {
            if (auto* s_Selector = ModSDK::GetInstance()->GetUIModSelector(); s_Selector) {
                s_Selector->Show();
            }
        }

        if (auto* s_Loader = ModSDK::GetInstance()->GetModLoader(); s_Loader) {
            for (auto* s_Plugin : s_Loader->GetLoadedMods()) {
                if (s_Plugin) {
                    s_Plugin->OnDrawMenu(ModSDK::GetInstance()->GetImGuiRenderer());
                }
            }
        }

        ImGui::End();

        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor();
    }
}
