#pragma once

#include "IComponentInterface.hpp"
#include "ZEntity.hpp"
#include "ZRenderableEntity.hpp"
#include "ZRender.hpp"
#include "ZInput.h"

class IRenderDestinationEntity;

class IReflectSource {
  public:
    virtual ~IReflectSource() = 0;
};

class ICameraManager : public IComponentInterface {
  public:
    virtual TEntityRef<IRenderDestinationEntity>* GetActiveRenderDestinationEntity(TEntityRef<IRenderDestinationEntity>& result) = 0;
    virtual void ICameraManager_Unk6() = 0;
    virtual void ICameraManager_Unk7() = 0;
    virtual void ICameraManager_Unk8() = 0;
    virtual TEntityRef<IRenderDestinationEntity>* GetActiveRenderDestinationEntity2(TEntityRef<IRenderDestinationEntity>& result) = 0;
};

class ZCameraManagerMain : public ICameraManager, public IReflectSource {};

class ICameraEntity : public IRenderDestinationSource {
  public:
    virtual void ICameraEntity_unk5() = 0;
    virtual void ICameraEntity_unk6() = 0;
    virtual void ICameraEntity_unk7() = 0;
    virtual void ICameraEntity_unk8() = 0;
    virtual void ICameraEntity_unk9() = 0;
    virtual void SetFovYDeg(float fFovYDeg) = 0;
};

class ZCameraEntity : public ZRenderableEntity, public ICameraEntity {};

class IFreeCameraControl {
  public:
    virtual void ICameraManager_Unk0() = 0;
    virtual bool SetActive(bool bActive) = 0;
    virtual void ICameraManager_Unk2() = 0;
    virtual void SetCameraEntity(const TEntityRef<ZCameraEntity>& cameraEntity) = 0;
    virtual void ICameraManager_Unk4() = 0;
};

class ZFreeCameraControlEntity : public ZEntityImpl, public IFreeCameraControl {
  public:
    bool ForceGetDigital(ZInputAction& inputAction) {
        return inputAction.Digital() || inputAction.Analog() > 0.60000002;
    }

    float GetDigitalAxis(ZInputAction& positiveAction, ZInputAction& negativeAction) {
        const float positive = SDK()->Functions()->ZInputAction_Digital->Call(&positiveAction) ? 1.0f : 0.0f;
        const float negative = SDK()->Functions()->ZInputAction_Digital->Call(&negativeAction) ? 1.0f : 0.0f;

        return positive - negative;
    }

    float GetCombinedAxis(ZInputAction& positiveAction, ZInputAction& negativeAction, float analogValue) {
        return std::clamp(analogValue + GetDigitalAxis(positiveAction, negativeAction), -1.0f, 1.0f);
    }

    TEntityRef<ZCameraEntity> m_cameraEntity; // 0x20
    bool m_bActive;                           // 0x38
    bool m_bIsGameControlActive;              // 0x39
    ZCameraEntity* m_pControlledCameraEntity; // 0x40
    float m_fMoveSpeed;                       // 0x48
    float m_fTurnSpeed;                       // 0x4C
    float m_fFovDependentSpeedMultiplier;     // 0x50
    float m_fTemporaryTranslationSpeedMultiplier;
    float m_fPersistentTranslationSpeedMultiplier; // 0x58
    float m_fDeltaTranslationSpeed;
    float m_fTemporaryRotationSpeedMultiplier;
    float m_fPersistentRotationSpeedMultiplier; // 0x64
    float m_fDeltaRotationSpeed;
    float m_fMoveX;
    float m_fMoveY;
    float m_fMoveZ;
    bool m_bMoveInWorldSpace;
    bool m_bResetRoll;
    float m_fPitch;
    float m_fDeltaPitch;
    float m_fDeltaPitchRaw;
    float m_fRoll;
    float m_fDeltaRoll;
    float m_fDeltaYaw;
    float m_fDeltaYawRaw;
    float m_fInitialFov;
    float m_fFov;
    float m_fDeltaFov;
    PAD(0x8);
    int32_t m_nControllerId; // 0xAC
    bool m_bIsTiltTurnCamFixedDegreeModifierActive;
    int32_t m_nFixedRotationDegree; // 0xB4

    inline static ZInputAction m_ActivateGameControl = ZInputAction("ActivateGameControl");

    inline static ZInputAction m_AnalogMoveXAxis = ZInputAction("AnalogMoveXAxis");
    inline static ZInputAction m_AnalogMoveYAxis = ZInputAction("AnalogMoveYAxis");

    inline static ZInputAction m_AnalogCamXAxis = ZInputAction("AnalogCamXAxis");
    inline static ZInputAction m_AnalogCamYAxis = ZInputAction("AnalogCamYAxis");

    inline static ZInputAction m_TiltCamera = ZInputAction("TiltCamera");
    inline static ZInputAction m_TurnCamera = ZInputAction("TurnCamera");

    inline static ZInputAction m_MoveXPositive = ZInputAction("MoveXPositive");
    inline static ZInputAction m_MoveXNegative = ZInputAction("MoveXNegative");
    inline static ZInputAction m_MoveYPositive = ZInputAction("MoveYPositive");
    inline static ZInputAction m_MoveYNegative = ZInputAction("MoveYNegative");
    inline static ZInputAction m_MoveZPositive = ZInputAction("MoveZPositive");
    inline static ZInputAction m_MoveZNegative = ZInputAction("MoveZNegative");

    inline static ZInputAction m_MoveInWorldSpace = ZInputAction("MoveInWorldSpace");
    inline static ZInputAction m_MoveInWorldSpaceTrigger = ZInputAction("MoveInWorldSpaceTrigger");
    inline static ZInputAction m_MoveInWorldSpaceButton = ZInputAction("MoveInWorldSpaceButton");
    inline static ZInputAction m_MoveInWorldSpaceXAxis = ZInputAction("MoveInWorldSpaceXAxis");
    inline static ZInputAction m_MoveInWorldSpaceYAxis = ZInputAction("MoveInWorldSpaceYAxis");
    inline static ZInputAction m_MoveInWorldSpaceZAxis = ZInputAction("MoveInWorldSpaceZAxis");
    inline static ZInputAction m_MoveInZDirection = ZInputAction("MoveInZDirection");

    inline static ZInputAction m_RollModifier = ZInputAction("RollModifier");
    inline static ZInputAction m_RollModifier2 = ZInputAction("RollModifier2");
    inline static ZInputAction m_RollAxis = ZInputAction("RollAxis");
    inline static ZInputAction m_ResetRoll = ZInputAction("ResetRoll");
    inline static ZInputAction m_ResetRoll2 = ZInputAction("ResetRoll2");

    inline static ZInputAction m_FovModifier = ZInputAction("FovModifier");
    inline static ZInputAction m_FovModifier2 = ZInputAction("FovModifier2");
    inline static ZInputAction m_FovAxis = ZInputAction("FovAxis");
    inline static ZInputAction m_ResetFov = ZInputAction("ResetFov");
    inline static ZInputAction m_ResetFov2 = ZInputAction("ResetFov2");

    inline static ZInputAction m_SpeedTranslationAxis = ZInputAction("SpeedTranslationAxis");
    inline static ZInputAction m_SpeedRotationAxis = ZInputAction("SpeedRotationAxis");
    inline static ZInputAction m_SpeedModifier = ZInputAction("SpeedModifier");
    inline static ZInputAction m_SpeedModifier2 = ZInputAction("SpeedModifier2");
    inline static ZInputAction m_ResetSpeed = ZInputAction("ResetSpeed");
    inline static ZInputAction m_ResetSpeed2 = ZInputAction("ResetSpeed2");

    inline static ZInputAction m_TiltTurnCameraFixedDegreeModifier = ZInputAction("TiltTurnCameraFixedDegreeModifier");

    inline static ZInputAction m_TemporaryCamSpeedMultiplierTrigger = ZInputAction("TemporaryCamSpeedMultiplierTrigger");
    inline static ZInputAction m_TemporaryCamSpeedMultiplierTrigger2 = ZInputAction("TemporaryCamSpeedMultiplierTrigger2");
    inline static ZInputAction m_TemporaryCamSpeedMultiplierLeftShift = ZInputAction("TemporaryCamSpeedMultiplierLeftShift");
    inline static ZInputAction m_TemporaryCamSpeedMultiplierRightShift = ZInputAction("TemporaryCamSpeedMultiplierRightShift");
};

class ZFreeCameraControlEditorStyleEntity : public ZEntityImpl, public IFreeCameraControl {};
