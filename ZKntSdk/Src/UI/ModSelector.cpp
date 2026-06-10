#include "ModSelector.hpp"

#include "../ModLoader.hpp"
#include "../ModSDK.hpp"
#include "Util/StringUtils.hpp"

#include <algorithm>
#include <imgui.h>

namespace zknt::ui {
    ModSelector::ModSelector() {
        InitializeSRWLock(&m_Lock);
    }

    void ModSelector::UpdateAvailableMods(
        const std::unordered_set<std::string>& p_Mods, const std::unordered_set<std::string>& p_IncompatibleMods,
        const std::unordered_set<std::string>& p_ActiveMods
    ) {
        AcquireSRWLockExclusive(&m_Lock);

        m_IncompatibleMods = p_IncompatibleMods;
        m_AvailableMods.clear();
        m_AvailableMods.reserve(p_Mods.size());
        for (const auto& s_Name : p_Mods) {
            m_AvailableMods.push_back({s_Name, p_ActiveMods.contains(knt::util::ToLowerCase(s_Name))});
        }
        std::ranges::sort(m_AvailableMods, [](const AvailableMod& a, const AvailableMod& b) {
            return knt::util::ToLowerCase(a.m_Name) < knt::util::ToLowerCase(b.m_Name);
        });

        // Auto-open on first launch if nothing is enabled yet.
        m_ShouldShow = p_ActiveMods.empty();

        ReleaseSRWLockExclusive(&m_Lock);
    }

    void ModSelector::Draw(bool p_HasFocus) {
        if (m_ShouldShow) {
            m_Open = true;
            m_ShouldShow = false;
        }

        if (!m_Open || !p_HasFocus) {
            return;
        }

        if (ImGui::Begin("MODS", &m_Open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
            ImGui::TextUnformatted(
                "Select the mods you'd like to use.\n"
                "Some mods might require a game restart to function properly."
            );
            ImGui::Separator();

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);

            AcquireSRWLockShared(&m_Lock);
            for (auto& s_Mod : m_AvailableMods) {
                ImGui::Checkbox(s_Mod.m_Name.c_str(), &s_Mod.m_Enabled);
            }

            if (!m_IncompatibleMods.empty()) {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "INCOMPATIBLE");
                for (const auto& s_Mod : m_IncompatibleMods) {
                    ImGui::BeginDisabled(true);
                    bool s_Off = false;
                    ImGui::Checkbox(s_Mod.c_str(), &s_Off);
                    ImGui::EndDisabled();
                }
            }
            ReleaseSRWLockShared(&m_Lock);

            ImGui::EndChild();
            ImGui::Separator();

            if (ImGui::Button("OK")) {
                ApplySelectedMods();
                m_Open = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Rescan Mods")) {
                ModSDK::GetInstance()->GetModLoader()->ScanAvailableMods();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reload All")) {
                ModSDK::GetInstance()->GetModLoader()->ReloadAllMods();
            }
        }
        ImGui::End();
    }

    void ModSelector::ApplySelectedMods() {
        std::unordered_set<std::string> s_Selected;
        AcquireSRWLockShared(&m_Lock);
        for (const auto& s_Mod : m_AvailableMods) {
            if (s_Mod.m_Enabled) {
                s_Selected.insert(s_Mod.m_Name);
            }
        }
        ReleaseSRWLockShared(&m_Lock);
        ModSDK::GetInstance()->GetModLoader()->SetActiveMods(s_Selected);
    }
}
