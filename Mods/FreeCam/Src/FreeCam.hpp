#pragma once

#include <IPluginInterface.hpp>
#include <Glacier/ZCamera.hpp>
#include <Glacier/ZPlayer.hpp>

class FreeCam : public zknt::IPluginInterface {
  public:
    FreeCam();
    ~FreeCam() override;

    void Init() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;

  private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    void ToggleFreecam();
    void EnableFreecam();
    void DisableFreecam();
    void TogglePlayerInput();
    void SetFreeCamFrozen(bool p_Frozen);

    void TeleportPlayer();

    bool HasSpawnedEntities() const;
    void CleanupSpawnedEntities();

    DECLARE_PLUGIN_DETOUR(
        FreeCam, ZString*, ZFreeCameraControlEntity_GenerateActionBindingString, ZFreeCameraControlEntity* p_Th, ZString& p_Result,
        int32_t p_ControllerId
    );
    DECLARE_PLUGIN_DETOUR(FreeCam, void, ZFreeCameraControlEntity_UpdateCamera, ZFreeCameraControlEntity* p_Th, float p_Dt);
    DECLARE_PLUGIN_DETOUR(
        FreeCam, ZString*, ZFreeCameraControlEditorStyleEntity_GenerateActionBindingString, ZFreeCameraControlEditorStyleEntity* p_Th,
        ZString& p_Result, int32_t p_ControllerId
    );
    DECLARE_PLUGIN_DETOUR(
        FreeCam, void, ZFreeCameraControlEditorStyleEntity_HandleDrag, ZFreeCameraControlEditorStyleEntity* const th, bool bRotationIsActive,
        bool bObjectHookIsActive, bool bIsOrbitActive
    );
    DECLARE_PLUGIN_DETOUR(
        FreeCam, void, ZFreeCameraControlEditorStyleEntity_MoveCameraWithKey, ZFreeCameraControlEditorStyleEntity* th, float fDeltaTime
    );

    bool m_IsFreeCamActive;
    bool m_ShouldToggle;
    bool m_GamePaused;
    bool m_MoveInFreecam;
    bool m_IsPlayerInputEnabled;
    bool m_IsFreeCamFrozen;
    bool m_IsEditorStyleFreeCamEnabled;

    ZInputAction m_ToggleFreeCamAction;
    ZInputAction m_ActivatePlayerInputAction;
    ZInputAction m_TogglePauseGameAction;
    ZInputAction m_TeleportPlayerAction;

    bool m_MenuVisible;
    bool m_ControlsVisible;
    std::unordered_map<std::string, std::string> m_PcControls;
    std::unordered_map<std::string, std::string> m_PcControlsEditorStyle;
    std::unordered_map<std::string, std::string> m_ControllerControls;

    TEntityRef<ZCameraEntity> m_FreeCamera;
    TEntityRef<ZFreeCameraControlEntity> m_FreeCameraControl;
    TEntityRef<ZFreeCameraControlEditorStyleEntity> m_FreeCameraControlEditorStyle;
    TEntityRef<ZCLBlockHumanoidPlayerMoveInput> m_BlockHumanoidPlayerMoveInput;
    TEntityRef<ZCLUnblockHumanoidPlayerMoveInput> m_UnblockHumanoidPlayerMoveInput;
    TEntityRef<ZCLBlockPlayerGadgetInput> m_BlockPlayerGadgetInput;
    TEntityRef<ZCLUnblockPlayerGadgetInput> m_UnblockPlayerGadgetInput;
    TEntityRef<ZCLBlockHumanoidPlayerCloseCombatInput> m_BlockHumanoidPlayerCloseCombatInput;
    TEntityRef<ZCLUnblockHumanoidPlayerCloseCombatInput> m_UnblockHumanoidPlayerCloseCombatInput;
    TEntityRef<ZCLGetLocalPlayerID> m_GetLocalPlayer;
    TEntityRef<ZCLTeleportHumanoidEntity> m_TeleportHumanoidEntity;
    TEntityRef<ZCLGetLocalPlayerHumanoidCharacter> m_GetLocalPlayerHumanoidCharacter;
    ZEntityRef m_PreviousCameraSource;
    bool m_FrameUpdateRegistered = false;

    bool m_Initialized = false;
    float m_Yaw = 0.0f;
    float m_Pitch = 0.0f;
    float m_Roll = 0.0f;
    SVector3 m_Position;
    float m_TranslationSpeedMul = 1.0f;
    float m_RotationSpeedMul = 1.0f;
};

DECLARE_ZKNT_PLUGIN(FreeCam)
