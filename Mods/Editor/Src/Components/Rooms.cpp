#include "Editor.hpp"

#include <IconsMaterialDesign.h>

#include <Glacier/ZRoom.hpp>

#include <Util/StringUtils.hpp>

void Editor::DrawRoomsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) {
    if (!p_HasFocus || !m_ShowRoomsWindow) {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    const auto s_IsWindowExpanded = ImGui::Begin(ICON_MD_MEETING_ROOM " Rooms", &m_ShowRoomsWindow);
    ImGui::PushFont(p_Renderer->GetRegularFont());

    if (!s_IsWindowExpanded) {
        return;
    }

    if (!m_CachedEntityTree || !m_CachedEntityTree->m_Entity) {
        if (ImGui::Button("Build entity tree")) {
            UpdateEntities();
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        return;
    }

    auto s_LocalPlayer = SDK()->Globals()->LocalPlayerData->m_pCharacterImpl->m_pCharacter;

    if (s_LocalPlayer) {
        uint16 s_CurrentRoomEntityIndex;
        SDK()->Functions()->ZRoomManagerMain_GetRoomFromPoint->Call(
            SDK()->Globals()->RoomManagerMain, s_CurrentRoomEntityIndex, s_LocalPlayer.m_pInterfaceRef->GetObjectToWorldMatrix().Pos
        );

        const ZRoomEntity* s_CurrentRoomEntity = SDK()->Globals()->RoomManagerMain->m_RoomEntities[s_CurrentRoomEntityIndex];
        ZEntityRef s_CurrentRoomEntityRef;

        s_CurrentRoomEntity->GetID(s_CurrentRoomEntityRef);

        auto s_Iterator = m_CachedEntityTreeMap.find(s_CurrentRoomEntityRef);

        if (s_Iterator != m_CachedEntityTreeMap.end()) {
            std::shared_ptr<EntityTreeNode> s_EntityTreeNode = s_Iterator->second;

            ImGui::Text("Room player is in: %s", s_EntityTreeNode->m_Name.c_str());

            ImGui::SameLine();

            if (ImGui::SmallButton("Select in entity tree##Player")) {
                OnSelectEntity(s_CurrentRoomEntityRef, true, std::nullopt);
            }

            ImGui::Separator();
        }
    }

    const auto s_CurrentCamera = SDK()->Functions()->GetCurrentCamera->Call();

    if (s_CurrentCamera) {
        uint16 s_CurrentRoomEntityIndex;
        SDK()->Functions()->ZRoomManagerMain_GetRoomFromPoint->Call(
            SDK()->Globals()->RoomManagerMain, s_CurrentRoomEntityIndex, s_CurrentCamera->GetObjectToWorldMatrix().Pos
        );

        const ZRoomEntity* s_CurrentRoomEntity = SDK()->Globals()->RoomManagerMain->m_RoomEntities[s_CurrentRoomEntityIndex];
        ZEntityRef s_CurrentRoomEntityRef;

        s_CurrentRoomEntity->GetID(s_CurrentRoomEntityRef);

        auto s_Iterator = m_CachedEntityTreeMap.find(s_CurrentRoomEntityRef);

        if (s_Iterator != m_CachedEntityTreeMap.end()) {
            std::shared_ptr<EntityTreeNode> s_EntityTreeNode = s_Iterator->second;

            ImGui::Text("Room camera is in: %s", s_EntityTreeNode->m_Name.c_str());

            ImGui::SameLine();

            if (ImGui::SmallButton("Select in entity tree##Camera")) {
                OnSelectEntity(s_CurrentRoomEntityRef, true, std::nullopt);
            }

            ImGui::Separator();
        }
    }

    std::vector<ZEntityRef> s_SortedRoomEntities;

    s_SortedRoomEntities.reserve(SDK()->Globals()->RoomManagerMain->m_RoomEntities.size());

    for (const auto s_RoomEntity : SDK()->Globals()->RoomManagerMain->m_RoomEntities) {
        if (!s_RoomEntity) {
            continue;
        }

        ZEntityRef s_GateEntityRef;

        s_RoomEntity->GetID(s_GateEntityRef);
        s_SortedRoomEntities.push_back(s_GateEntityRef);
    }

    std::sort(s_SortedRoomEntities.begin(), s_SortedRoomEntities.end(), [&](const ZEntityRef& a, const ZEntityRef& b) {
        auto s_IteratorA = m_CachedEntityTreeMap.find(a);
        auto s_IteratorB = m_CachedEntityTreeMap.find(b);

        if (s_IteratorA == m_CachedEntityTreeMap.end()) {
            return false;
        }

        if (s_IteratorB == m_CachedEntityTreeMap.end()) {
            return true;
        }

        return s_IteratorA->second->m_Name < s_IteratorB->second->m_Name;
    });

    ImGui::Checkbox("Show only visible rooms", &m_ShowOnlyVisibleRooms);
    ImGui::Checkbox("Show only visible gates", &m_ShowOnlyVisibleGates);

    static char s_RoomName[2048]{""};

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Room name");
    ImGui::SameLine();

    ImGui::InputText("##RoomName", s_RoomName, sizeof(s_RoomName));

    static char s_GateName[2048]{""};

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Gate name");
    ImGui::SameLine();

    ImGui::InputText("##GateName", s_GateName, sizeof(s_GateName));

    for (size_t i = 0; i < s_SortedRoomEntities.size(); ++i) {
        ZEntityRef& s_RoomEntityRef = s_SortedRoomEntities[i];
        ZRoomEntity* s_RoomEntity = s_RoomEntityRef.QueryInterface<ZRoomEntity>();
        bool s_IsRoomVisible = false;

        if (m_ShowOnlyVisibleRooms) {
            for (const auto& s_RoomEntityIndex : SDK()->Globals()->RenderManager->m_pRoomManagerRender->m_RoomsVisible) {
                ZRoomEntity* s_RoomEntity2 = SDK()->Globals()->RoomManagerMain->m_RoomEntities[s_RoomEntityIndex];

                if (s_RoomEntity2 == s_RoomEntity) {
                    s_IsRoomVisible = true;
                    break;
                }
            }

            if (!s_IsRoomVisible) {
                continue;
            }
        }

        auto s_Iterator = m_CachedEntityTreeMap.find(s_RoomEntityRef);

        if (s_Iterator == m_CachedEntityTreeMap.end()) {
            continue;
        }

        std::shared_ptr<EntityTreeNode> s_RoomEntityTreeNode = s_Iterator->second;

        if (!knt::util::FindSubstring(s_RoomEntityTreeNode->m_Name, s_RoomName)) {
            continue;
        }

        ImGuiTreeNodeFlags s_RoomNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

        ImGui::SetNextItemAllowOverlap();

        ImGui::PushID(i);

        bool s_IsTreeNodeOpen = ImGui::TreeNodeEx(s_RoomEntityTreeNode->m_Name.c_str(), s_RoomNodeFlags);

        ImGui::SameLine();

        if (ImGui::SmallButton("Select in entity tree")) {
            OnSelectEntity(s_RoomEntityRef, true, std::nullopt);
        }

        if (s_IsTreeNodeOpen) {
            if (ImGui::TreeNode("Gates")) {
                std::vector<ZEntityRef> s_SortedGateEntities;

                s_SortedGateEntities.reserve(s_RoomEntity->m_Gates.size());

                for (const auto s_GateEntity : s_RoomEntity->m_Gates) {
                    if (!s_GateEntity) {
                        continue;
                    }

                    ZEntityRef gateRef;

                    s_GateEntity->GetID(gateRef);
                    s_SortedGateEntities.push_back(gateRef);
                }

                std::sort(s_SortedGateEntities.begin(), s_SortedGateEntities.end(), [&](const ZEntityRef& a, const ZEntityRef& b) {
                    auto s_IteratorA = m_CachedEntityTreeMap.find(a);
                    auto s_IteratorB = m_CachedEntityTreeMap.find(b);

                    if (s_IteratorA == m_CachedEntityTreeMap.end()) {
                        return false;
                    }

                    if (s_IteratorB == m_CachedEntityTreeMap.end()) {
                        return true;
                    }

                    return s_IteratorA->second->m_Name < s_IteratorB->second->m_Name;
                });

                for (const auto& s_GateEntityRef : s_SortedGateEntities) {
                    const ZGateEntity* s_GateEntity = s_GateEntityRef.QueryInterface<ZGateEntity>();
                    bool s_IsGateVisible = false;

                    if (m_ShowOnlyVisibleGates) {
                        for (const auto& s_GateRenderDataIndex : SDK()->Globals()->RenderManager->m_pRoomManagerRender->m_GatesVisible) {
                            uint16_t s_GateEntityIndex =
                                SDK()->Globals()->RenderManager->m_pRoomManagerRender->m_GateRenderData[s_GateRenderDataIndex].m_nGateID;

                            if (SDK()->Globals()->RoomManagerMain->m_GateEntities[s_GateEntityIndex] == s_GateEntity) {
                                s_IsGateVisible = true;
                                break;
                            }
                        }

                        if (!s_IsGateVisible) {
                            continue;
                        }
                    }

                    auto s_Iterator = m_CachedEntityTreeMap.find(s_GateEntityRef);

                    if (s_Iterator == m_CachedEntityTreeMap.end()) {
                        continue;
                    }

                    std::shared_ptr<EntityTreeNode> s_GateEntityTreeNode = s_Iterator->second;

                    if (!knt::util::FindSubstring(s_GateEntityTreeNode->m_Name, s_GateName)) {
                        continue;
                    }

                    ImGuiTreeNodeFlags s_GateNodeFlags =
                        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;

                    ImGui::SetNextItemAllowOverlap();

                    ImGui::TreeNodeEx(s_GateEntityTreeNode->m_Name.c_str(), s_GateNodeFlags);

                    ImGui::SameLine();

                    ImGui::PushID(s_GateEntityTreeNode->m_EntityID);

                    if (ImGui::SmallButton("Select in entity tree")) {
                        OnSelectEntity(s_GateEntityRef, true, std::nullopt);
                    }

                    ImGui::PopID();
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}
