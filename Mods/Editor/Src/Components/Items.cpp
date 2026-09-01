#include "Editor.hpp"

#include <IconsMaterialDesign.h>

#include <Glacier/ZStore.hpp>
#include <Glacier/ZPlayer.hpp>

#include <Logging.hpp>
#include <Util/StringUtils.hpp>

class ZDynamicGameplaySpawnerEntryEntity;

void Editor::DrawItemsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) {
    if (!p_HasFocus || !m_ShowItemsWindow) {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    const auto s_IsWindowExpanded = ImGui::Begin(ICON_MD_BUILD " Items", &m_ShowItemsWindow);
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

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Item name");
    ImGui::SameLine();

    const float s_ButtonWidth = ImGui::CalcTextSize("Teleport all items to player").x + ImGui::GetStyle().FramePadding.x * 2.0f;

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - s_ButtonWidth - ImGui::GetStyle().ItemSpacing.x);

    ImGui::InputText("##ItemName", m_ItemEntityName, sizeof(m_ItemEntityName));

    ImGui::SameLine();

    auto s_LocalPlayer = SDK()->Globals()->LocalPlayerData->m_pCharacterImpl->m_pCharacter;

    auto& s_ItemRegistry = SDK()->Globals()->GPWTransientStores->m_pTransientStoreRegistries->m_ItemRegistry;

    ImGui::BeginDisabled(!s_LocalPlayer || s_ItemRegistry.m_Entries.size() == 0);

    if (ImGui::Button("Teleport all items to player")) {
        for (size_t i = 0; i < s_ItemRegistry.m_Entries.size(); ++i) {
            const auto& s_Entry = s_ItemRegistry.m_Entries[i];

            if (!s_Entry.m_pInterfaceRef) {
                continue;
            }

            s_Entry.m_pInterfaceRef->m_spatialEntity.m_pInterfaceRef->SetObjectToWorldMatrixFromEditor(
                s_LocalPlayer.m_pInterfaceRef->GetObjectToWorldMatrix()
            );
        }
    }

    ImGui::EndDisabled();

    if (ImGui::CollapsingHeader("Filters")) {
        ImGui::Checkbox("Item type filtering", &m_FilterByItemType);

        ImGui::Spacing();

        ImGui::BeginDisabled(!m_FilterByItemType);

        ImGui::Checkbox("Handgun", &m_ShowHandguns);
        ImGui::Checkbox("Taser", &m_ShowTasers);
        ImGui::Checkbox("Dart gun", &m_ShowDartGuns);
        ImGui::Checkbox("Submachine gun", &m_ShowSubmachineGuns);
        ImGui::Checkbox("Assault rifle", &m_ShowAssaultRifles);
        ImGui::Checkbox("Shotgun", &m_ShowShotguns);
        ImGui::Checkbox("Long rifle", &m_ShowLongRifles);

        ImGui::Separator();

        ImGui::Checkbox("Smoke pellets", &m_ShowSmokePellets);
        ImGui::Checkbox("Blast device", &m_ShowBlastDevices);

        ImGui::Separator();

        ImGui::Checkbox("Brick", &m_ShowBricks);
        ImGui::Checkbox("Vase", &m_ShowVases);
        ImGui::Checkbox("Bust", &m_ShowBusts);

        ImGui::Separator();

        ImGui::Checkbox("Cosmetic", &m_ShowCosmetics);

        ImGui::Separator();

        ImGui::Checkbox("Undefined", &m_ShowCosmetics);

        ImGui::EndDisabled();
    }

    ImGui::Checkbox("Select item in list when clicked in game", &m_SelectItemOnMouseClick);

    if (!SpawnEntities()) {
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        return;
    }

    ImGui::Separator();

    ImGui::BeginChild("ItemList", ImVec2(500, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (size_t i = 0; i < s_ItemRegistry.m_Entries.size(); ++i) {
        const auto& s_Entry = s_ItemRegistry.m_Entries[i];

        if (!s_Entry.m_pInterfaceRef) {
            continue;
        }

        const bool s_IsSelected = m_SelectedItem.m_pInterfaceRef == s_Entry.m_pInterfaceRef;

        if (m_ScrollToItem && s_IsSelected) {
            ImGui::SetScrollHereY(0.25f);

            m_ScrollToItem = false;
        }

        ZEntityRef s_ItemEntity;
        s_Entry.m_pInterfaceRef->GetID(s_ItemEntity);

        std::shared_lock s_TreeLock(m_CachedEntityTreeMutex);
        const auto s_EntityIt = m_CachedEntityTreeMap.find(s_ItemEntity);

        std::string s_ItemName;
        ZEntityRef m_ItemRootEntity;

        if (s_EntityIt != m_CachedEntityTreeMap.end()) {
            auto s_RootEntity = GetRootEntity(s_EntityIt->second);
            auto s_ParentEntity = s_RootEntity->m_Parent.lock();

            auto s_LastItemEntity = s_RootEntity;
            auto s_Node = s_ParentEntity;
            bool s_IsItemInterfaceFound = false;

            while (s_Node) {
                if (s_Node->m_Entity && s_Node->m_Entity.QueryInterface<ZItemCharacterEntityBase>()) {
                    s_LastItemEntity = s_Node;
                    s_IsItemInterfaceFound = true;
                }
                else {
                    break;
                }

                s_Node = s_Node->m_Parent.lock();
            }

            if (s_IsItemInterfaceFound) {
                m_ItemRootEntity = s_LastItemEntity->m_Entity;
                s_ItemName = s_LastItemEntity->m_Name;
            }
            else if (
                s_ParentEntity->m_Entity && s_ParentEntity->m_Entity.QueryInterface<ZSpatialEntity>()
                && !s_ParentEntity->m_Entity.QueryInterface<ZDynamicGameplaySpawnerEntryEntity>()
            ) {
                m_ItemRootEntity = s_ParentEntity->m_Entity;
                s_ItemName = s_ParentEntity->m_Name;
            }
            else {
                m_ItemRootEntity = s_RootEntity->m_Entity;
                s_ItemName = s_RootEntity->m_Name;
            }
        }
        else {
            s_ItemName = fmt::format("{:016x}", s_ItemEntity->GetType()->m_nEntityID);
        }

        if (!MatchesItemFilters(s_Entry.m_pInterfaceRef, s_ItemName)) {
            continue;
        }

        std::string s_ItemID = std::format("{}###{}", s_ItemName, i);

        if (ImGui::Selectable(s_ItemID.c_str(), s_IsSelected)) {
            if (!s_IsSelected) {
                m_SelectedItem = s_Entry;
                m_SelectedItemRootEntity = m_ItemRootEntity;

                Logger::Info("[Editor] Selected item (by list): {}", s_ItemName.c_str());
            }
        }
    }

    ImGui::EndChild();

    if (!m_SelectedItem) {
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        return;
    }

    ZEntityRef s_ItemEntity;
    m_SelectedItem.m_pInterfaceRef->GetID(s_ItemEntity);

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::BeginChild("ItemDetails", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

    DrawItemInfo(m_SelectedItem.m_pInterfaceRef, m_SelectedItemRootEntity, true);

    ImGui::Separator();

    if (ImGui::Button("Select in entity tree")) {
        if (!m_CachedEntityTree || !m_CachedEntityTree->m_Entity) {
            UpdateEntities();
        }

        OnSelectEntity(m_SelectedItemRootEntity, true, std::nullopt);
    }

    bool s_HasOwner = false;
    ZEntityRef s_Owner;

    if (m_SelectedItem.m_pInterfaceRef->m_spawnedItem) {
        s_Owner = m_SelectedItem.m_pInterfaceRef->m_spawnedItem.GetLogicalParent().GetLogicalParent().GetLogicalParent();

        s_HasOwner = s_Owner.QueryInterface<ZHumanoidCharacterEntity>() != nullptr;
    }

    ImGui::BeginDisabled(!s_HasOwner);

    if (ImGui::Button("Select owner in entity tree")) {
        if (!m_CachedEntityTree || !m_CachedEntityTree->m_Entity) {
            UpdateEntities();
        }

        OnSelectEntity(s_Owner, true, std::nullopt);
    }

    ImGui::EndDisabled();

    if (s_LocalPlayer) {
        ImGui::Spacing();

        if (ImGui::Button("Teleport item to player")) {
            auto s_SpatialEntity = m_SelectedItemRootEntity.QueryInterface<ZSpatialEntity>();

            if (s_SpatialEntity) {
                s_SpatialEntity->SetObjectToWorldMatrixFromEditor(s_LocalPlayer.m_pInterfaceRef->GetObjectToWorldMatrix());
            }
        }

        if (ImGui::Button("Teleport player to item")) {
            TeleportHumanoid(s_LocalPlayer.m_entityRef, m_SelectedItemRootEntity.QueryInterface<ZSpatialEntity>());
        }
    }

    ImGui::EndChild();
    ImGui::EndGroup();

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Editor::DrawItemInfo(ZItemCharacterEntityBase* p_Item, ZEntityRef p_ItemRootEntity, bool p_ShowOwnerEntityName) {
    auto s_Definition = GetItemDefinition(p_Item);

    ZString s_Name;

    if (s_Definition->m_itemDisplayNameSweet.Exists()) {
        ZTextLine* s_TextLine = s_Definition->m_itemDisplayNameSweet.GetResource();
        s_Name = s_TextLine->GetText();
    }
    else {
        s_Name = s_Definition->m_itemDisplayNameRaw;
    }

    std::string s_EntityName;

    if (p_ItemRootEntity) {
        std::shared_lock s_TreeLock(m_CachedEntityTreeMutex);
        auto s_EntityIt = m_CachedEntityTreeMap.find(p_ItemRootEntity);

        if (s_EntityIt != m_CachedEntityTreeMap.end()) {
            s_EntityName = s_EntityIt->second->m_Name;
        }
    }

    std::string s_OwnerEntityName;

    if (p_ShowOwnerEntityName && p_Item->m_spawnedItem) {
        ZEntityRef s_Owner = p_Item->m_spawnedItem.GetLogicalParent().GetLogicalParent().GetLogicalParent();

        if (s_Owner.QueryInterface<ITEntityRefValue<ZHumanoidCharacterEntity>>()) {
            auto s_EntityIt2 = m_CachedEntityTreeMap.find(s_Owner);

            if (s_EntityIt2 != m_CachedEntityTreeMap.end()) {
                s_OwnerEntityName = s_EntityIt2->second->m_Name;
            }
        }
    }

    std::string s_TemplateResourceID;
    std::string s_TemplateRuntimeResourceID;

    if (p_Item->m_itemTemplate.IsValid()) {
        TResourcePtr<ZTemplateEntityFactory> s_ResourcePtr;
        SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, p_Item->m_itemTemplate);

        ZTemplateEntityFactory* s_TemplateEntityFactory = s_ResourcePtr.GetResource();

        if (s_TemplateEntityFactory) {
            s_TemplateResourceID = s_TemplateEntityFactory->m_SourceResourceID.c_str();
            s_TemplateRuntimeResourceID = fmt::format("{:016X}", p_Item->m_itemTemplate.GetID());
        }
    }
    else if (p_ItemRootEntity) {
        std::shared_lock s_FactoryLock(m_EntityRefToFactoryRuntimeResourceIDsMutex);
        auto s_Iterator2 = m_EntityRefToFactoryRuntimeResourceIDs.find(p_ItemRootEntity);

        if (s_Iterator2 != m_EntityRefToFactoryRuntimeResourceIDs.end()) {
            const auto [s_TemplateFactoryRuntimeResourceID, s_ParentTemplateFactoryRuntimeResourceID] = s_Iterator2->second;

            TResourcePtr<ZTemplateEntityFactory> s_ResourcePtr;
            SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_TemplateFactoryRuntimeResourceID);

            ZTemplateEntityFactory* s_TemplateEntityFactory = s_ResourcePtr.GetResource();

            if (s_TemplateEntityFactory) {
                s_TemplateResourceID = s_TemplateEntityFactory->m_SourceResourceID.c_str();
                s_TemplateRuntimeResourceID = fmt::format("{:016X}", s_TemplateFactoryRuntimeResourceID.GetID());
            }
        }
    }

    ImGui::Text("Name: %s", !s_Name.IsEmpty() ? s_Name.c_str() : "<None>");
    ImGui::TextWrapped("Entity name: %s", s_EntityName.c_str());
    ImGui::Text("Type: %s", ItemTypeToString(s_Definition->m_itemType));

    if (p_ShowOwnerEntityName) {
        ImGui::Text("Owner entity name: %s", !s_OwnerEntityName.empty() ? s_OwnerEntityName.c_str() : "<None>");
    }

    ImGui::TextWrapped("Template resource ID: %s", s_TemplateResourceID.c_str());
    ImGui::Text("Template runtime resource ID: %s", s_TemplateRuntimeResourceID.c_str());

    if (ImGuiCopyWidget(s_TemplateRuntimeResourceID)) {
        CopyToClipboard(s_TemplateRuntimeResourceID);
    }
}

ZItemCharacterDefinitionBase* Editor::GetItemDefinition(ZItemCharacterEntityBase* p_Item) {
    if (!p_Item->m_definition.Exists()) {
        return nullptr;
    }

    TResourcePtr<ZEntityRef> s_ResourcePtr;
    SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, p_Item->m_definition.GetResourceInfo().rid);

    SEntityResource* s_EntityResource = static_cast<SEntityResource*>(s_ResourcePtr.GetResourceData());

    return s_EntityResource->entityRef.QueryInterface<ZItemCharacterDefinitionBase>();
}

const char* Editor::ItemTypeToString(EItemType p_ItemType) {
    switch (p_ItemType) {
    case EItemType::NoItem:
        return "No item";
    case EItemType::HandGun:
        return "Handgun";
    case EItemType::Taser:
        return "Taser";
    case EItemType::DartGun:
        return "Dart gun";
    case EItemType::SubMachineGun:
        return "Submachine gun";
    case EItemType::AssaultRifle:
        return "Assault rifle";
    case EItemType::Shotgun:
        return "Shotgun";
    case EItemType::SmokePellets:
        return "Smoke pellets";
    case EItemType::BlastDevice:
        return "Blast device";
    case EItemType::Brick:
        return "Brick";
    case EItemType::Vase:
        return "Vase";
    case EItemType::Bust:
        return "Bust";
    case EItemType::Undefined:
        return "Undefined";
    case EItemType::LongRifle:
        return "Long rifle";
    case EItemType::Cosmetic:
        return "Cosmetic";
    default:
        return "";
    }
}

bool Editor::MatchesItemFilters(ZItemCharacterEntityBase* p_Item, const std::string& p_ItemName) {
    const auto s_Definition = GetItemDefinition(p_Item);

    if (!s_Definition) {
        return false;
    }

    bool s_MatchesItemType = true;

    if (m_FilterByItemType) {
        s_MatchesItemType = false;

        switch (s_Definition->m_itemType) {
        case EItemType::NoItem:
            s_MatchesItemType |= m_ShowNoItems;
            break;

        case EItemType::HandGun:
            s_MatchesItemType |= m_ShowHandguns;
            break;

        case EItemType::Taser:
            s_MatchesItemType |= m_ShowTasers;
            break;

        case EItemType::DartGun:
            s_MatchesItemType |= m_ShowDartGuns;
            break;

        case EItemType::SubMachineGun:
            s_MatchesItemType |= m_ShowSubmachineGuns;
            break;

        case EItemType::AssaultRifle:
            s_MatchesItemType |= m_ShowAssaultRifles;
            break;

        case EItemType::Shotgun:
            s_MatchesItemType |= m_ShowShotguns;
            break;

        case EItemType::SmokePellets:
            s_MatchesItemType |= m_ShowSmokePellets;
            break;

        case EItemType::BlastDevice:
            s_MatchesItemType |= m_ShowBlastDevices;
            break;

        case EItemType::Brick:
            s_MatchesItemType |= m_ShowBricks;
            break;

        case EItemType::Vase:
            s_MatchesItemType |= m_ShowVases;
            break;

        case EItemType::Bust:
            s_MatchesItemType |= m_ShowBusts;
            break;

        case EItemType::Undefined:
            s_MatchesItemType |= m_ShowUndefinedItems;
            break;

        case EItemType::LongRifle:
            s_MatchesItemType |= m_ShowLongRifles;
            break;

        case EItemType::Cosmetic:
            s_MatchesItemType |= m_ShowCosmetics;
            break;
        }
    }

    const bool s_MatchesName = m_ItemEntityName[0] == '\0' || knt::util::FindSubstring(p_ItemName, m_ItemEntityName);

    return s_MatchesName && s_MatchesItemType;
}
