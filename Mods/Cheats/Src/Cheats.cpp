#include "Cheats.hpp"
#include <Glacier/ZPlayer.hpp>
#include <Glacier/ZGameLoopManager.hpp>
#include <Glacier/ZMath.hpp>
#include <Logging.hpp>

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
    const auto s_Delete = [](auto& p_Ref) {
        if (p_Ref) {
            SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, p_Ref.m_entityRef);
        }
        p_Ref = {};
    };

    s_Delete(m_Teleporter);
    s_Delete(m_TeleportTarget);
    s_Delete(m_LocalPlayerHumanoidGetter);
    s_Delete(m_CollisionModifier);
    s_Delete(m_ImmuneModifier);
    s_Delete(m_UnkillableModifier);
    s_Delete(m_InfiniteAmmoModifier);
    s_Delete(m_ImmuneBoolValue);
    s_Delete(m_UnkillableBoolValue);
}

void Cheats::OnEngineInitialized() {
    const ZMemberDelegate<Cheats, void(const SGameUpdateEvent&)> s_Delegate(this, &Cheats::OnFrameUpdate);
    SDK()->Globals()->GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
    m_FrameUpdateRegistered = true;

    const char* binds = "NoclipInput={"
                        "ToggleNoclip=& hold(kb,lctrl) tap(kb,n);"
                        "Forward=hold(kb,w);"
                        "Backward=hold(kb,s);"
                        "Left=hold(kb,a);"
                        "Right=hold(kb,d);"
                        "Fast=hold(kb,lshift) | hold(kb,rshift);};";

    ZInputContext* s_InputContext = SDK()->Functions()->GetGlobalInputContext->Call();

    SDK()->Functions()->AddBindings->Call(binds, s_InputContext);
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
    m_ImmuneBoolValue = TEntityRef<ZCLValueBoolEntity>::SpawnEntity(ResId<"[modules:/zclvalueboolentity.class].entitytype">);
    m_UnkillableBoolValue = TEntityRef<ZCLValueBoolEntity>::SpawnEntity(ResId<"[modules:/zclvalueboolentity.class].entitytype">);

    if (!m_Teleporter || !m_TeleportTarget || !m_LocalPlayerHumanoidGetter || !m_CollisionModifier || !m_ImmuneModifier || !m_UnkillableModifier
        || !m_InfiniteAmmoModifier || !m_ImmuneBoolValue || !m_UnkillableBoolValue) {
        Logger::Error("Failed to spawn one or more player modifier entities.");
        CleanupSpawnedEntities();
        return false;
    }

    const auto s_HumanoidRef = TInterfaceRef<ITEntityRefValue<ZHumanoidCharacterEntity>>::FromEntityRef(m_LocalPlayerHumanoidGetter.m_entityRef);

    if (!s_HumanoidRef) {
        Logger::Error("Failed to get ITEntityRefValue for player.");
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

    // Wire the immune/unkillable modifiers to their bool-value sources.
    const auto s_ImmuneBoolRef = TInterfaceRef<IBoolValue>::FromEntityRef(m_ImmuneBoolValue.m_entityRef);
    const auto s_UnkillableBoolRef = TInterfaceRef<IBoolValue>::FromEntityRef(m_UnkillableBoolValue.m_entityRef);

    if (!s_ImmuneBoolRef || !s_UnkillableBoolRef) {
        Logger::Error("Failed to get IBoolValue ref for damage modifiers.");
        CleanupSpawnedEntities();
        return false;
    }

    m_ImmuneModifier.m_entityRef.SetProperty("m_invulnerable", s_ImmuneBoolRef);
    m_UnkillableModifier.m_entityRef.SetProperty("m_isUnkillable", s_UnkillableBoolRef);

    // Make sure the freshly spawned entities pick up the current toggle states.
    m_StateDirty = true;
    return true;
}

void Cheats::ApplyPlayerModifiers() {
    m_ImmuneBoolValue.m_entityRef.SetProperty("m_bValue", m_GodMode);
    m_ImmuneModifier.m_entityRef.SignalInputPin("Do");

    m_UnkillableBoolValue.m_entityRef.SetProperty("m_bValue", m_Unkillable);
    m_UnkillableModifier.m_entityRef.SignalInputPin("Do");

    m_InfiniteAmmoModifier.m_entityRef.SetProperty("m_infiniteAmmo", m_InfiniteAmmo);
    m_InfiniteAmmoModifier.m_entityRef.SignalInputPin("Do");

    // Collision is forced off while noclip is active.
    const bool s_CollisionEnabled = !(m_NoclipEnabled || m_DisableCollision);
    m_CollisionModifier.m_entityRef.SetProperty("m_collisionEnabled", s_CollisionEnabled);
    m_CollisionModifier.m_entityRef.SignalInputPin("Do");
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

    if (!EnsureEntitiesSpawned()) {
        return;
    }

    if (m_StateDirty) {
        ApplyPlayerModifiers();
        m_StateDirty = false;
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

void Cheats::OnDrawMenu() {
    if (ImGui::Button("CHEATS")) {
        m_ShowPanel = !m_ShowPanel;
    }
}

void Cheats::OnDrawUI(bool p_HasFocus) {
    if (!m_ShowPanel || !p_HasFocus) {
        return;
    }

    if (ImGui::Begin("Cheats", &m_ShowPanel, ImGuiWindowFlags_AlwaysAutoResize)) {
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
    }

    ImGui::End();
}

DEFINE_ZKNT_PLUGIN(Cheats)
