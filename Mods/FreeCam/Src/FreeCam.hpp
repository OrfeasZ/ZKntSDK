#pragma once

#include <IPluginInterface.hpp>
#include <Glacier/ZCamera.hpp>

class ZCLBlockHumanoidPlayerMoveInput : public ZEntityImpl {};

class ZCLUnblockHumanoidPlayerMoveInput : public ZEntityImpl {};

class ZCLGetLocalPlayerID : public ZEntityImpl {};

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

    bool HasSpawnedEntities() const;
    void CleanupSpawnedEntities();

    DECLARE_PLUGIN_DETOUR(
        FreeCam, ZString*, ZFreeCameraControlEntity_GenerateActionBindingString, ZFreeCameraControlEntity* p_Th, ZString& p_Result, int p_ControllerId
    );
    DECLARE_PLUGIN_DETOUR(FreeCam, void, ZFreeCameraControlEntity_UpdateCamera, ZFreeCameraControlEntity* p_Th, float p_Dt);

    bool m_FreeCamActive;
    bool m_ShouldToggle;

    ZInputAction m_ToggleFreeCamAction;

    bool m_MenuVisible;
    bool m_ControlsVisible;
    std::unordered_map<std::string, std::string> m_PcControls;
    std::unordered_map<std::string, std::string> m_PcControlsEditorStyle;
    std::unordered_map<std::string, std::string> m_ControllerControls;

    TEntityRef<ZCameraEntity> m_FreeCamera;
    TEntityRef<ZFreeCameraControlEntity> m_FreeCameraControl;
    TEntityRef<ZCLBlockHumanoidPlayerMoveInput> m_BlockMove;
    TEntityRef<ZCLUnblockHumanoidPlayerMoveInput> m_UnblockMove;
    TEntityRef<ZCLGetLocalPlayerID> m_GetLocalPlayer;
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
