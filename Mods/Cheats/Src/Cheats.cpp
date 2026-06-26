#include "Cheats.hpp"
#include <Glacier/ZPlayer.hpp>
#include <Glacier/ZGameLoopManager.hpp>
#include <Glacier/ZMath.hpp>
#include <Glacier/ZLoadout.hpp>
#include <Logging.hpp>
#include <Util/ImGuiUtils.hpp>

Cheats::Cheats()
    : m_ToggleNoclipAction("ToggleNoclip")
    , m_ForwardAction("Forward")
    , m_BackwardAction("Backward")
    , m_LeftAction("Left")
    , m_RightAction("Right")
    , m_FastAction("Fast") {}

Cheats::~Cheats() {
    if (m_FrameUpdateRegistered) {
        const ZMemberDelegate<Cheats, void(const SGameUpdateEvent&)> s_Delegate(this, &Cheats::OnFrameUpdate);
        SDK()->Globals()->GameLoopManager->UnregisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
        m_FrameUpdateRegistered = false;
    }

    CleanupSpawnedEntities();
}

void Cheats::CleanupSpawnedEntities() {
    const auto s_Delete = [](auto* p_Ref) {
        if (*p_Ref) {
            SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, p_Ref->m_entityRef);
        }
        *p_Ref = {};
    };

    s_Delete(&m_Teleporter);
    s_Delete(&m_TeleportTarget);
    s_Delete(&m_LocalPlayerHumanoidGetter);
    s_Delete(&m_CollisionModifier);
    s_Delete(&m_ImmuneModifier);
    s_Delete(&m_UnkillableModifier);
    s_Delete(&m_InfiniteAmmoModifier);
    s_Delete(&m_InvisibleModifier);
    s_Delete(&m_LocalPlayerIDGetter);
    s_Delete(&m_SetHumanoidOutfit);
    s_Delete(&m_ImmuneBoolValue);
    s_Delete(&m_UnkillableBoolValue);
    s_Delete(&m_InvisibleBoolValue);
    s_Delete(&m_CurrentElectricityGetter);
    s_Delete(&m_CurrentChemicalGetter);
    s_Delete(&m_MaximumElectricityGetter);
    s_Delete(&m_MaximumChemicalGetter);
    s_Delete(&m_ElectricityGiver);
    s_Delete(&m_ChemicalGiver);
    s_Delete(&s_ElectricityAmountFloatValue);
    s_Delete(&s_ChemicalAmountFloatValue);
    s_Delete(&m_GadgetSpawner);
    s_Delete(&m_GadgetSpawnerItemEntry);
    s_Delete(&m_GadgetAttacher);
    s_Delete(&m_GadgetSlotAssigner);

    for (auto& s_AmmunitionGetter : m_AmmunitionGetters) {
        s_Delete(&s_AmmunitionGetter);
    }

    for (auto& s_AmmunitionSetter : m_AmmunitionSetters) {
        s_Delete(&s_AmmunitionSetter);
    }
}

void Cheats::Init() {
    SDK()->Hooks()->ZKntLoadoutCollectionEntity_ZKntLoadoutCollectionEntity->AddDetour(
        this, &Cheats::ZKntLoadoutCollectionEntity_ZKntLoadoutCollectionEntity
    );
}

void Cheats::OnEngineInitialized() {
    const ZMemberDelegate<Cheats, void(const SGameUpdateEvent&)> s_Delegate(this, &Cheats::OnFrameUpdate);
    SDK()->Globals()->GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
    m_FrameUpdateRegistered = true;

    const char* s_Bindings = "NoclipInput={"
                             "ToggleNoclip=& hold(kb,lctrl) tap(kb,n);"
                             "Forward=hold(kb,w);"
                             "Backward=hold(kb,s);"
                             "Left=hold(kb,a);"
                             "Right=hold(kb,d);"
                             "Fast=hold(kb,lshift) | hold(kb,rshift);};";

    ZInputContext* s_InputContext = SDK()->Functions()->GetGlobalInputContext->Call();

    SDK()->Functions()->AddBindings->Call(s_Bindings, s_InputContext);
}

void Cheats::OnDrawMenu() {
    if (ImGui::Button("CHEATS")) {
        m_ShowPanel = !m_ShowPanel;
    }
}

void Cheats::OnDrawUI(bool p_HasFocus) {
    if (!m_ShowPanel || !p_HasFocus) {
        return;
    }

    ImGui::SetNextWindowSize({500, 500}, ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Cheats", &m_ShowPanel)) {
        ImGui::Checkbox("Noclip (Ctrl+N)", &m_NoclipEnabled);

        // Collision is forced off while noclip is active.
        ImGui::BeginDisabled(m_NoclipEnabled);
        bool s_DisableCollision = m_NoclipEnabled || m_DisableCollision;
        if (ImGui::Checkbox("Disable collision", &s_DisableCollision)) {
            m_DisableCollision = s_DisableCollision;
            m_StateDirty = true;
        }
        ImGui::EndDisabled();

        ImGui::Separator();

        m_StateDirty |= ImGui::Checkbox("God mode (invincible)", &m_GodMode);
        m_StateDirty |= ImGui::Checkbox("Buddha mode (unkillable)", &m_Unkillable);
        m_StateDirty |= ImGui::Checkbox("Infinite ammo", &m_InfiniteAmmo);
        m_StateDirty |= ImGui::Checkbox("Invisible", &m_Invisible);
        ImGui::Checkbox("Q-Lens: Infinite electricity", &m_InfiniteElectricity);
        ImGui::Checkbox("Q-Lens: Infinite chemical", &m_InfiniteChecmical);

        ImGui::Separator();

        if (m_KntLoadoutCollectionEntity && m_OutfitCategories.empty()) {
            LoadPlayerOutfitSets();
        }

        static char s_OutfitCategory[1024]{};
        static char s_Outfit[1024]{};
        static char s_OutfitVariation[1024]{};

        static const std::set<std::string>* s_CategoryOutfits = nullptr;
        static const OutfitInfo* s_OutfitInfo = nullptr;

        ImGui::Text("Player outfits");

        ImGui::BeginDisabled(!m_KntLoadoutCollectionEntity || m_OutfitCategories.empty());

        Util::ImGuiUtils::InputWithAutocomplete(
            "Outfit category##OutfitCategory", s_OutfitCategory, sizeof(s_OutfitCategory), m_OutfitCategoryToOutfits,
            [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [&](const std::string&, const std::string& p_Name, const auto& p_Pair) {
                s_CategoryOutfits = &p_Pair.second;
                s_OutfitInfo = nullptr;

                s_Outfit[0] = '\0';
                s_OutfitVariation[0] = '\0';
            }
        );

        ImGui::EndDisabled();

        ImGui::BeginDisabled(!m_KntLoadoutCollectionEntity || m_OutfitCategories.empty());

        Util::ImGuiUtils::InputWithAutocomplete(
            "Outfit##Outfit", s_Outfit, sizeof(s_Outfit), s_CategoryOutfits ? *s_CategoryOutfits : std::set<std::string>{},
            [](const auto& p_Outfit) -> const std::string { return p_Outfit; }, [](const auto& p_Outfit) -> const std::string { return p_Outfit; },
            [&](const std::string&, const std::string& p_Name, const auto&) {
                if (const auto it = m_OutfitNameToOutfitInfo.find(p_Name); it != m_OutfitNameToOutfitInfo.end()) {
                    s_OutfitInfo = &it->second;
                    s_OutfitVariation[0] = '\0';
                }
            }
        );

        ImGui::EndDisabled();

        ImGui::BeginDisabled(!m_KntLoadoutCollectionEntity || m_OutfitCategories.empty());

        Util::ImGuiUtils::InputWithAutocomplete(
            "Outfit variation##OutfitVariation", s_OutfitVariation, sizeof(s_OutfitVariation),
            s_OutfitInfo ? s_OutfitInfo->m_Variations : std::vector<std::pair<std::string, size_t>>{},
            [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [&](const std::string&, const std::string&, const auto& p_Pair) { SetPlayerOutfit(s_OutfitInfo->m_OutfitSet, p_Pair.second); }
        );

        ImGui::EndDisabled();

        ImGui::Separator();

        ImGui::Text("All outfits");

        static char s_Outfit2[1024]{""};
        static char s_OutfitVariation2[1024]{""};
        static const OutfitInfo* s_OutfitInfo2 = nullptr;

        ImGui::BeginDisabled(m_AllOutfitSets.empty());

        Util::ImGuiUtils::InputWithAutocomplete(
            "Outfit set##OutfitSets", s_Outfit2, sizeof(s_Outfit2), m_AllOutfitSets, [](auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [](auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [&](const std::string&, const std::string& p_Name, const auto& p_Pair) {
                if (const auto it = m_AllOutfitSets.find(p_Name); it != m_AllOutfitSets.end()) {
                    s_OutfitInfo2 = &it->second;
                    s_OutfitVariation2[0] = '\0';
                }
            }
        );

        ImGui::EndDisabled();

        ImGui::BeginDisabled(m_AllOutfitSets.empty() || !s_OutfitInfo2);

        Util::ImGuiUtils::InputWithAutocomplete(
            "Outfit variation##OutfitVariations", s_OutfitVariation2, sizeof(s_OutfitVariation2),
            s_OutfitInfo2 ? s_OutfitInfo2->m_Variations : std::vector<std::pair<std::string, size_t>>{},
            [](auto& p_Pair) -> std::string { return p_Pair.first; }, [](auto& p_Pair) -> std::string { return p_Pair.first; },
            [&](const std::string&, const std::string& p_Name, const std::pair<std::string, size_t>& p_Value) {
                SetPlayerOutfit(s_OutfitInfo2->m_OutfitSet, p_Value.second);
            }
        );

        ImGui::EndDisabled();

        ImGui::BeginDisabled(!m_AllOutfitSets.empty());

        if (ImGui::Button("Get all outfits")) {
            LoadAllOutfitSets();
        }

        ImGui::EndDisabled();

        ImGui::Separator();

        if (m_KntLoadoutCollectionEntity && m_Gadgets.empty()) {
            LoadGadgets();
        }

        static char s_Slot1Gadget[1024]{};
        static char s_Slot2Gadget[1024]{};
        static char s_Slot3Gadget[1024]{};
        static char s_Slot4Gadget[1024]{};

        ImGui::Text("Spawn gadgets");

        ImGui::BeginDisabled(m_Gadgets.empty());

        Util::ImGuiUtils::InputWithAutocomplete(
            "Slot 1##Slot1", s_Slot1Gadget, sizeof(s_Slot1Gadget), m_Gadgets, [](auto& p_GadgetInfo) -> std::string { return p_GadgetInfo.m_Name; },
            [](auto& p_GadgetInfo) -> std::string { return p_GadgetInfo.m_Name; },
            [&](const std::string&, const std::string& p_Name, const GadgetInfo& p_GadgetInfo) {
                SpawnGadget(p_GadgetInfo.m_GadgetItemDefinition, p_GadgetInfo.m_ItemTemplate, Gameplay::EGadgetActivationSlot::Slot1);
            }
        );

        Util::ImGuiUtils::InputWithAutocomplete(
            "Slot 2##Slot2", s_Slot2Gadget, sizeof(s_Slot2Gadget), m_Gadgets, [](auto& p_GadgetInfo) -> std::string { return p_GadgetInfo.m_Name; },
            [](auto& p_GadgetInfo) -> std::string { return p_GadgetInfo.m_Name; },
            [&](const std::string&, const std::string& p_Name, const GadgetInfo& p_GadgetInfo) {
                SpawnGadget(p_GadgetInfo.m_GadgetItemDefinition, p_GadgetInfo.m_ItemTemplate, Gameplay::EGadgetActivationSlot::Slot2);
            }
        );

        Util::ImGuiUtils::InputWithAutocomplete(
            "Slot 3##Slot3", s_Slot3Gadget, sizeof(s_Slot3Gadget), m_Gadgets, [](auto& p_GadgetInfo) -> std::string { return p_GadgetInfo.m_Name; },
            [](auto& p_GadgetInfo) -> std::string { return p_GadgetInfo.m_Name; },
            [&](const std::string&, const std::string& p_Name, const GadgetInfo& p_GadgetInfo) {
                SpawnGadget(p_GadgetInfo.m_GadgetItemDefinition, p_GadgetInfo.m_ItemTemplate, Gameplay::EGadgetActivationSlot::Slot3);
            }
        );

        Util::ImGuiUtils::InputWithAutocomplete(
            "Slot 4##Slot4", s_Slot4Gadget, sizeof(s_Slot4Gadget), m_Gadgets, [](auto& p_GadgetInfo) -> std::string { return p_GadgetInfo.m_Name; },
            [](auto& p_GadgetInfo) -> std::string { return p_GadgetInfo.m_Name; },
            [&](const std::string&, const std::string& p_Name, const GadgetInfo& p_GadgetInfo) {
                SpawnGadget(p_GadgetInfo.m_GadgetItemDefinition, p_GadgetInfo.m_ItemTemplate, Gameplay::EGadgetActivationSlot::Slot4);
            }
        );

        ImGui::EndDisabled();
    }

    ImGui::End();
}

void Cheats::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) {
    if (!SDK()->Globals()->LocalPlayerData->m_pCharacterImpl) {
        return;
    }

    auto s_LocalPlayer = SDK()->Globals()->LocalPlayerData->m_pCharacterImpl->m_pCharacter;
    if (!s_LocalPlayer) {
        return;
    }

    const auto s_PlayerSpatialEntity = s_LocalPlayer.m_entityRef.QueryInterface<ZSpatialEntity>();
    if (!s_PlayerSpatialEntity) {
        return;
    }

    const auto s_CurrentCamera = SDK()->Functions()->GetCurrentCamera->Call();
    if (!s_CurrentCamera) {
        return;
    }

    if (m_ToggleNoclipAction.Digital()) {
        m_NoclipEnabled = !m_NoclipEnabled;
    }

    if (m_NoclipEnabled != m_NoclipWasEnabled) {
        if (m_NoclipEnabled) {
            m_PlayerPosition = s_PlayerSpatialEntity->GetObjectToWorldMatrix();
        }

        m_StateDirty = true;
        m_NoclipWasEnabled = m_NoclipEnabled;
    }

    if (AnyCheatEnabled() && !EnsureEntitiesSpawned()) {
        return;
    }

    if (m_StateDirty && m_Teleporter) {
        ApplyPlayerModifiers();
        m_StateDirty = false;
    }

    if (m_InfiniteElectricity && m_CurrentElectricityGetter && m_MaximumElectricityGetter) {
        if (m_CurrentElectricityGetter.m_pInterfaceRef->GetValue() < m_MaximumElectricityGetter.m_pInterfaceRef->GetValue()) {
            s_ElectricityAmountFloatValue.m_entityRef.SetProperty<float32>("m_nValue", m_MaximumElectricityGetter.m_pInterfaceRef->GetValue());
            m_ElectricityGiver.m_entityRef.SignalInputPin("Do");
        }
    }

    if (m_InfiniteChecmical && m_CurrentChemicalGetter && m_MaximumChemicalGetter) {
        if (m_CurrentChemicalGetter.m_pInterfaceRef->GetValue() < m_MaximumChemicalGetter.m_pInterfaceRef->GetValue()) {
            s_ChemicalAmountFloatValue.m_entityRef.SetProperty<float32>("m_nValue", m_MaximumElectricityGetter.m_pInterfaceRef->GetValue());
            m_ChemicalGiver.m_entityRef.SignalInputPin("Do");
        }
    }

    if (m_AssignGadgetToSlot && !m_GadgetSpawner.m_pInterfaceRef->IsSpawning) {
        m_GadgetAttacher.m_entityRef.SignalInputPin("Do");
        m_GadgetSlotAssigner.m_entityRef.SignalInputPin("Do");

        m_AssignGadgetToSlot = false;
    }

    if (m_InfiniteAmmo) {
        for (size_t i = 0; i < m_AmmunitionGetters.size(); ++i) {
            if (m_AmmunitionGetters[i].m_pInterfaceRef->GetValue() == 0) {
                m_AmmunitionSetters[i].m_entityRef.SetProperty<uint32>("m_ammunitionToGive", 1);
                m_AmmunitionSetters[i].m_entityRef.SignalInputPin("Do");
            }
        }
    }

    if (!m_NoclipEnabled) {
        return;
    }

    auto s_CameraTrans = s_CurrentCamera->GetObjectToWorldMatrix();

    // Meters per second.
    float s_MoveSpeed = 5.f;

    if (m_FastAction.Digital()) {
        s_MoveSpeed = 20.f;
    }

    if (m_ForwardAction.Digital()) {
        m_PlayerPosition.Trans += s_CameraTrans.Up * -s_MoveSpeed * p_UpdateEvent.m_GameTimeDelta.ToSeconds();
    }

    if (m_BackwardAction.Digital()) {
        m_PlayerPosition.Trans += s_CameraTrans.Up * s_MoveSpeed * p_UpdateEvent.m_GameTimeDelta.ToSeconds();
    }

    if (m_LeftAction.Digital()) {
        m_PlayerPosition.Trans += s_CameraTrans.Right * -s_MoveSpeed * p_UpdateEvent.m_GameTimeDelta.ToSeconds();
    }

    if (m_RightAction.Digital()) {
        m_PlayerPosition.Trans += s_CameraTrans.Right * s_MoveSpeed * p_UpdateEvent.m_GameTimeDelta.ToSeconds();
    }

    m_TeleportTarget->SetObjectToWorldMatrixFromEditor(m_PlayerPosition);
    m_Teleporter.m_entityRef.SignalInputPin("Do");
}

bool Cheats::EnsureEntitiesSpawned() {
    if (m_Teleporter) {
        return true;
    }

    m_Teleporter = TEntityRef<ZCLTeleportHumanoidEntity>::SpawnEntity(ResId<"[modules:/zclteleporthumanoidentity.class].entitytype">);
    m_TeleportTarget = TEntityRef<ZSpatialEntity>::SpawnEntity(ResId<"[modules:/zspatialentity.class].entitytype">);
    m_LocalPlayerHumanoidGetter =
        TEntityRef<ZCLGetLocalPlayerHumanoidCharacter>::SpawnEntity(ResId<"[modules:/zclgetlocalplayerhumanoidcharacter.class].entitytype">);
    m_CollisionModifier =
        TEntityRef<ZCLEnableDisableHumanoidCollision>::SpawnEntity(ResId<"[modules:/zclenabledisablehumanoidcollision.class].entitytype">);
    m_ImmuneModifier = TEntityRef<ZCLSetHumanoidImmuneToDamage>::SpawnEntity(ResId<"[modules:/zclsethumanoidimmunetodamage.class].entitytype">);
    m_UnkillableModifier =
        TEntityRef<ZCLSetHumanoidUnkillableByDamage>::SpawnEntity(ResId<"[modules:/zclsethumanoidunkillablebydamage.class].entitytype">);
    m_InfiniteAmmoModifier =
        TEntityRef<ZCLSetHumanoidInfiniteClipAmmo>::SpawnEntity(ResId<"[modules:/zclsethumanoidinfiniteclipammo.class].entitytype">);
    m_InvisibleModifier = TEntityRef<ZCLSetPlayerInvisibleToNpcs>::SpawnEntity(ResId<"[modules:/zclsetplayerinvisibletonpcs.class].entitytype">);
    m_LocalPlayerIDGetter = TEntityRef<ZCLGetLocalPlayerID>::SpawnEntity(ResId<"[modules:/zclgetlocalplayerid.class].entitytype">);
    m_SetHumanoidOutfit = TEntityRef<ZCLSetHumanoidOutfitEntity>::SpawnEntity(ResId<"[modules:/zclsethumanoidoutfitentity.class].entitytype">);
    m_ImmuneBoolValue = TEntityRef<ZCLValueBoolEntity>::SpawnEntity(ResId<"[modules:/zclvalueboolentity.class].entitytype">);
    m_UnkillableBoolValue = TEntityRef<ZCLValueBoolEntity>::SpawnEntity(ResId<"[modules:/zclvalueboolentity.class].entitytype">);
    m_InvisibleBoolValue = TEntityRef<ZCLValueBoolEntity>::SpawnEntity(ResId<"[modules:/zclvalueboolentity.class].entitytype">);
    m_CurrentElectricityGetter =
        TEntityRef<ZCLGetCurrentPlayerResource>::SpawnEntity(ResId<"[modules:/zclgetcurrentplayerresource.class].entitytype">);
    m_CurrentChemicalGetter = TEntityRef<ZCLGetCurrentPlayerResource>::SpawnEntity(ResId<"[modules:/zclgetcurrentplayerresource.class].entitytype">);
    m_MaximumElectricityGetter =
        TEntityRef<ZCLGetMaximumPlayerResource>::SpawnEntity(ResId<"[modules:/zclgetmaximumplayerresource.class].entitytype">);
    m_MaximumChemicalGetter = TEntityRef<ZCLGetMaximumPlayerResource>::SpawnEntity(ResId<"[modules:/zclgetmaximumplayerresource.class].entitytype">);
    m_ElectricityGiver = TEntityRef<ZCLGiveResourceToPlayer>::SpawnEntity(ResId<"[modules:/zclgiveresourcetoplayer.class].entitytype">);
    m_ChemicalGiver = TEntityRef<ZCLGiveResourceToPlayer>::SpawnEntity(ResId<"[modules:/zclgiveresourcetoplayer.class].entitytype">);
    s_ElectricityAmountFloatValue = TEntityRef<ZCLValueFloatEntity>::SpawnEntity(ResId<"[modules:/zclvaluefloatentity.class].entitytype">);
    s_ChemicalAmountFloatValue = TEntityRef<ZCLValueFloatEntity>::SpawnEntity(ResId<"[modules:/zclvaluefloatentity.class].entitytype">);
    m_GadgetSpawner = TEntityRef<ZDynamicGameplaySpawnerEntity>::SpawnEntity(ResId<"[modules:/zdynamicgameplayspawnerentity.class].entitytype">);
    m_GadgetSpawnerItemEntry =
        TEntityRef<ZDynamicGameplaySpawnerItemEntryEntity>::SpawnEntity(ResId<"[modules:/zdynamicgameplayspawneritementryentity.class].entitytype">);
    m_GadgetAttacher = TEntityRef<ZCLAttachItemToHumanoid>::SpawnEntity(ResId<"[modules:/zclattachitemtohumanoid.class].entitytype">);
    m_GadgetSlotAssigner = TEntityRef<ZCLAssignGadgetToSlot>::SpawnEntity(ResId<"[modules:/zclassigngadgettoslot.class].entitytype">);

    for (size_t i = 0; i < m_AmmunitionGetters.size(); ++i) {
        m_AmmunitionGetters[i] =
            TEntityRef<ZCLGetPlayerInventoryAmmunition>::SpawnEntity(ResId<"[modules:/zclgetplayerinventoryammunition.class].entitytype">);
    }

    for (size_t i = 0; i < m_AmmunitionSetters.size(); ++i) {
        m_AmmunitionSetters[i] =
            TEntityRef<ZCLGiveHumanoidPlayerAmmunition>::SpawnEntity(ResId<"[modules:/zclgivehumanoidplayerammunition.class].entitytype">);
    }

    const bool s_AreAmmunitionGettersSpawned =
        std::ranges::all_of(m_AmmunitionGetters, [](const auto& p_AmmunitionGetter) { return static_cast<bool>(p_AmmunitionGetter); });
    const bool s_AreAmmunitionSettersSpawned =
        std::ranges::all_of(m_AmmunitionSetters, [](const auto& p_AmmunitionSetter) { return static_cast<bool>(p_AmmunitionSetter); });

    if (!m_Teleporter || !m_TeleportTarget || !m_LocalPlayerHumanoidGetter || !m_CollisionModifier || !m_ImmuneModifier || !m_UnkillableModifier
        || !m_InfiniteAmmoModifier || !m_InvisibleModifier || !m_LocalPlayerIDGetter || !m_SetHumanoidOutfit || !m_ImmuneBoolValue
        || !m_UnkillableBoolValue || !m_InvisibleBoolValue || !m_CurrentElectricityGetter || !m_CurrentChemicalGetter || !m_MaximumElectricityGetter
        || !m_MaximumChemicalGetter || !m_ElectricityGiver || !m_ChemicalGiver || !s_ElectricityAmountFloatValue || !s_ChemicalAmountFloatValue
        || !s_AreAmmunitionGettersSpawned || !s_AreAmmunitionSettersSpawned) {
        Logger::Error(
            "Failed to spawn some cheat entities. Teleporter: {}, TeleportTarget: {}, LocalPlayerHumanoidGetter: {}, CollisionModifier: {}, "
            "ImmuneModifier: {}, UnkillableModifier: {}, InfiniteAmmoModifier: {}, InvisibleModifier: {}, LocalPlayerIDGetter: {}, "
            "SetHumanoidOutfit: {}, ImmuneBoolValue: {}, UnkillableBoolValue: {}, InvisibleBoolValue: {}, CurrentElectricityGetter: {}, "
            "CurrentChemicalGetter: {}, MaximumElectricityGetter: {}, MaximumChemicalGetter: {}, ElectricityGiver: {}, ChemicalGiver: {}, "
            "ElectricityAmountFloatValue: {}, ChemicalAmountFloatValue: {}, GadgetSpawner: {}, GadgetSpawnerItemEntry: {}, GadgetAttacher: {}, "
            "GadgetSlotAssigner: {}",
            static_cast<bool>(m_Teleporter), static_cast<bool>(m_TeleportTarget), static_cast<bool>(m_LocalPlayerHumanoidGetter),
            static_cast<bool>(m_CollisionModifier), static_cast<bool>(m_ImmuneModifier), static_cast<bool>(m_UnkillableModifier),
            static_cast<bool>(m_InfiniteAmmoModifier), static_cast<bool>(m_InvisibleModifier), static_cast<bool>(m_LocalPlayerIDGetter),
            static_cast<bool>(m_SetHumanoidOutfit), static_cast<bool>(m_ImmuneBoolValue), static_cast<bool>(m_UnkillableBoolValue),
            static_cast<bool>(m_InvisibleBoolValue), static_cast<bool>(m_CurrentElectricityGetter), static_cast<bool>(m_CurrentChemicalGetter),
            static_cast<bool>(m_MaximumElectricityGetter), static_cast<bool>(m_MaximumChemicalGetter), static_cast<bool>(m_ElectricityGiver),
            static_cast<bool>(m_ChemicalGiver), static_cast<bool>(s_ElectricityAmountFloatValue), static_cast<bool>(s_ChemicalAmountFloatValue),
            static_cast<bool>(m_GadgetSpawner), static_cast<bool>(m_GadgetSpawnerItemEntry), static_cast<bool>(m_GadgetAttacher),
            static_cast<bool>(m_GadgetSlotAssigner), s_AreAmmunitionGettersSpawned, s_AreAmmunitionSettersSpawned
        );
        CleanupSpawnedEntities();
        return false;
    }

    const auto s_HumanoidRef = TInterfaceRef<ITEntityRefValue<ZHumanoidCharacterEntity>>::FromEntityRef(m_LocalPlayerHumanoidGetter.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("Failed to get ITEntityRefValue for player.");
        CleanupSpawnedEntities();
        return false;
    }

    const auto s_PlayerIDRef = TInterfaceRef<IIntValue>::FromEntityRef(m_LocalPlayerIDGetter.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("Failed to get IIntValue for player.");
        CleanupSpawnedEntities();
        return false;
    }

    // Point every humanoid-targeting modifier at the local player.
    m_Teleporter.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_Teleporter.m_entityRef.SetProperty("m_targetSpatial", m_TeleportTarget);
    m_CollisionModifier.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_ImmuneModifier.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_UnkillableModifier.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_InfiniteAmmoModifier.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_GadgetAttacher.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);

    // Point every player ID-targeting modifier at the local player's ID.
    m_InvisibleModifier.m_entityRef.SetProperty("m_playerID", s_PlayerIDRef);
    m_CurrentElectricityGetter.m_entityRef.SetProperty("m_playerID", s_PlayerIDRef);
    m_CurrentChemicalGetter.m_entityRef.SetProperty("m_playerID", s_PlayerIDRef);
    m_ElectricityGiver.m_entityRef.SetProperty("m_playerID", s_PlayerIDRef);
    m_ChemicalGiver.m_entityRef.SetProperty("m_playerID", s_PlayerIDRef);

    for (size_t i = 0; i < m_AmmunitionSetters.size(); ++i) {
        m_AmmunitionSetters[i].m_entityRef.SetProperty("m_playerID", s_PlayerIDRef);
    }

    // Wire the immune/unkillable/invisible modifiers to their bool-value sources.
    const auto s_ImmuneBoolRef = TInterfaceRef<IBoolValue>::FromEntityRef(m_ImmuneBoolValue.m_entityRef);
    const auto s_UnkillableBoolRef = TInterfaceRef<IBoolValue>::FromEntityRef(m_UnkillableBoolValue.m_entityRef);
    const auto s_InvisibleBoolRef = TInterfaceRef<IBoolValue>::FromEntityRef(m_InvisibleBoolValue.m_entityRef);

    if (!s_ImmuneBoolRef || !s_UnkillableBoolRef || !s_InvisibleBoolRef) {
        Logger::Error("Failed to get IBoolValue ref for modifiers.");
        CleanupSpawnedEntities();
        return false;
    }

    m_ImmuneModifier.m_entityRef.SetProperty("m_invulnerable", s_ImmuneBoolRef);
    m_UnkillableModifier.m_entityRef.SetProperty("m_isUnkillable", s_UnkillableBoolRef);
    m_InvisibleModifier.m_entityRef.SetProperty("m_invisible", s_InvisibleBoolRef);

    TResourcePtr<ZEntityRef> s_ElectricResourceDefinitionPtr;
    SDK()->Globals()->ResourceManager->LoadResource(
        s_ElectricResourceDefinitionPtr, ResId<"[assembly:/_knt/design/gadgets/gadget_resource_definitions.template?/"
                                               "playerresource_def_electric.entitytemplate].entityresource">
    );

    TResourcePtr<ZEntityRef> s_ChemicalResourceDefinitionPtr;
    SDK()->Globals()->ResourceManager->LoadResource(
        s_ChemicalResourceDefinitionPtr, ResId<"[assembly:/_knt/design/gadgets/gadget_resource_definitions.template?/"
                                               "playerresource_def_chemical.entitytemplate].entityresource">
    );

    m_CurrentElectricityGetter.m_entityRef.SetProperty("m_resourceDefinition", s_ElectricResourceDefinitionPtr);
    m_CurrentChemicalGetter.m_entityRef.SetProperty("m_resourceDefinition", s_ChemicalResourceDefinitionPtr);
    m_MaximumElectricityGetter.m_entityRef.SetProperty("m_resourceDefinition", s_ElectricResourceDefinitionPtr);
    m_MaximumChemicalGetter.m_entityRef.SetProperty("m_resourceDefinition", s_ChemicalResourceDefinitionPtr);
    m_ElectricityGiver.m_entityRef.SetProperty("m_resourceDefinition", s_ElectricResourceDefinitionPtr);
    m_ChemicalGiver.m_entityRef.SetProperty("m_resourceDefinition", s_ChemicalResourceDefinitionPtr);

    const auto s_ElectricityAmountFloatRef = TInterfaceRef<IFloatValue>::FromEntityRef(s_ElectricityAmountFloatValue.m_entityRef);
    const auto s_ChemicalAmountFloatRef = TInterfaceRef<IFloatValue>::FromEntityRef(s_ChemicalAmountFloatValue.m_entityRef);

    if (!s_ElectricityAmountFloatRef || !s_ChemicalAmountFloatRef) {
        Logger::Error("Failed to get IFloatValue ref for electricity and chemical resource amounts.");
        CleanupSpawnedEntities();
        return false;
    }

    m_ElectricityGiver.m_entityRef.SetProperty("m_amount", s_ElectricityAmountFloatRef);
    m_ChemicalGiver.m_entityRef.SetProperty("m_amount", s_ChemicalAmountFloatRef);

    for (size_t i = 0; i < m_AmmunitionGetters.size(); ++i) {
        m_AmmunitionGetters[i].m_entityRef.SetProperty("m_firearmClass", static_cast<EFirearmClass>(i));
    }

    for (size_t i = 0; i < m_AmmunitionSetters.size(); ++i) {
        m_AmmunitionSetters[i].m_entityRef.SetProperty("m_firearmClass", static_cast<EFirearmClass>(i));
    }

    // Make sure the freshly spawned entities pick up the current toggle states.
    m_StateDirty = true;

    Logger::Info("Cheat entities spawned!");

    return true;
}

bool Cheats::AnyCheatEnabled() const {
    return m_NoclipEnabled || m_DisableCollision || m_GodMode || m_Unkillable || m_InfiniteAmmo || m_Invisible || m_InfiniteElectricity
           || m_InfiniteChecmical;
}

void Cheats::ApplyPlayerModifiers() {
    m_ImmuneBoolValue.m_entityRef.SetProperty("m_bValue", m_GodMode);
    m_ImmuneModifier.m_entityRef.SignalInputPin("Do");

    m_UnkillableBoolValue.m_entityRef.SetProperty("m_bValue", m_Unkillable);
    m_UnkillableModifier.m_entityRef.SignalInputPin("Do");

    m_InfiniteAmmoModifier.m_entityRef.SetProperty("m_infiniteAmmo", m_InfiniteAmmo);
    m_InfiniteAmmoModifier.m_entityRef.SignalInputPin("Do");

    m_InvisibleBoolValue.m_entityRef.SetProperty("m_bValue", m_Invisible);
    m_InvisibleModifier.m_entityRef.SignalInputPin("Do");

    // Collision is forced off while noclip is active.
    const bool s_CollisionEnabled = !(m_NoclipEnabled || m_DisableCollision);
    m_CollisionModifier.m_entityRef.SetProperty("m_collisionEnabled", s_CollisionEnabled);
    m_CollisionModifier.m_entityRef.SignalInputPin("Do");
}

void Cheats::LoadPlayerOutfitSets() {
    if (!m_KntLoadoutCollectionEntity) {
        return;
    }

    m_OutfitCategories.push_back("All");

    for (const auto& s_OutfitCategoryResourcePtr : m_KntLoadoutCollectionEntity->m_outfitCategories) {
        SEntityResource* s_OutfitCategoryEntityResource = static_cast<SEntityResource*>(s_OutfitCategoryResourcePtr.GetResourceData());
        ZOutfitCategory* s_OutfitCategory = s_OutfitCategoryEntityResource->entityRef.QueryInterface<ZOutfitCategory>();

        m_OutfitCategories.push_back(s_OutfitCategory->m_titleRaw.c_str());

        for (const auto& s_OutfitDefinitionResourcePtr : s_OutfitCategory->m_outfits) {
            SEntityResource* s_OutfitDefinitionEntityResource = static_cast<SEntityResource*>(s_OutfitDefinitionResourcePtr.GetResourceData());
            ZOutfitDefinitionEntity* s_OutfitDefinitionEntity = s_OutfitDefinitionEntityResource->entityRef.QueryInterface<ZOutfitDefinitionEntity>();

            ZTextLine* s_TextLine = static_cast<ZTextLine*>(s_OutfitDefinitionEntity->m_outfitDisplayNameSweet.GetResourceData());
            ZTextListData* s_TextListData = static_cast<ZTextListData*>(s_TextLine->m_pTextList.GetResourceData());
            ZString s_Name = s_TextLine->GetText();

            m_OutfitCategoryToOutfits[s_OutfitCategory->m_titleRaw.c_str()].insert(s_Name.c_str());

            SEntityResource* s_OutfitSetEntityResource = static_cast<SEntityResource*>(s_OutfitDefinitionEntity->m_outfitSet.GetResourceData());
            ZHumanoidOutfitSet* s_HumanoidOutfitSet = s_OutfitSetEntityResource->entityRef.QueryInterface<ZHumanoidOutfitSet>();

            ZTemplateEntityBlueprintFactory* s_TemplateEntityBlueprintFactory = static_cast<ZTemplateEntityBlueprintFactory*>(
                static_cast<IEntityFactory*>(s_OutfitSetEntityResource->factoryResource.GetResourceData())->GetBlueprint()
            );

            OutfitInfo& s_OutfitInfo = m_OutfitNameToOutfitInfo[s_Name.c_str()];
            s_OutfitInfo.m_OutfitSet = s_OutfitDefinitionEntity->m_outfitSet.GetResourceInfo().rid;

            const auto s_SubEntityCount = s_TemplateEntityBlueprintFactory->GetSubEntitiesCount();

            for (const auto& s_HumanoidOutfitReferenceInterfaceRef : s_HumanoidOutfitSet->m_outfits) {
                for (int i = 0; i < s_SubEntityCount; ++i) {
                    if (i == s_TemplateEntityBlueprintFactory->m_rootEntityIndex) {
                        continue;
                    }

                    const ZEntityRef s_SubEntity = s_TemplateEntityBlueprintFactory->GetSubEntity(s_OutfitSetEntityResource->entityRef.m_pObj, i);
                    ZHumanoidOutfitReference* s_HumanoidOutfitReference = s_SubEntity.QueryInterface<ZHumanoidOutfitReference>();

                    if (s_HumanoidOutfitReferenceInterfaceRef.m_pInterface == s_HumanoidOutfitReference) {
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

void Cheats::LoadAllOutfitSets() {
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

                OutfitInfo& s_OutfitInfo = m_AllOutfitSets[s_RootEntityName];
                s_OutfitInfo.m_OutfitSet = s_ResourceInfo.rid;

                const auto s_SubEntityCount = s_TemplateEntityBlueprintFactory->GetSubEntitiesCount();

                for (const auto& s_HumanoidOutfitReferenceInterfaceRef : s_HumanoidOutfitSet->m_outfits) {
                    for (int i = 0; i < s_SubEntityCount; ++i) {
                        if (i == s_TemplateEntityBlueprintFactory->m_rootEntityIndex) {
                            continue;
                        }

                        const ZEntityRef s_SubEntity = s_TemplateEntityBlueprintFactory->GetSubEntity(s_EntityResource->entityRef.m_pObj, i);
                        ZHumanoidOutfitReference* s_HumanoidOutfitReference = s_SubEntity.QueryInterface<ZHumanoidOutfitReference>();

                        if (s_HumanoidOutfitReferenceInterfaceRef.m_pInterface == s_HumanoidOutfitReference) {
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

void Cheats::SetPlayerOutfit(const ZRuntimeResourceID& p_OutfitSetRuntimeResourceID, size_t p_OutfitVariationIndex) {
    if (!EnsureEntitiesSpawned()) {
        return;
    }

    const auto s_HumanoidRef = TInterfaceRef<ITEntityRefValue<ZHumanoidCharacterEntity>>::FromEntityRef(m_LocalPlayerHumanoidGetter.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("Failed to get ITEntityRefValue for player.");
        CleanupSpawnedEntities();
        return;
    }

    TResourcePtr<ZEntityRef> s_ResourcePtr;
    SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, p_OutfitSetRuntimeResourceID);

    m_SetHumanoidOutfit.m_entityRef.SetProperty("m_humanoid", s_HumanoidRef);
    m_SetHumanoidOutfit.m_entityRef.SetProperty<TResourcePtr<ZEntityRef>>("m_outfitSet", s_ResourcePtr);
    m_SetHumanoidOutfit.m_entityRef.SetProperty<int32_t>("m_selectedOutfit", p_OutfitVariationIndex);

    m_SetHumanoidOutfit.m_entityRef.SignalInputPin("Do");
}

void Cheats::LoadGadgets() {
    if (!m_KntLoadoutCollectionEntity) {
        return;
    }

    for (const auto& s_GadgetResourcePtr : m_KntLoadoutCollectionEntity->m_gadgets) {
        SEntityResource* s_GadgetLoaderEntityResource = static_cast<SEntityResource*>(s_GadgetResourcePtr.GetResourceData());

        ZTemplateEntityFactory* s_TemplateEntityFactory =
            static_cast<ZTemplateEntityFactory*>(s_GadgetLoaderEntityResource->factoryResource.GetResourceData());

        constexpr uint32_t s_ItemResourcePropertyID = Hash::Crc32("m_itemResource");
        constexpr uint32_t s_ItemDefinitionPropertyID = Hash::Crc32("m_itemDefinition");

        GadgetInfo& s_GadgetInfo = m_Gadgets.emplace_back();

        for (const auto& s_ResourceIDProperty : s_TemplateEntityFactory->m_resourceIDPropertyValues) {
            if (s_ResourceIDProperty.propertyID == s_ItemDefinitionPropertyID) {
                ZRuntimeResourceID s_ItemDefinition = *s_ResourceIDProperty.value.As<ZRuntimeResourceID>();

                TResourcePtr<ZEntityRef> s_ResourcePtr;
                SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_ItemDefinition);

                SEntityResource* s_EntityResource = static_cast<SEntityResource*>(s_ResourcePtr.GetResourceData());

                ZGadgetItemDefinition* s_GadgetItemDefinition = s_EntityResource->entityRef.QueryInterface<ZGadgetItemDefinition>();
                ZTextLine* s_TextLine = s_GadgetItemDefinition->m_itemDisplayNameSweet.GetResource();

                s_GadgetInfo.m_Name = s_TextLine->GetText().c_str();
                s_GadgetInfo.m_GadgetItemDefinition = s_ItemDefinition;
            }
            else if (s_ResourceIDProperty.propertyID == s_ItemResourcePropertyID) {
                s_GadgetInfo.m_ItemTemplate = *s_ResourceIDProperty.value.As<ZRuntimeResourceID>();
            }
        }
    }
}

void Cheats::SpawnGadget(
    const ZRuntimeResourceID& p_ItemDefinition, const ZRuntimeResourceID& p_ItemResource, Gameplay::EGadgetActivationSlot p_Slot
) {
    if (!EnsureEntitiesSpawned()) {
        return;
    }

    m_GadgetSpawnerItemEntry.m_entityRef.SetProperty("m_itemTemplate", p_ItemResource);

    auto s_LocalPlayer = SDK()->Globals()->LocalPlayerData->m_pCharacterImpl->m_pCharacter;

    if (!s_LocalPlayer) {
        return;
    }

    m_GadgetSpawnerItemEntry.m_entityRef.SetProperty("m_mTransform", s_LocalPlayer.m_pInterfaceRef->GetObjectToWorldMatrix().ToMatrix43());

    TArray<TInterfaceRef<ZDynamicGameplaySpawnerEntryEntity>> s_Entries;
    s_Entries.push_back(TInterfaceRef<ZDynamicGameplaySpawnerEntryEntity>::FromEntityRef(m_GadgetSpawnerItemEntry.m_entityRef));

    m_GadgetSpawner.m_entityRef.SetProperty("m_entries", s_Entries);

    ZEntityRef s_ExposedEntity = m_GadgetSpawnerItemEntry.m_entityRef.GetExposedEntity("3765481439");

    m_GadgetAttacher.m_entityRef.SetProperty("m_item", TInterfaceRef<ITEntityRefValue<ZItemCharacterEntityBase>>::FromEntityRef(s_ExposedEntity));

    TResourcePtr<ZEntityRef> s_ResourcePtr;
    SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, p_ItemDefinition);

    m_GadgetSlotAssigner.m_entityRef.SetProperty("m_itemDefinition", s_ResourcePtr);
    m_GadgetSlotAssigner.m_entityRef.SetProperty(
        "m_gadgetItem", TInterfaceRef<ITEntityRefValue<ZItemCharacterEntityBase>>::FromEntityRef(s_ExposedEntity)
    );
    m_GadgetSlotAssigner.m_entityRef.SetProperty("m_slot", p_Slot);

    m_GadgetSpawner.m_entityRef.SignalInputPin("Spawn");

    m_AssignGadgetToSlot = true;
}

DEFINE_PLUGIN_DETOUR(
    Cheats, ZKntLoadoutCollectionEntity*, ZKntLoadoutCollectionEntity_ZKntLoadoutCollectionEntity, ZKntLoadoutCollectionEntity* th, bool unk
) {
    m_KntLoadoutCollectionEntity = th;

    return {HookAction::Continue()};
}

DEFINE_ZKNT_PLUGIN(Cheats)
