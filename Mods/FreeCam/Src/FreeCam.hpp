#pragma once

#include <IPluginInterface.hpp>
#include <Glacier/ZCamera.hpp>

class FreeCam : public zknt::IPluginInterface {
  public:
    ~FreeCam() override;

    void Init() override;
    void OnEngineInitialized() override;

  private:
    DECLARE_PLUGIN_DETOUR(
        FreeCam, ZString*, ZFreeCameraControlEntity_GenerateActionBindingString, ZFreeCameraControlEntity* p_Th, ZString& p_Result,
        int p_ControllerId
    );
    DECLARE_PLUGIN_DETOUR(FreeCam, void, ZFreeCameraControlEntity_UpdateCamera, ZFreeCameraControlEntity* p_Th, float p_Dt);

    bool HasSpawnedEntities() const;
    void CleanupSpawnedEntities();
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    TEntityRef<ZCameraEntity> m_FreeCamera;
    TEntityRef<ZFreeCameraControlEntity> m_FreeCameraControl;
    ZEntityRef m_PreviousCameraSource;
    bool m_FrameUpdateRegistered = false;
    bool m_SpawnKeyHeld = false;

    bool m_Initialized = false;
    float m_Yaw = 0.0f;
    float m_Pitch = 0.0f;
    float m_Roll = 0.0f;
    SVector3 m_Position;
    float m_TranslationSpeedMul = 1.0f;
    float m_RotationSpeedMul = 1.0f;
};

DECLARE_ZKNT_PLUGIN(FreeCam)
