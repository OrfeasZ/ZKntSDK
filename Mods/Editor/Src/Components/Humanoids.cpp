#include "Editor.hpp"

#include <IconsMaterialDesign.h>

#include <Glacier/ZStore.hpp>
#include <Glacier/ZPlayer.hpp>
#include <Glacier/ZGameplayTagListEntity.hpp>
#include <Glacier/ZEditorEnumType.hpp>
#include <Glacier/EGameplayTag.hpp>

#include <Logging.hpp>
#include <Util/ImGuiUtils.hpp>
#include <Util/StringUtils.hpp>

void Editor::DrawHumanoidsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) {
    if (!p_HasFocus || !m_ShowHumanoidsWindow) {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    const auto s_IsWindowExpanded = ImGui::Begin(ICON_MD_PEOPLE " Humanoids", &m_ShowHumanoidsWindow);
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

    static char s_Outfit[1024]{""};
    static char s_OutfitVariation[1024]{""};
    static const OutfitInfo* s_OutfitInfo = nullptr;

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Humanoid name");
    ImGui::SameLine();

    const float s_ButtonWidth = ImGui::CalcTextSize("Teleport all humanoids to player").x + ImGui::GetStyle().FramePadding.x * 2.0f;

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - s_ButtonWidth - ImGui::GetStyle().ItemSpacing.x);

    ImGui::InputText("##HumanoidName", m_HumanoidEntityName, sizeof(m_HumanoidEntityName));

    ImGui::SameLine();

    auto s_LocalPlayer = SDK()->Globals()->LocalPlayerData->m_pCharacterImpl->m_pCharacter;

    auto& s_HumanoidRegistry = SDK()->Globals()->GPWTransientStores->m_pTransientStoreRegistries->m_HumanoidRegistry;

    ImGui::BeginDisabled(!s_LocalPlayer || s_HumanoidRegistry.m_Entries.size() == 0);

    if (ImGui::Button("Teleport all humanoids to player")) {
        for (size_t i = 0; i < s_HumanoidRegistry.m_Entries.size(); ++i) {
            const auto& s_Entry = s_HumanoidRegistry.m_Entries[i];

            if (!s_Entry.m_pInterfaceRef) {
                continue;
            }

            ZEntityRef s_HumanoidEntity;
            s_Entry.m_pInterfaceRef->GetID(s_HumanoidEntity);

            TeleportHumanoid(s_HumanoidEntity, s_LocalPlayer.m_pInterfaceRef);
        }
    }

    ImGui::EndDisabled();

    if (ImGui::CollapsingHeader("Filters")) {
        ImGui::Text("State");

        constexpr const char* s_AliveFilterNames[] = {"Any", "Alive", "Dead"};

        if (ImGui::BeginCombo("Alive", s_AliveFilterNames[static_cast<int>(m_AliveFilter)])) {
            for (int i = 0; i < IM_ARRAYSIZE(s_AliveFilterNames); ++i) {
                const bool s_IsSelected = static_cast<int>(m_AliveFilter) == i;

                if (ImGui::Selectable(s_AliveFilterNames[i], s_IsSelected)) {
                    m_AliveFilter = static_cast<EAliveFilter>(i);
                }

                if (s_IsSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Separator();

        ImGui::Checkbox("Gameplay tag filtering", &m_IsGamePlayTagFilteringEnabled);

        ImGui::Spacing();

        ImGui::BeginDisabled(!m_IsGamePlayTagFilteringEnabled);

        ImGui::Text("Type");
        ImGui::Checkbox("Enemy", &m_ShowEnemies);
        ImGui::Checkbox("Companion", &m_ShowCompanions);
        ImGui::Checkbox("Authority", &m_ShowAuthorities);
        ImGui::Checkbox("Civilian", &m_ShowCivilians);
        ImGui::Checkbox("Ally", &m_ShowAllies);

        ImGui::Separator();

        ImGui::Text("Archetype");
        ImGui::Checkbox("Grunt", &m_ShowGrunts);
        ImGui::Checkbox("Merc", &m_ShowMercs);
        ImGui::Checkbox("Armored merc", &m_ShowArmoredMercs);
        ImGui::Checkbox("Specialist", &m_ShowSpecialists);
        ImGui::Checkbox("Leader", &m_ShowLeaders);
        ImGui::Checkbox("Sniper", &m_ShowSnipers);
        ImGui::Checkbox("Tank", &m_ShowTanks);
        ImGui::Checkbox("Brute", &m_ShowBrutes);
        ImGui::Checkbox("Android", &m_ShowAndroids);

        ImGui::EndDisabled();

        ImGui::Separator();
    }

    m_FilterByType = m_ShowEnemies || m_ShowCompanions || m_ShowAuthorities || m_ShowCivilians || m_ShowAllies;

    m_FilterByArchetype = m_ShowGrunts || m_ShowMercs || m_ShowArmoredMercs || m_ShowSpecialists || m_ShowLeaders || m_ShowSnipers || m_ShowTanks
                          || m_ShowBrutes || m_ShowAndroids;

    ImGui::Checkbox("Select humanoid in list when clicked in game", &m_SelectHumanoidOnMouseClick);

    if (!SpawnEntities()) {
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        return;
    }

    ImGui::Separator();

    ImGui::BeginChild("HumanoidList", ImVec2(500, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (size_t i = 0; i < s_HumanoidRegistry.m_Entries.size(); ++i) {
        const auto& s_Entry = s_HumanoidRegistry.m_Entries[i];

        if (!s_Entry.m_pInterfaceRef) {
            continue;
        }

        const bool s_IsSelected = m_SelectedHumanoid.m_pInterfaceRef == s_Entry.m_pInterfaceRef;

        if (m_ScrollToHumanoid && s_IsSelected) {
            ImGui::SetScrollHereY(0.25f);

            m_ScrollToHumanoid = false;
        }

        ZEntityRef s_HumanoidEntity;
        s_Entry.m_pInterfaceRef->GetID(s_HumanoidEntity);

        ZEntityRef s_MainEntity;

        if (s_LocalPlayer && s_LocalPlayer.m_entityRef == s_HumanoidEntity) {
            // DynamicGameplaySpawner -> Player
            s_MainEntity = s_HumanoidEntity.GetLogicalParent().GetLogicalParent();
        }
        else {
            // Humanoid Spawner -> NPCSpawner -> Root entity
            s_MainEntity = s_HumanoidEntity.GetLogicalParent().GetLogicalParent().GetLogicalParent();
        }

        std::shared_lock s_TreeLock(m_CachedEntityTreeMutex);
        const auto s_EntityIt = m_CachedEntityTreeMap.find(s_MainEntity);

        std::string s_HumanoidName;

        if (s_EntityIt != m_CachedEntityTreeMap.end()) {
            s_HumanoidName = s_EntityIt->second->m_Name;
        }
        else {
            s_HumanoidName = fmt::format("{:016x}", s_MainEntity->GetType()->m_nEntityID);
        }

        if (!MatchesHumanoidFilters(s_HumanoidEntity, s_HumanoidName)) {
            continue;
        }

        std::string s_HumanoidID = std::format("{}###{}", s_HumanoidName, i);

        if (ImGui::Selectable(s_HumanoidID.c_str(), s_IsSelected)) {
            if (!s_IsSelected) {
                m_SelectedHumanoid = s_Entry;
                s_Outfit[0] = '\0';

                Logger::Info("[Editor] Selected humanoid (by list): {}", s_HumanoidName.c_str());
            }
        }
    }

    ImGui::EndChild();

    if (!m_SelectedHumanoid) {
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        return;
    }

    ZEntityRef s_HumanoidEntity;
    m_SelectedHumanoid.m_pInterfaceRef->GetID(s_HumanoidEntity);

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::BeginChild("HumanoidDetails", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

    const auto s_GameplayTagListEntity = s_HumanoidEntity.QueryInterface<ZGameplayTagListEntity>();

    if (s_GameplayTagListEntity && s_GameplayTagListEntity->Tags.size() > 0) {
        TResourcePtr<ZEditorEnumType> s_EnumResourcePtr;
        SDK()->Globals()->ResourceManager->LoadResource(s_EnumResourcePtr, ResId<"[assembly:/_knt/common/gameplaytags.enumtype].enumtype">);

        ImGui::Text("Tags");

        ImGui::Spacing();

        for (const auto& s_Tag : s_GameplayTagListEntity->Tags) {
            ImGui::Text(s_EnumResourcePtr.GetResource()->m_pEnumType->aItemNames[s_Tag].c_str());
        }

        ImGui::Separator();
    }

    if (ImGui::Button("Select in entity tree")) {
        if (!m_CachedEntityTree || !m_CachedEntityTree->m_Entity) {
            UpdateEntities();
        }

        OnSelectEntity(s_HumanoidEntity, true, std::nullopt);
    }

    if (s_LocalPlayer && s_LocalPlayer.m_entityRef != s_HumanoidEntity) {
        ImGui::Spacing();

        if (ImGui::Button("Teleport humanoid to player")) {
            TeleportHumanoid(s_HumanoidEntity, s_LocalPlayer.m_pInterfaceRef);
        }

        if (ImGui::Button("Teleport player to humanoid")) {
            TeleportHumanoid(s_LocalPlayer.m_entityRef, m_SelectedHumanoid.m_pInterfaceRef);
        }

        ImGui::Spacing();
    }

    if (ImGui::Button("Disarm equipped firearm")) {
        DisarmHumanoid(s_HumanoidEntity, EFirearmDisarmType::EquippedOnly);
    }

    if (ImGui::Button("Disarm all attached firearms")) {
        DisarmHumanoid(s_HumanoidEntity, EFirearmDisarmType::AllAttached);
    }

    if (ImGui::Button("Kill humanoid")) {
        SetHumanoidHealth(s_HumanoidEntity, 0.f);
    }

    ImGui::Separator();

    if (m_OutfitSetToOutfitInfo.empty()) {
        if (ImGui::Button("Get outfit")) {
            LoadAllOutfitSets();
        }
    }

    ImGui::BeginDisabled(m_OutfitSetToOutfitInfo.empty());

    bool s_IsOutfitSetPopupOpen = knt::util::InputWithAutocomplete(
        "Outfit set##OutfitSet", s_Outfit, sizeof(s_Outfit), m_OutfitSetToOutfitInfo, [](auto& p_Pair) -> const std::string& { return p_Pair.first; },
        [](auto& p_Pair) -> const std::string& { return p_Pair.first; },
        [&](const std::string&, const std::string& p_Name, const auto& p_Pair) {
            if (const auto it = m_OutfitSetToOutfitInfo.find(p_Name); it != m_OutfitSetToOutfitInfo.end()) {
                s_OutfitInfo = &it->second;
                s_OutfitVariation[0] = '\0';
            }
        }
    );

    ImGui::EndDisabled();

    if (!m_OutfitSetToOutfitInfo.empty() && !s_IsOutfitSetPopupOpen && s_Outfit[0] == '\0') {
        const ZRuntimeResourceID s_OutfitSet = m_SelectedHumanoid.m_pInterfaceRef->m_outfitSet.GetResourceInfo().rid;
        const char* s_Outfit2 = m_OutfitSetRuntimeResourceIDToName[s_OutfitSet].c_str();

        strncpy(s_Outfit, s_Outfit2, sizeof(s_Outfit) - 1);

        s_Outfit[sizeof(s_Outfit) - 1] = '\0';

        if (const auto it = m_OutfitSetToOutfitInfo.find(s_Outfit); it != m_OutfitSetToOutfitInfo.end()) {
            s_OutfitInfo = &it->second;
        }

        int32_t s_SelectedOutfit = m_SelectedHumanoid.m_pInterfaceRef->m_selectedOutfit;

        if (s_SelectedOutfit >= s_OutfitInfo->m_Variations.size()) {
            s_SelectedOutfit = 0;
        }

        const char* s_OutfitVariation2 = s_OutfitInfo->m_Variations[s_SelectedOutfit].first.c_str();

        strncpy(s_OutfitVariation, s_OutfitVariation2, sizeof(s_OutfitVariation) - 1);
    }

    ImGui::BeginDisabled(m_OutfitSetToOutfitInfo.empty() || !s_OutfitInfo);

    bool s_IsOutfitVariationPopupOpen = knt::util::InputWithAutocomplete(
        "Outfit variation##OutfitVariation", s_OutfitVariation, sizeof(s_OutfitVariation),
        s_OutfitInfo ? s_OutfitInfo->m_Variations : std::vector<std::pair<std::string, size_t>>{},
        [](auto& p_Pair) -> std::string { return p_Pair.first; }, [](auto& p_Pair) -> std::string { return p_Pair.first; },
        [&](const std::string&, const std::string& p_Name, const std::pair<std::string, size_t>& p_Value) {
            SetHumanoidOutfit(m_SelectedHumanoid.m_pInterfaceRef, s_OutfitInfo->m_OutfitSet, p_Value.second);

            s_Outfit[0] = '\0';
        }
    );

    if (!m_OutfitSetToOutfitInfo.empty() && !s_IsOutfitVariationPopupOpen && s_OutfitVariation[0] == '\0') {
        if (s_OutfitInfo) {
            int32_t s_SelectedOutfit = m_SelectedHumanoid.m_pInterfaceRef->m_selectedOutfit;

            if (s_SelectedOutfit >= s_OutfitInfo->m_Variations.size()) {
                s_SelectedOutfit = 0;
            }

            const char* s_OutfitVariation2 = s_OutfitInfo->m_Variations[s_SelectedOutfit].first.c_str();

            strncpy(s_OutfitVariation, s_OutfitVariation2, sizeof(s_OutfitVariation) - 1);
        }
    }

    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Spacing();

    if (s_LocalPlayer && s_LocalPlayer.m_entityRef != s_HumanoidEntity) {
        if (ImGui::Button("Set outfit to player")) {
            if (SDK()->Globals()->LocalPlayerData->m_pCharacterImpl) {
                SetHumanoidOutfit(
                    s_LocalPlayer.m_pInterfaceRef, m_SelectedHumanoid.m_pInterfaceRef->m_outfitSet.GetResourceInfo().rid,
                    m_SelectedHumanoid.m_pInterfaceRef->m_selectedOutfit
                );

                s_Outfit[0] = '\0';
            }
        }

        if (ImGui::Button("Set player's outfit")) {
            SetHumanoidOutfit(
                m_SelectedHumanoid.m_pInterfaceRef, s_LocalPlayer.m_pInterfaceRef->m_outfitSet.GetResourceInfo().rid,
                s_LocalPlayer.m_pInterfaceRef->m_selectedOutfit
            );

            s_Outfit[0] = '\0';
        }
    }

    static float s_Health;
    static float s_MinHealth = 0.f;
    static float s_MaxHealth;

    m_TargetEntityRefValue.m_entityRef.SetProperty("m_vValue", s_HumanoidEntity);

    const auto s_HumanoidRef = TInterfaceRef<IEntityRefValue>::FromEntityRef(m_TargetEntityRefValue.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("[Editor] Failed to get ITEntityRefValue for humanoid.");

        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        return;
    }

    m_HumanoidHealthGetter.m_entityRef.SetProperty("m_rTarget", s_HumanoidRef);
    m_HumanoidMaxHealthGetter.m_entityRef.SetProperty("m_rTarget", s_HumanoidRef);

    s_Health = m_HumanoidHealthGetter.m_pInterfaceRef->GetValue();
    s_MaxHealth = m_HumanoidMaxHealthGetter.m_pInterfaceRef->GetValue();

    ImGui::Separator();

    if (ImGui::DragScalar("Health", ImGuiDataType_Float, &s_Health, 0.1f, &s_MinHealth, &s_MaxHealth)) {
        SetHumanoidHealth(s_HumanoidEntity, s_Health);
    }

    ImGui::Text("Max health: %.1f", s_MaxHealth);

    bool& s_Invincible = m_HumanoidEntityIDToInvincibility[s_HumanoidEntity->GetType()->m_nEntityID];

    if (ImGui::Checkbox("Invincible", &s_Invincible)) {
        SetHumanoidImmuneToDamage(s_HumanoidEntity, s_Invincible);
    }

    ImGui::Separator();

    auto s_EquippedItem = GetEquippedItem(m_SelectedHumanoid.m_pInterfaceRef);

    if (s_EquippedItem) {
        ImGui::Text("Equipped item");

        ImGui::Spacing();

        ImGui::PushID("EquippedItem");

        ZEntityRef s_EntityRef;
        s_EquippedItem->GetID(s_EntityRef);

        DrawItemInfo(s_EquippedItem, s_EntityRef.GetLogicalParent().GetLogicalParent(), false);

        ImGui::PopID();

        ImGui::Separator();
    }

    const auto s_StoreData = SDK()->Globals()->KntGPWStores->m_pStoreCollection->m_pStoreData;

    const uint32_t s_HumanoidHandle = m_SelectedHumanoid.m_pInterfaceRef->m_StoreHandle;
    const uint16_t s_HumanoidIndex = static_cast<uint16_t>(s_HumanoidHandle);

    auto& s_InventoryIndexStore = s_StoreData->m_HumanoidToInventoryIndex;

    if (!s_InventoryIndexStore.IsValidHandle(s_HumanoidHandle)) {
        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        return;
    }

    const uint16_t s_InventoryIndex = s_InventoryIndexStore.m_Blocks[s_HumanoidIndex / 100][s_HumanoidIndex % 100];

    if (s_InventoryIndex >= s_StoreData->m_Inventories.size()) {
        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        return;
    }

    ImGui::Text("Inventory");

    ImGui::Spacing();

    const auto& s_Inventory = s_StoreData->m_Inventories[s_InventoryIndex];
    auto& s_ItemRegistryEntries = SDK()->Globals()->GPWTransientStores->m_pRegistryData->m_ItemRegistryEntries;

    for (const auto& s_Entry : s_Inventory) {
        const uint32_t s_ItemHandle = s_Entry.m_ItemHandle;

        if (!s_ItemRegistryEntries.IsValidHandle(s_ItemHandle)) {
            continue;
        }

        const uint16_t s_ItemIndex = static_cast<uint16_t>(s_ItemHandle);

        const TInterfaceRef<ZItemCharacterEntityBase>& s_Item = s_ItemRegistryEntries.m_Blocks[s_ItemIndex / 100][s_ItemIndex % 100];

        if (!s_Item) {
            continue;
        }

        ImGui::PushID(s_ItemHandle);

        ZEntityRef s_EntityRef;
        s_Item.m_pInterfaceRef->GetID(s_EntityRef);

        DrawItemInfo(s_EquippedItem, s_EntityRef.GetLogicalParent().GetLogicalParent(), false);

        ImGui::PopID();
    }

    ImGui::EndChild();
    ImGui::EndGroup();

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Editor::LoadAllOutfitSets() {
    for (const auto& s_ResourceInfo : (*SDK()->Globals()->ResourceContainer)->m_resources) {
        if (s_ResourceInfo.resourceType != 'ERES') {
            continue;
        }

        for (size_t i = 0; i < s_ResourceInfo.numReferences; ++i) {
            const uint32_t s_ReferenceIndex = (*SDK()->Globals()->ResourceContainer)->m_references[s_ResourceInfo.firstReferenceIndex + i].index;
            const ZResourceContainer::SResourceInfo& s_ReferenceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[s_ReferenceIndex];

            if (s_ReferenceInfo.resourceType == 'CPPT' && s_ReferenceInfo.rid == ResId<"[modules:/zhumanoidoutfitset.class].entitytype">) {
                TResourcePtr<ZEntityRef> s_ResourcePtr;
                SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_ResourceInfo.rid);

                SEntityResource* s_EntityResource = static_cast<SEntityResource*>(s_ResourcePtr.GetResourceData());
                ZHumanoidOutfitSet* s_HumanoidOutfitSet = s_EntityResource->entityRef.QueryInterface<ZHumanoidOutfitSet>();

                ZTemplateEntityBlueprintFactory* s_TemplateEntityBlueprintFactory = static_cast<ZTemplateEntityBlueprintFactory*>(
                    static_cast<IEntityFactory*>(s_EntityResource->factoryResource.GetResourceData())->GetBlueprint()
                );

                std::string s_RootEntityName = s_TemplateEntityBlueprintFactory->m_pTemplateEntityBlueprint
                                                   ->subEntities[s_TemplateEntityBlueprintFactory->m_pTemplateEntityBlueprint->rootEntityIndex]
                                                   .entityName.c_str();

                OutfitInfo& s_OutfitInfo = m_OutfitSetToOutfitInfo[s_RootEntityName];
                s_OutfitInfo.m_OutfitSet = s_ResourceInfo.rid;

                m_OutfitSetRuntimeResourceIDToName.insert({s_ResourceInfo.rid, s_RootEntityName});

                const auto s_SubEntityCount = s_TemplateEntityBlueprintFactory->GetSubEntitiesCount();

                for (const auto& s_HumanoidOutfitReferenceInterfaceRef : s_HumanoidOutfitSet->m_outfits) {
                    for (int i = 0; i < s_SubEntityCount; ++i) {
                        if (i == s_TemplateEntityBlueprintFactory->m_rootEntityIndex) {
                            continue;
                        }

                        const ZEntityRef s_SubEntity = s_TemplateEntityBlueprintFactory->GetSubEntity(s_EntityResource->entityRef.m_pObj, i);
                        ZHumanoidOutfitReference* s_HumanoidOutfitReference = s_SubEntity.QueryInterface<ZHumanoidOutfitReference>();

                        if (s_HumanoidOutfitReferenceInterfaceRef.m_pInterfaceRef == s_HumanoidOutfitReference) {
                            s_OutfitInfo.m_Variations.push_back(
                                {s_TemplateEntityBlueprintFactory->m_pTemplateEntityBlueprint->subEntities[i].entityName.c_str(),
                                 s_OutfitInfo.m_Variations.size()}
                            );

                            break;
                        }
                    }
                }
            }
        }
    }
}

void Editor::SetHumanoidOutfit(
    ZHumanoidCharacterEntity* p_HumanoidCharacterEntity, const ZRuntimeResourceID& p_OutfitSetRuntimeResourceID, size_t p_OutfitVariationIndex
) {
    if (!SpawnEntities()) {
        return;
    }

    ZEntityRef s_HumanoidEntity;
    p_HumanoidCharacterEntity->GetID(s_HumanoidEntity);

    m_TargetEntityRefValue.m_entityRef.SetProperty("m_vValue", s_HumanoidEntity);
    m_HumanoidRefFromEntityRefGetter.m_entityRef.SetProperty("m_entityRef", TEntityRef<IEntityRefValue>(m_TargetEntityRefValue.m_entityRef));

    const auto s_HumanoidRef = TInterfaceRef<ITEntityRefValue<ZHumanoidCharacterEntity>>::FromEntityRef(m_HumanoidRefFromEntityRefGetter.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("[Editor] Failed to get ITEntityRefValue for humanoid.");
        return;
    }

    TResourcePtr<ZEntityRef> s_ResourcePtr;
    SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, p_OutfitSetRuntimeResourceID);

    m_HumanoidOutfitSetter.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_HumanoidOutfitSetter.m_entityRef.SetProperty<TResourcePtr<ZEntityRef>>("m_outfitSet", s_ResourcePtr);
    m_HumanoidOutfitSetter.m_entityRef.SetProperty<int32_t>("m_selectedOutfit", p_OutfitVariationIndex);

    m_HumanoidOutfitSetter.m_entityRef.SignalInputPin("Do");
}

void Editor::SetHumanoidHealth(ZEntityRef p_Humanoid, float p_Health) {
    if (!SpawnEntities()) {
        return;
    }

    m_TargetEntityRefValue.m_entityRef.SetProperty("m_vValue", p_Humanoid);

    const auto s_HumanoidRef = TInterfaceRef<IEntityRefValue>::FromEntityRef(m_TargetEntityRefValue.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("[Editor] Failed to get ITEntityRefValue for humanoid.");
        return;
    }

    m_HealthAmmountFloatValue.m_entityRef.SetProperty<float32>("m_nValue", p_Health);
    m_InterruptPassiveRegenerationBoolValue.m_entityRef.SetProperty("m_bValue", true);

    const auto s_HealthAmmountFloatRef = TInterfaceRef<IFloatValue>::FromEntityRef(m_HealthAmmountFloatValue.m_entityRef);
    const auto s_InterruptPassiveRegenerationBoolRef = TInterfaceRef<IBoolValue>::FromEntityRef(m_InterruptPassiveRegenerationBoolValue.m_entityRef);

    m_HumanoidHealthSetter.m_entityRef.SetProperty("m_target", s_HumanoidRef);
    m_HumanoidHealthSetter.m_entityRef.SetProperty("m_healthAmount", s_HealthAmmountFloatRef);
    m_HumanoidHealthSetter.m_entityRef.SetProperty("m_interruptPassiveRegeneration", s_InterruptPassiveRegenerationBoolRef);

    m_HumanoidHealthSetter.m_entityRef.SignalInputPin("Do");
}

void Editor::SetHumanoidImmuneToDamage(ZEntityRef p_Humanoid, bool p_Invulnerable) {
    if (!SpawnEntities()) {
        return;
    }

    m_TargetEntityRefValue.m_entityRef.SetProperty("m_vValue", p_Humanoid);
    m_HumanoidRefFromEntityRefGetter.m_entityRef.SetProperty("m_entityRef", TEntityRef<IEntityRefValue>(m_TargetEntityRefValue.m_entityRef));

    const auto s_HumanoidRef = TInterfaceRef<ITEntityRefValue<ZHumanoidCharacterEntity>>::FromEntityRef(m_HumanoidRefFromEntityRefGetter.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("[Editor] Failed to get ITEntityRefValue for humanoid.");
        return;
    }

    m_InvulnerableBoolValue.m_entityRef.SetProperty("m_bValue", p_Invulnerable);

    const auto s_InvulnerableBoolRef = TInterfaceRef<IBoolValue>::FromEntityRef(m_InvulnerableBoolValue.m_entityRef);

    m_HumanoidImmuneToDamageSetter.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_HumanoidImmuneToDamageSetter.m_entityRef.SetProperty("m_invulnerable", s_InvulnerableBoolRef);

    m_HumanoidImmuneToDamageSetter.m_entityRef.SignalInputPin("Do");
}

void Editor::TeleportHumanoid(ZEntityRef p_Humanoid, ZSpatialEntity* p_Target) {
    if (!p_Target || !SpawnEntities()) {
        return;
    }

    m_TargetEntityRefValue.m_entityRef.SetProperty("m_vValue", p_Humanoid);
    m_HumanoidRefFromEntityRefGetter.m_entityRef.SetProperty("m_entityRef", TEntityRef<IEntityRefValue>(m_TargetEntityRefValue.m_entityRef));

    const auto s_HumanoidRef = TInterfaceRef<ITEntityRefValue<ZHumanoidCharacterEntity>>::FromEntityRef(m_HumanoidRefFromEntityRefGetter.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("[Editor] Failed to get ITEntityRefValue for humanoid.");
        return;
    }

    m_TeleportTarget->SetObjectToWorldMatrixFromEditor(p_Target->GetObjectToWorldMatrix());

    m_HumanoidTeleporter.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_HumanoidTeleporter.m_entityRef.SetProperty("m_targetSpatial", m_TeleportTarget);

    m_HumanoidTeleporter.m_entityRef.SignalInputPin("Do");
}

void Editor::DisarmHumanoid(ZEntityRef p_Humanoid, EFirearmDisarmType p_FirearmDisarmType) {
    if (!SpawnEntities()) {
        return;
    }

    m_TargetEntityRefValue.m_entityRef.SetProperty("m_vValue", p_Humanoid);
    m_HumanoidRefFromEntityRefGetter.m_entityRef.SetProperty("m_entityRef", TEntityRef<IEntityRefValue>(m_TargetEntityRefValue.m_entityRef));

    const auto s_HumanoidRef = TInterfaceRef<ITEntityRefValue<ZHumanoidCharacterEntity>>::FromEntityRef(m_HumanoidRefFromEntityRefGetter.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("[Editor] Failed to get ITEntityRefValue for humanoid.");
        return;
    }

    m_HumanoidDisarmer.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_HumanoidDisarmer.m_entityRef.SetProperty("m_disarmType", p_FirearmDisarmType);

    m_HumanoidDisarmer.m_entityRef.SignalInputPin("Do");
}

ZItemCharacterEntityBase* Editor::GetEquippedItem(ZHumanoidCharacterEntity* p_HumanoidCharacterEntity) {
    if (!p_HumanoidCharacterEntity) {
        return nullptr;
    }

    auto* s_StoreData = SDK()->Globals()->KntGPWStores->m_pStoreCollection->m_pStoreData;

    auto* s_RegistryData = SDK()->Globals()->GPWTransientStores->m_pRegistryData;

    if (!s_StoreData || !s_RegistryData) {
        return nullptr;
    }

    const uint32_t s_HumanoidHandle = p_HumanoidCharacterEntity->m_StoreHandle;

    auto& s_EquippedItemStore = s_StoreData->m_HumanoidToEquippedItemHandle;

    if (!s_EquippedItemStore.IsValidHandle(s_HumanoidHandle)) {
        return nullptr;
    }

    const uint16_t s_HumanoidIndex = static_cast<uint16_t>(s_HumanoidHandle);

    const uint32_t s_ItemHandle = s_EquippedItemStore.m_Blocks[s_HumanoidIndex / 100][s_HumanoidIndex % 100];

    auto& s_ItemStore = s_RegistryData->m_ItemRegistryEntries;

    if (!s_ItemStore.IsValidHandle(s_ItemHandle)) {
        return nullptr;
    }

    const uint16_t s_ItemIndex = static_cast<uint16_t>(s_ItemHandle);

    const TInterfaceRef<ZItemCharacterEntityBase>& s_Item = s_ItemStore.m_Blocks[s_ItemIndex / 100][s_ItemIndex % 100];

    return s_Item.m_pInterfaceRef;
}

bool Editor::MatchesHumanoidFilters(ZEntityRef p_HumanoidEntity, const std::string& p_HumanoidName) {
    m_TargetEntityRefValue.m_entityRef.SetProperty("m_vValue", p_HumanoidEntity);

    const auto s_HumanoidRef = TInterfaceRef<IEntityRefValue>::FromEntityRef(m_TargetEntityRefValue.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("[Editor] Failed to get ITEntityRefValue for humanoid.");
        return false;
    }

    m_IsHumanoidAliveGetter.m_entityRef.SetProperty("m_rTarget", s_HumanoidRef);

    const bool s_IsHumanoidAlive = m_IsHumanoidAliveGetter.m_pInterfaceRef->GetValue();

    switch (m_AliveFilter) {
    case EAliveFilter::Any:
        break;
    case EAliveFilter::Alive:
        if (!s_IsHumanoidAlive) {
            return false;
        }
        break;
    case EAliveFilter::Dead:
        if (s_IsHumanoidAlive) {
            return false;
        }
        break;
    }

    bool s_MatchesType = true;
    bool s_MatchesArchetype = true;

    if (m_IsGamePlayTagFilteringEnabled && (m_FilterByType || m_FilterByArchetype)) {
        const auto s_GameplayTagListEntity = p_HumanoidEntity.QueryInterface<ZGameplayTagListEntity>();

        if (s_GameplayTagListEntity && s_GameplayTagListEntity->Tags.size() != 0) {
            s_MatchesType = !m_FilterByType;
            s_MatchesArchetype = !m_FilterByArchetype;

            for (const uint16_t s_Tag : s_GameplayTagListEntity->Tags) {
                switch (static_cast<EGameplayTag>(s_Tag)) {
                case EGameplayTag::NPC_Enemy:
                    s_MatchesType |= m_ShowEnemies;
                    break;

                case EGameplayTag::NPC_Companion:
                    s_MatchesType |= m_ShowCompanions;
                    break;

                case EGameplayTag::NPC_Authority:
                    s_MatchesType |= m_ShowAuthorities;
                    break;

                case EGameplayTag::NPC_Civilian:
                    s_MatchesType |= m_ShowCivilians;
                    break;

                case EGameplayTag::NPC_Ally:
                    s_MatchesType |= m_ShowAllies;
                    break;

                case EGameplayTag::Archetype_Grunt:
                    s_MatchesArchetype |= m_ShowGrunts;
                    break;

                case EGameplayTag::Archetype_Merc:
                    s_MatchesArchetype |= m_ShowMercs;
                    break;

                case EGameplayTag::Archetype_Merc_Armored:
                    s_MatchesArchetype |= m_ShowArmoredMercs;
                    break;

                case EGameplayTag::Archetype_Specialist:
                    s_MatchesArchetype |= m_ShowSpecialists;
                    break;

                case EGameplayTag::Archetype_Leader:
                    s_MatchesArchetype |= m_ShowLeaders;
                    break;

                case EGameplayTag::Archetype_Sniper:
                    s_MatchesArchetype |= m_ShowSnipers;
                    break;

                case EGameplayTag::Archetype_Tank:
                    s_MatchesArchetype |= m_ShowTanks;
                    break;

                case EGameplayTag::Archetype_Brute:
                    s_MatchesArchetype |= m_ShowBrutes;
                    break;

                case EGameplayTag::Archetype_Android:
                    s_MatchesArchetype |= m_ShowAndroids;
                    break;
                }

                if (s_MatchesType && s_MatchesArchetype) {
                    break;
                }
            }
        }
    }

    const bool s_MatchesName = m_HumanoidEntityName[0] == '\0' || knt::util::FindSubstring(p_HumanoidName, m_HumanoidEntityName);

    return s_MatchesName && (s_MatchesType || s_MatchesArchetype);
}
