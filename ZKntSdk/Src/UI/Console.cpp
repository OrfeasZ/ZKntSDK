#include "Console.hpp"

#include <imgui.h>

#include <IModSDK.hpp>
#include <Logging.hpp>
#include <Util/StringUtils.hpp>
#include <Events.hpp>

namespace zknt::ui {
    Console::Console() {
        InitializeSRWLock(&m_Lock);
    }

    void Console::Draw(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) {
        if (!p_HasFocus) {
            return;
        }

        ImGui::PushFont(p_Renderer->GetBlackFont());
        const auto s_IsWindowExpanded =
            ImGui::Begin("CONSOLE", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
        ImGui::PushFont(p_Renderer->GetRegularFont());

        ImGui::SetWindowCollapsed(true, ImGuiCond_Once);

        const auto& s_ImGuiIO = ImGui::GetIO();
        ImGui::SetWindowSize(ImVec2(s_ImGuiIO.DisplaySize.x - 60, 400), ImGuiCond_Always);
        ImGui::SetWindowPos(ImVec2(30, 80 * (s_ImGuiIO.DisplaySize.y / 1800.f)), ImGuiCond_Always);

        if (s_IsWindowExpanded) {
            // Render the list of log lines.
            const float s_FooterHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -s_FooterHeight), false);

            AcquireSRWLockShared(&m_Lock);

            ImGui::PushTextWrapPos();

            for (auto& s_LogLine : m_LogLines) {
                ImVec4 s_Color;
                bool s_Colored = false;

                switch (s_LogLine.Level) {
                case spdlog::level::trace:
                    s_Color = ImVec4(168.f / 255.f, 61.f / 255.f, 1.f, 1.f);
                    s_Colored = true;
                    break;

                case spdlog::level::debug:
                    s_Color = ImVec4(61.f / 255.f, 129.f / 255.f, 1.f, 1.f);
                    s_Colored = true;
                    break;

                case spdlog::level::info:
                    break;

                case spdlog::level::warn:
                    s_Color = ImVec4(1.f, 168.f / 255.f, 61.f / 255.f, 1.f);
                    s_Colored = true;
                    break;

                case spdlog::level::err:
                case spdlog::level::critical:
                    s_Color = ImVec4(1.f, 69.f / 255.f, 69.f / 255.f, 1.f);
                    s_Colored = true;
                    break;

                case spdlog::level::off:
                    break;
                }

                if (s_Colored) {
                    ImGui::PushStyleColor(ImGuiCol_Text, s_Color);
                }

                ImGui::TextUnformatted(s_LogLine.Text.c_str(), s_LogLine.Text.c_str() + s_LogLine.Text.size());

                if (s_Colored) {
                    ImGui::PopStyleColor();
                }
            }

            ImGui::PopTextWrapPos();

            // Auto scroll to bottom.
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0);
            }

            ReleaseSRWLockShared(&m_Lock);

            ImGui::EndChild();

            // Render the text input.
            ImGui::Separator();

            char s_Command[2048] = {};
            ImGui::InputText("##consoleCommand", s_Command, IM_ARRAYSIZE(s_Command), ImGuiInputTextFlags_EnterReturnsTrue);

            if (ImGui::IsItemDeactivatedAfterEdit()) {
                Logger::Info("> {}", s_Command);

                TArray<ZString> s_Args{};
                std::vector<std::string> s_Split = knt::util::Split(s_Command, " ");

                for (const std::string& arg : s_Split) {
                    s_Args.push_back(*new ZString(arg));
                }

                SDK()->Events()->OnConsoleCommand->Call(s_Args);

                s_Command[0] = '\0';
                ImGui::SetKeyboardFocusHere(-1);
            }

            ImGui::SetItemDefaultFocus();
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    void Console::AddLogLine(spdlog::level::level_enum p_Level, const std::string& p_Text) {
        AcquireSRWLockExclusive(&m_Lock);

        m_LogLines.push_back(LogLine{p_Level, std::string(p_Text.c_str(), p_Text.size())});

        ReleaseSRWLockExclusive(&m_Lock);
    }
}
