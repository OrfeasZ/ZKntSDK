#include "FreeCam.hpp"

#include <Logging.hpp>

#include <Glacier/CompileReflection.hpp>
#include <Glacier/ZGameLoopManager.hpp>
#include <Glacier/ZString.hpp>

#include <algorithm>
#include <cmath>

class IEntityFactory : public IComponentInterface {};

bool FreeCam::HasSpawnedEntities() const {
    return m_FreeCamera.m_pInterfaceRef != nullptr && m_FreeCameraControl.m_pInterfaceRef != nullptr;
}

void FreeCam::CleanupSpawnedEntities() {
    if (m_PreviousCameraSource) {
        TEntityRef<IRenderDestinationEntity> s_RenderDest;
        SDK()->Globals()->CameraManagerMain->GetActiveRenderDestinationEntity(s_RenderDest);

        if (s_RenderDest.m_pInterfaceRef) {
            s_RenderDest.m_pInterfaceRef->SetSource(m_PreviousCameraSource);
        }
    }

    if (m_FreeCameraControl.m_pInterfaceRef) {
        m_FreeCameraControl.m_pInterfaceRef->SetActive(false);
    }

    if (m_FreeCameraControl.m_entityRef) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_FreeCameraControl.m_entityRef);
    }

    if (m_FreeCamera.m_entityRef) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_FreeCamera.m_entityRef);
    }

    m_FreeCameraControl = {};
    m_FreeCamera = {};
    m_PreviousCameraSource = {};
    m_Initialized = false;
}

FreeCam::~FreeCam() {
    if (!m_FrameUpdateRegistered) {
        CleanupSpawnedEntities();
        return;
    }

    const ZMemberDelegate<FreeCam, void(const SGameUpdateEvent&)> s_Delegate(this, &FreeCam::OnFrameUpdate);
    SDK()->Globals()->GameLoopManager->UnregisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
    m_FrameUpdateRegistered = false;
    CleanupSpawnedEntities();
}

void FreeCam::Init() {
    SDK()->Hooks()->ZFreeCameraControlEntity_GenerateActionBindingString->AddDetour(
        this, &FreeCam::ZFreeCameraControlEntity_GenerateActionBindingString
    );
    SDK()->Hooks()->ZFreeCameraControlEntity_UpdateCamera->AddDetour(this, &FreeCam::ZFreeCameraControlEntity_UpdateCamera);
}

void FreeCam::OnEngineInitialized() {
    if (m_FrameUpdateRegistered) {
        return;
    }

    const ZMemberDelegate<FreeCam, void(const SGameUpdateEvent&)> s_Delegate(this, &FreeCam::OnFrameUpdate);
    SDK()->Globals()->GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
    m_FrameUpdateRegistered = true;
}

DEFINE_PLUGIN_DETOUR(
    FreeCam, ZString*, ZFreeCameraControlEntity_GenerateActionBindingString, ZFreeCameraControlEntity* p_Th, ZString& p_Result, int p_ControllerId
) {
    ZString* s_Res = p_Hook->CallOriginal(p_Th, p_Result, p_ControllerId);

    p_Result.m_pChars = "FreeCamControl0={"
                        "TiltCamera=rel(ms,y);"
                        "TurnCamera=rel(ms,x);"
                        "MoveXPositive=| hold(kb,left) hold(kb,a);"
                        "MoveXNegative=| hold(kb,right) hold(kb,d);"
                        "MoveYPositive=| hold(kb,up) hold(kb,w);"
                        "MoveYNegative=| hold(kb,down) hold(kb,s);"
                        "MoveZPositive=| hold(kb,pgup) hold(kb,e);"
                        "MoveZNegative=| hold(kb,pgdn) hold(kb,q);"
                        "TiltTurnCameraFixedDegreeModifier=hold(kb,f);"
                        "RollModifier=| hold(kb,lctrl) hold(kb,rctrl);"
                        "FovModifier=| hold(kb,lctrl) hold(kb,rctrl);"
                        "SpeedModifier=| hold(kb,lalt) hold(kb,ralt);"
                        "MoveInWorldSpace=hold(kb,space);"
                        "ResetRoll=hold(kb,x);"
                        "ResetFov=hold(kb,z);"
                        "ResetSpeed=hold(kb,z);"
                        "AnalogCamXAxis0=ana(gc0,rightx);"
                        "AnalogCamYAxis0=ana(gc0,righty);"
                        "AnalogMoveXAxis0=ana(gc0,leftx);"
                        "AnalogMoveYAxis0=ana(gc0,lefty);"
                        "MoveInZDirection0=| hold(gc0,right_bumper) hold(gc0,right1);"
                        "RollModifier0=| hold(gc0,a) hold(gc0,cross);"
                        "RollAxis0=ana(gc0,leftx);"
                        "ResetRoll0=| hold(gc0,leftstick) hold(gc0,left_thumb);"
                        "FovModifier0=| hold(gc0,y) hold(gc0,triangle);"
                        "FovAxis0=ana(gc0,lefty);"
                        "ResetFov0=| hold(gc0,leftstick) hold(gc0,left_thumb);"
                        "SpeedModifier0=| hold(gc0,b) hold(gc0,circle);"
                        "SpeedTranslationAxis0=ana(gc0,lefty);"
                        "SpeedRotationAxis0=ana(gc0,leftx);"
                        "ResetSpeed0=| hold(gc0,leftstick) hold(gc0,left_thumb);"
                        "MoveInWorldSpaceTrigger0=ana(gc0,left_analog_trigger);"
                        "MoveInWorldSpaceButton0=hold(gc0,left2);"
                        "MoveInWorldSpaceXAxis0=ana(gc0,leftx);"
                        "MoveInWorldSpaceYAxis0=ana(gc0,lefty);"
                        "MoveInWorldSpaceZAxis0=ana(gc0,righty);"
                        "ActivateGameControl0=| hold(gc0,left_bumper) hold(gc0,left1);"
                        "TemporaryCamSpeedMultiplierTrigger0=ana(gc0,right_analog_trigger);"
                        "TemporaryCamSpeedMultiplierTrigger20=ana(gc0,right2_analog);"
                        "TemporaryCamSpeedMultiplierLeftShift=hold(kb,lshift);"
                        "TemporaryCamSpeedMultiplierRightShift=hold(kb,rshift);"
                        "};";

    p_Result.m_nLength = static_cast<uint32_t>(std::strlen(p_Result.m_pChars)) | 0x80000000;

    return {HookAction::Return(), s_Res};
}

DEFINE_PLUGIN_DETOUR(FreeCam, void, ZFreeCameraControlEntity_UpdateCamera, ZFreeCameraControlEntity* p_Th, float p_Dt) {
    p_Th->m_fMoveX = 0.0f;
    p_Th->m_fMoveY = 0.0f;
    p_Th->m_fMoveZ = 0.0f;
    p_Th->m_fDeltaPitch = 0.0f;
    p_Th->m_fDeltaYaw = 0.0f;
    p_Th->m_fDeltaPitchRaw = 0.0f;
    p_Th->m_fDeltaYawRaw = 0.0f;
    p_Th->m_fDeltaRoll = 0.0f;
    p_Th->m_fDeltaFov = 0.0f;
    p_Th->m_fDeltaTranslationSpeed = 0.0f;
    p_Th->m_fDeltaRotationSpeed = 0.0f;

    ZCameraEntity* s_Camera = m_FreeCamera.m_pInterfaceRef;
    if (!s_Camera) {
        return {HookAction::Return()};
    }

    constexpr float c_MouseSensitivity = 0.0007f;
    constexpr float c_StickLookSpeed = 1.5f;
    constexpr float c_BaseMoveSpeed = 10.0f;
    constexpr float c_MaxPitch = 1.55334f; // bit less than 90deg

    const SVector3 s_WorldUp(0.0f, 0.0f, 1.0f);
    const float s_Dt = std::clamp(p_Dt, 0.0f, 0.1f);

    if (!m_Initialized) {
        const SMatrix s_Mat = s_Camera->GetObjectToWorldMatrix();
        SVector3 s_SeedForward(-s_Mat.ZAxis.x, -s_Mat.ZAxis.y, -s_Mat.ZAxis.z);
        s_SeedForward = s_SeedForward.Normalized();

        m_Pitch = std::asin(std::clamp(s_SeedForward.z, -1.0f, 1.0f));
        m_Yaw = std::atan2(s_SeedForward.x, s_SeedForward.y);
        m_Roll = 0.0f;
        m_Position = SVector3(s_Mat.Trans.x, s_Mat.Trans.y, s_Mat.Trans.z);
        m_TranslationSpeedMul = 1.0f;
        m_RotationSpeedMul = 1.0f;
        m_Initialized = true;
    }

    if (p_Th->ForceGetDigital(p_Th->m_aActivateGameControl[p_Th->m_nControllerId])) {
        p_Th->m_bIsGameControlActive = true;
        return {HookAction::Return()};
    }

    p_Th->m_bIsGameControlActive = false;

    const int s_Controller = p_Th->m_nControllerId;
    const float s_MouseYaw = p_Th->m_TurnCamera.Analog();
    const float s_MousePitch = p_Th->m_TiltCamera.Analog();
    const float s_StickYaw = p_Th->m_aAnalogCamXAxis[s_Controller].Analog();
    const float s_StickPitch = p_Th->m_aAnalogCamYAxis[s_Controller].Analog();

    constexpr float c_RollSpeed = 1.5f;
    constexpr float c_SpeedAdjustRate = 2.0f;
    constexpr float c_DegToRad = 0.01745329f;

    float s_MoveRight = p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, p_Th->m_aAnalogMoveXAxis[s_Controller].Analog());
    float s_MoveForward = p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_aAnalogMoveYAxis[s_Controller].Analog());
    const float s_MoveUp = p_Th->GetCombinedAxis(p_Th->m_MoveZPositive, p_Th->m_MoveZNegative, 0.0f);

    const float s_TempSpeed = std::max({
        p_Th->m_aTemporaryCamSpeedMultiplierTrigger[s_Controller].Analog(),
        p_Th->m_aTemporaryCamSpeedMultiplierTrigger2[s_Controller].Analog(),
        p_Th->m_TemporaryCamSpeedMultiplierLeftShift.Digital() ? 1.0f : 0.0f,
        p_Th->m_TemporaryCamSpeedMultiplierRightShift.Digital() ? 1.0f : 0.0f,
    });
    const float s_TempMultiplier = s_TempSpeed * 9.0f + 1.0f;

    const bool s_RollFovModifier = p_Th->m_RollModifier.Digital() || p_Th->m_FovModifier.Digital()
                                   || p_Th->ForceGetDigital(p_Th->m_aRollModifier[s_Controller])
                                   || p_Th->ForceGetDigital(p_Th->m_aFovModifier[s_Controller]);

    const bool s_SpeedModifier = p_Th->m_SpeedModifier.Digital() || p_Th->ForceGetDigital(p_Th->m_aSpeedModifier[s_Controller]);
    const bool s_FixedModifier = p_Th->m_TiltTurnCameraFixedDegreeModifier.Digital();

    const bool s_PlanarMove = p_Th->m_MoveInWorldSpace.Digital() || p_Th->m_aMoveInWorldSpaceButton[s_Controller].Digital()
                              || p_Th->m_aMoveInWorldSpaceTrigger[s_Controller].Analog() > 0.6f;

    if (s_RollFovModifier || s_SpeedModifier || s_FixedModifier) {
        s_MoveRight = 0.0f;
        s_MoveForward = 0.0f;
    }

    if (s_RollFovModifier) {
        if (p_Th->m_ResetRoll.Digital() || p_Th->ForceGetDigital(p_Th->m_aResetRoll[s_Controller])) {
            m_Roll = 0.0f;
        }
        else {
            const float s_RollInput = p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, p_Th->m_aRollAxis[s_Controller].Analog());
            m_Roll += s_RollInput * c_RollSpeed * s_Dt;
        }

        if (p_Th->m_ResetFov.Digital() || p_Th->ForceGetDigital(p_Th->m_aResetFov[s_Controller])) {
            p_Th->m_fFov = p_Th->m_fInitialFov;
            p_Th->m_fDeltaFov = 0.0f;
        }
        else {
            p_Th->m_fDeltaFov = p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_aFovAxis[s_Controller].Analog());
        }
    }

    if (s_SpeedModifier) {
        if (p_Th->m_ResetSpeed.Digital() || p_Th->ForceGetDigital(p_Th->m_aResetSpeed[s_Controller])) {
            m_TranslationSpeedMul = 1.0f;
            m_RotationSpeedMul = 1.0f;
        }
        else {
            const float s_TransAdjust =
                p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_aSpeedTranslationAxis[s_Controller].Analog());
            const float s_RotAdjust =
                p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, p_Th->m_aSpeedRotationAxis[s_Controller].Analog());
            m_TranslationSpeedMul = std::clamp(m_TranslationSpeedMul * std::exp(s_TransAdjust * c_SpeedAdjustRate * s_Dt), 0.05f, 200.0f);
            m_RotationSpeedMul = std::clamp(m_RotationSpeedMul * std::exp(s_RotAdjust * c_SpeedAdjustRate * s_Dt), 0.1f, 10.0f);
        }
    }

    if (!s_FixedModifier) {
        const float s_LookScale = m_RotationSpeedMul;
        m_Yaw += (s_MouseYaw * c_MouseSensitivity + s_StickYaw * c_StickLookSpeed * s_Dt) * s_LookScale;
        m_Pitch += (-s_MousePitch * c_MouseSensitivity - s_StickPitch * c_StickLookSpeed * s_Dt) * s_LookScale;
        p_Th->m_bIsTiltTurnCamFixedDegreeModifierActive = false;
    }
    else {
        const float s_SnapX = p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, p_Th->m_aAnalogMoveXAxis[s_Controller].Analog());
        const float s_SnapY = p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_aAnalogMoveYAxis[s_Controller].Analog());

        if (std::abs(s_SnapX) > 0.6f || std::abs(s_SnapY) > 0.6f) {
            if (!p_Th->m_bIsTiltTurnCamFixedDegreeModifierActive) {
                p_Th->m_bIsTiltTurnCamFixedDegreeModifierActive = true;

                int s_Degrees = p_Th->m_nFixedRotationDegree;
                if (s_Degrees == 0) {
                    s_Degrees = 15;
                }
                const float s_Step = static_cast<float>(s_Degrees) * c_DegToRad;

                if (std::abs(s_SnapY) > 0.6f) {
                    m_Pitch += (s_SnapY > 0.0f) ? s_Step : -s_Step;
                }
                else {
                    m_Yaw += (s_SnapX > 0.0f) ? -s_Step : s_Step;
                }
            }
        }
        else {
            p_Th->m_bIsTiltTurnCamFixedDegreeModifierActive = false;
        }
    }

    m_Pitch = std::clamp(m_Pitch, -c_MaxPitch, c_MaxPitch);

    const float s_CosYaw = std::cos(m_Yaw);
    const float s_SinYaw = std::sin(m_Yaw);
    const float s_CosPitch = std::cos(m_Pitch);
    const float s_SinPitch = std::sin(m_Pitch);

    const SVector3 s_Forward(s_SinYaw * s_CosPitch, s_CosYaw * s_CosPitch, s_SinPitch);
    const SVector3 s_Right = s_WorldUp.Cross(s_Forward).Normalized();
    const SVector3 s_Up = s_Forward.Cross(s_Right).Normalized();

    const SVector3 s_MoveForwardDir = s_PlanarMove ? SVector3(s_Forward.x, s_Forward.y, 0.0f).Normalized() : s_Forward;
    const float s_Speed = c_BaseMoveSpeed * s_TempMultiplier * m_TranslationSpeedMul * s_Dt;
    m_Position = m_Position + s_Right * (s_MoveRight * s_Speed) + s_MoveForwardDir * (s_MoveForward * s_Speed) + s_WorldUp * (s_MoveUp * s_Speed);

    const float s_CosRoll = std::cos(m_Roll);
    const float s_SinRoll = std::sin(m_Roll);
    const SVector3 s_VisualRight = -s_Right;
    const SVector3 s_RolledRight = s_VisualRight * s_CosRoll + s_Up * s_SinRoll;
    const SVector3 s_RolledUp = s_Up * s_CosRoll - s_VisualRight * s_SinRoll;

    SMatrix s_Out;
    s_Out.XAxis = float4(s_RolledRight, 0.0f);
    s_Out.YAxis = float4(s_RolledUp, 0.0f);
    s_Out.ZAxis = float4(-s_Forward, 0.0f);
    s_Out.Trans = float4(m_Position, 1.0f);
    s_Camera->SetObjectToWorldMatrixFromEditor(s_Out);

    // Overwrite transform manually because for some reason calling just
    // SetObjectToWorldMatrixFromEditor results in stepping / snapping when looking around.
    s_Camera->m_mTransform = s_Out.ToMatrix43();
    s_Camera->m_bWorldTransformDirty = false;

    return {HookAction::Return()};
}

void FreeCam::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) {
    static_cast<void>(p_UpdateEvent);

    const bool s_SpawnDown = (GetAsyncKeyState('K') & 0x8000) != 0;
    if (!s_SpawnDown || m_SpawnKeyHeld) {
        m_SpawnKeyHeld = s_SpawnDown;
        return;
    }

    m_SpawnKeyHeld = true;

    if (HasSpawnedEntities()) {
        Logger::Info("Disabling existing free camera.");
        TEntityRef<IRenderDestinationEntity> s_RenderDest;
        SDK()->Globals()->CameraManagerMain->GetActiveRenderDestinationEntity(s_RenderDest);

        if (s_RenderDest.m_pInterfaceRef && m_PreviousCameraSource) {
            s_RenderDest.m_pInterfaceRef->SetSource(m_PreviousCameraSource);
        }
        else {
            Logger::Warn("No stored previous camera source while toggling off.");
        }

        CleanupSpawnedEntities();
        Logger::Info("Disabled free camera and restored previous camera source.");
        return;
    }

    CleanupSpawnedEntities();
    Logger::Info("Enabling free camera.");

    constexpr auto c_CameraEntityFactoryId = ResId<"[modules:/zcameraentity.class].entitytype">;
    TResourcePtr<IEntityFactory> s_CameraEntityFactory;
    SDK()->Globals()->ResourceManager->GetResourcePtr(s_CameraEntityFactory, c_CameraEntityFactoryId, 0);
    if (!s_CameraEntityFactory) {
        Logger::Error("Free camera entity factory is not loaded.");
        return;
    }

    constexpr auto c_FreeCameraControlFactoryId = ResId<"[modules:/zfreecameracontrolentity.class].entitytype">;
    TResourcePtr<IEntityFactory> s_FreeCameraControlFactory;
    SDK()->Globals()->ResourceManager->GetResourcePtr(s_FreeCameraControlFactory, c_FreeCameraControlFactoryId, 0);
    if (!s_FreeCameraControlFactory) {
        Logger::Error("Free camera control entity factory is not loaded.");
        return;
    }

    SEntityCreateInfo s_CameraInfo;
    SDK()->Functions()->SEntityCreateInfo_SEntityCreateInfo->Call(&s_CameraInfo, ZString(), s_CameraEntityFactory, ZEntityRef(), -1);
    SDK()->Functions()->ZEntityManager_NewEntity->Call(SDK()->Globals()->EntityManager, m_FreeCamera.m_entityRef, s_CameraInfo);
    m_FreeCamera.m_pInterfaceRef = m_FreeCamera.m_entityRef.QueryInterface<ZCameraEntity>();
    if (!m_FreeCamera.m_pInterfaceRef) {
        Logger::Error("Failed to create free camera entity.");
        return;
    }

    SEntityCreateInfo s_ControlInfo;
    SDK()->Functions()->SEntityCreateInfo_SEntityCreateInfo->Call(&s_ControlInfo, ZString(), s_FreeCameraControlFactory, ZEntityRef(), -1);
    SDK()->Functions()->ZEntityManager_NewEntity->Call(SDK()->Globals()->EntityManager, m_FreeCameraControl.m_entityRef, s_ControlInfo);
    m_FreeCameraControl.m_pInterfaceRef = m_FreeCameraControl.m_entityRef.QueryInterface<ZFreeCameraControlEntity>();
    if (!m_FreeCameraControl.m_pInterfaceRef) {
        Logger::Error("Failed to create free camera control entity.");
        return;
    }

    m_FreeCameraControl.m_pInterfaceRef->SetCameraEntity(m_FreeCamera);
    m_FreeCameraControl.m_pInterfaceRef->SetActive(true);

    TEntityRef<IRenderDestinationEntity> s_RenderDest;
    SDK()->Globals()->CameraManagerMain->GetActiveRenderDestinationEntity(s_RenderDest);
    const auto s_CurrentCamera = SDK()->Functions()->GetCurrentCamera->Call();
    if (!s_CurrentCamera || !s_RenderDest.m_pInterfaceRef) {
        Logger::Error("Failed to retrieve active camera or render destination.");
        return;
    }

    m_FreeCamera.m_pInterfaceRef->SetObjectToWorldMatrixFromEditor(s_CurrentCamera->GetObjectToWorldMatrix());
    m_PreviousCameraSource = s_RenderDest.m_pInterfaceRef->GetSource();
    s_RenderDest.m_pInterfaceRef->SetSource(m_FreeCamera.m_entityRef);
    Logger::Info("Enabled free camera!");

    m_Initialized = false;
}

DEFINE_ZKNT_PLUGIN(FreeCam)
