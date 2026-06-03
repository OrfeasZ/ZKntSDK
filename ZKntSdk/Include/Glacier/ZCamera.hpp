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

class ICameraEntity : public IRenderDestinationSource {};

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

    inline static ZInputAction m_aActivateGameControl[2] = {ZInputAction("ActivateGameControl0"), ZInputAction("ActivateGameControl1")};

    inline static ZInputAction m_aAnalogMoveXAxis[2] = {ZInputAction("AnalogMoveXAxis0"), ZInputAction("AnalogMoveXAxis1")};
    inline static ZInputAction m_aAnalogMoveYAxis[2] = {ZInputAction("AnalogMoveYAxis0"), ZInputAction("AnalogMoveYAxis1")};

    inline static ZInputAction m_aAnalogCamXAxis[2] = {ZInputAction("AnalogCamXAxis0"), ZInputAction("AnalogCamXAxis1")};
    inline static ZInputAction m_aAnalogCamYAxis[2] = {ZInputAction("AnalogCamYAxis0"), ZInputAction("AnalogCamYAxis1")};

    inline static ZInputAction m_TiltCamera = ZInputAction("TiltCamera");
    inline static ZInputAction m_TurnCamera = ZInputAction("TurnCamera");

    inline static ZInputAction m_MoveXPositive = ZInputAction("MoveXPositive");
    inline static ZInputAction m_MoveXNegative = ZInputAction("MoveXNegative");
    inline static ZInputAction m_MoveYPositive = ZInputAction("MoveYPositive");
    inline static ZInputAction m_MoveYNegative = ZInputAction("MoveYNegative");
    inline static ZInputAction m_MoveZPositive = ZInputAction("MoveZPositive");
    inline static ZInputAction m_MoveZNegative = ZInputAction("MoveZNegative");

    inline static ZInputAction m_MoveInWorldSpace = ZInputAction("MoveInWorldSpace");
    inline static ZInputAction m_aMoveInWorldSpaceTrigger[2] = {ZInputAction("MoveInWorldSpaceTrigger0"), ZInputAction("MoveInWorldSpaceTrigger1")};
    inline static ZInputAction m_aMoveInWorldSpaceButton[2] = {ZInputAction("MoveInWorldSpaceButton0"), ZInputAction("MoveInWorldSpaceButton1")};
    inline static ZInputAction m_aMoveInWorldSpaceX[2] = {ZInputAction("MoveInWorldSpaceXAxis0"), ZInputAction("MoveInWorldSpaceXAxis1")};
    inline static ZInputAction m_aMoveInWorldSpaceY[2] = {ZInputAction("MoveInWorldSpaceYAxis0"), ZInputAction("MoveInWorldSpaceYAxis1")};
    inline static ZInputAction m_aMoveInWorldSpaceZ[2] = {ZInputAction("MoveInWorldSpaceZAxis0"), ZInputAction("MoveInWorldSpaceZAxis1")};
    inline static ZInputAction m_aMoveInZDirection[2] = {ZInputAction("MoveInZDirection0"), ZInputAction("MoveInZDirection1")};

    inline static ZInputAction m_RollModifier = ZInputAction("RollModifier");
    inline static ZInputAction m_aRollModifier[2] = {ZInputAction("RollModifier0"), ZInputAction("RollModifier1")};
    inline static ZInputAction m_aRollAxis[2] = {ZInputAction("RollAxis0"), ZInputAction("RollAxis1")};
    inline static ZInputAction m_ResetRoll = ZInputAction("ResetRoll");
    inline static ZInputAction m_aResetRoll[2] = {ZInputAction("ResetRoll0"), ZInputAction("ResetRoll1")};

    inline static ZInputAction m_FovModifier = ZInputAction("FovModifier");
    inline static ZInputAction m_aFovModifier[2] = {ZInputAction("FovModifier0"), ZInputAction("FovModifier1")};
    inline static ZInputAction m_aFovAxis[2] = {ZInputAction("FovAxis0"), ZInputAction("FovAxis1")};
    inline static ZInputAction m_ResetFov = ZInputAction("ResetFov");
    inline static ZInputAction m_aResetFov[2] = {ZInputAction("ResetFov0"), ZInputAction("ResetFov1")};

    inline static ZInputAction m_aSpeedTranslationAxis[2] = {ZInputAction("SpeedTranslationAxis0"), ZInputAction("SpeedTranslationAxis1")};
    inline static ZInputAction m_aSpeedRotationAxis[2] = {ZInputAction("SpeedRotationAxis0"), ZInputAction("SpeedRotationAxis1")};
    inline static ZInputAction m_SpeedModifier = ZInputAction("SpeedModifier");
    inline static ZInputAction m_aSpeedModifier[2] = {ZInputAction("SpeedModifier0"), ZInputAction("SpeedModifier1")};
    inline static ZInputAction m_ResetSpeed = ZInputAction("ResetSpeed");
    inline static ZInputAction m_aResetSpeed[2] = {ZInputAction("ResetSpeed0"), ZInputAction("ResetSpeed1")};

    inline static ZInputAction m_TiltTurnCameraFixedDegreeModifier = ZInputAction("TiltTurnCameraFixedDegreeModifier");

    inline static ZInputAction m_aTemporaryCamSpeedMultiplierTrigger[2] = {
        ZInputAction("TemporaryCamSpeedMultiplierTrigger0"), ZInputAction("TemporaryCamSpeedMultiplierTrigger1")
    };
    inline static ZInputAction m_aTemporaryCamSpeedMultiplierTrigger2[2] = {
        ZInputAction("TemporaryCamSpeedMultiplierTrigger20"), ZInputAction("TemporaryCamSpeedMultiplierTrigger21")
    };
    inline static ZInputAction m_TemporaryCamSpeedMultiplierLeftShift = ZInputAction("TemporaryCamSpeedMultiplierLeftShift");
    inline static ZInputAction m_TemporaryCamSpeedMultiplierRightShift = ZInputAction("TemporaryCamSpeedMultiplierRightShift");
};

class ZFreeCameraControlEditorStyleEntity : public ZEntityImpl, public IFreeCameraControl {};
