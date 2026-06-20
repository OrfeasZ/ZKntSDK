#pragma once

#include <IPluginInterface.hpp>
#include <Glacier/ZCamera.hpp>
#include <Glacier/ZValue.hpp>
#include <Glacier/ZPlayer.hpp>

class Cheats : public zknt::IPluginInterface {
  public:
    Cheats();
    ~Cheats();

    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;

  private:
    void CleanupSpawnedEntities();
    bool EnsureEntitiesSpawned();
    void ApplyPlayerModifiers();
    bool AnyCheatEnabled() const;

    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    bool m_FrameUpdateRegistered = false;
    bool m_NoclipEnabled = false;
    bool m_NoclipWasEnabled = false;
    SMatrix m_PlayerPosition = {};

    // Cheats helpers.
    TEntityRef<ZCLTeleportHumanoidEntity> m_Teleporter;
    TEntityRef<ZSpatialEntity> m_TeleportTarget;
    TEntityRef<ZCLGetLocalPlayerHumanoidCharacter> m_LocalPlayerHumanoidGetter;
    TEntityRef<ZCLEnableDisableHumanoidCollision> m_CollisionModifier;
    TEntityRef<ZCLSetHumanoidImmuneToDamage> m_ImmuneModifier;
    TEntityRef<ZCLSetHumanoidUnkillableByDamage> m_UnkillableModifier;
    TEntityRef<ZCLSetHumanoidInfiniteClipAmmo> m_InfiniteAmmoModifier;
    TEntityRef<ZCLValueBoolEntity> m_ImmuneBoolValue;
    TEntityRef<ZCLValueBoolEntity> m_UnkillableBoolValue;

    bool m_ShowPanel = false;
    bool m_DisableCollision = false;
    bool m_GodMode = false;
    bool m_Unkillable = false;
    bool m_InfiniteAmmo = false;
    bool m_StateDirty = false;

    ZInputAction m_ToggleNoclipAction;
    ZInputAction m_ForwardAction;
    ZInputAction m_BackwardAction;
    ZInputAction m_LeftAction;
    ZInputAction m_RightAction;
    ZInputAction m_FastAction;
};

DECLARE_ZKNT_PLUGIN(Cheats)
