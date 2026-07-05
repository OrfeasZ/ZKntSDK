#include "FreeCam.hpp"

#include "IconsMaterialDesign.h"

#include <Logging.hpp>

#include <Glacier/CompileReflection.hpp>
#include <Glacier/ZGameLoopManager.hpp>
#include <Glacier/ZString.hpp>
#include <Glacier/ZCollision.hpp>

#include <algorithm>
#include <cmath>

class IIntValue;

FreeCam::FreeCam()
    : m_IsFreeCamActive(false)
    , m_ShouldToggle(false)
    , m_GamePaused(false)
    , m_MoveInFreecam(false)
    , m_IsPlayerInputEnabled(false)
    , m_IsFreeCamFrozen(false)
    , m_IsEditorStyleFreeCamEnabled(false)
    , m_ToggleFreeCamAction("ToggleFreeCamera")
    , m_ActivatePlayerInputAction("ActivatePlayerInput")
    , m_ActivateGameControlAction("ActivateGameControl")
    , m_TogglePauseGameAction("TogglePauseGame")
    , m_TeleportPlayerAction("TeleportPlayer")
    , m_MenuVisible(false)
    , m_ControlsVisible(false) {
    m_PcControls = {
        {"K", "Toggle freecam"},
        {"F3", "Freeze camera and enable player input"},

        {"W / S", "Move camera forward/backward"},
        {"A / D", "Move camera left/right"},
        {"Q / E", "Move camera down/up"},
        {"Arrow Keys", "Move camera"},

        {"Mouse", "Rotate camera"},

        {"Ctrl + A/D", "Roll camera"},
        {"Ctrl + W/S", "Change FOV"},

        {"Alt + W/S", "Change camera speed"},
        {"Alt + A/D", "Change rotation speed"},

        {"Ctrl + X", "Reset roll"},
        {"Ctrl + Z", "Reset FOV"},
        {"Alt + Z", "Reset camera speed"},

        {"Space + W/A/S/D", "Move camera in world space"},
        {"Space + Q/E", "Move camera vertically in world space"},

        {"Shift", "Temporary speed boost"},
        {"F", "Fixed-degree camera rotation"},

        {"Ctrl + F6", "Teleport Hitman"},
        {"F8", "Pause/Resume game"}
    };

    m_PcControlsEditorStyle = {
        {"P", "Toggle freecam"},
        {"F3", "Freeze camera and enable player input"},
        {"MMB", "Drag camera"},
        {"Scroll Wheel", "Zoom camera"},
        {"RMB", "Activate rotate"},
        {"Alt + MMB or RMB", "Orbit camera"},
        {"Z + Alt + MMB or RMB", "Orbit camera around selected entity"},
        {"Z", "Zoom to selected entity (press twice to focus the gizmo)"},
        {"Alt + Scroll Wheel", "Zoom camera with precision"},
        {"Shift", "Speed modifier"},
        {"RMB + Scroll wheel", "Adjust speed"}
    };

    m_ControllerControls = {
        {"Right Stick", "Rotate camera"},
        {"Left Stick", "Move camera"},

        {"RB", "Move camera vertically"},
        {"RT", "Temporary speed boost"},

        {"A + Left Stick", "Roll camera"},
        {"Y + Left Stick", "Change FOV"},
        {"B + Left Stick", "Change camera speed"},

        {"Left Stick Press", "Reset roll/FOV/speed"},

        {"LT + Left Stick", "Move camera in world space"},
        {"LT + Right Stick Vertical", "Move camera vertically in world space"},

        {"LB", "Freeze camera and enable player input"}
    };
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
    SDK()->Hooks()->ZFreeCameraControlEditorStyleEntity_GenerateActionBindingString->AddDetour(
        this, &FreeCam::ZFreeCameraControlEditorStyleEntity_GenerateActionBindingString
    );
    SDK()->Hooks()->ZFreeCameraControlEditorStyleEntity_HandleDrag->AddDetour(this, &FreeCam::ZFreeCameraControlEditorStyleEntity_HandleDrag);
    SDK()->Hooks()->ZFreeCameraControlEditorStyleEntity_MoveCameraWithKey->AddDetour(
        this, &FreeCam::ZFreeCameraControlEditorStyleEntity_MoveCameraWithKey
    );
}

void FreeCam::OnEngineInitialized() {
    if (m_FrameUpdateRegistered) {
        return;
    }

    const ZMemberDelegate<FreeCam, void(const SGameUpdateEvent&)> s_Delegate(this, &FreeCam::OnFrameUpdate);
    SDK()->Globals()->GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);
    m_FrameUpdateRegistered = true;

    const char* s_Bindings = "FreeCameraInput={"
                             "ToggleFreeCamera=tap(kb,k);"
                             "TogglePauseGame=tap(kb,f8);"
                             "ActivatePlayerInput=tap(kb,f3);"
                             "TeleportPlayer=& hold(kb,lctrl) tap(kb,f6);};";

    ZInputContext* s_InputContext = SDK()->Functions()->GetGlobalInputContext->Call();

    SDK()->Functions()->AddBindings->Call(s_Bindings, s_InputContext);
}

void FreeCam::OnDrawMenu() {
    if (ImGui::Button(ICON_MD_PHOTO_CAMERA " FREECAM")) {
        m_MenuVisible = !m_MenuVisible;
    }
}

void FreeCam::OnDrawUI(bool p_HasFocus) {
    if (m_MenuVisible) {
        const auto s_Center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(s_Center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::PushFont(SDK()->GetImGuiBlackFont());
        const auto s_MenuExpanded = ImGui::Begin(ICON_MD_PHOTO_CAMERA " FreeCam", &m_MenuVisible);
        ImGui::PushFont(SDK()->GetImGuiRegularFont());

        if (s_MenuExpanded) {
            bool s_IsFreeCamActive = m_IsFreeCamActive;

            if (ImGui::Checkbox("Enable freecam", &s_IsFreeCamActive)) {
                ToggleFreecam();
            }

            ImGui::BeginDisabled(s_IsFreeCamActive);
            ImGui::Checkbox("Use editor style freecam", &m_IsEditorStyleFreeCamEnabled);
            ImGui::EndDisabled();

            bool s_IsPlayerInputEnabled = m_IsPlayerInputEnabled;

            if (ImGui::Checkbox("Enable player input", &s_IsPlayerInputEnabled)) {
                TogglePlayerInput();
            }

            if (ImGui::Checkbox("Pause game in freecam", &m_GamePaused)) {
                if (m_IsFreeCamActive) {
                    SDK()->Globals()->GameTimeManager->m_bPaused = m_GamePaused;
                }
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Whether the Hitman should move along with the camera when in freecam.");
            }

            if (ImGui::Button(ICON_MD_SPORTS_ESPORTS " Show freecam controls")) {
                m_ControlsVisible = !m_ControlsVisible;
            }
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    if (m_ControlsVisible) {
        ImGui::PushFont(SDK()->GetImGuiBlackFont());
        const auto s_ControlsExpanded = ImGui::Begin(ICON_MD_PHOTO_CAMERA " FreeCam Controls", &m_ControlsVisible);
        ImGui::PushFont(SDK()->GetImGuiRegularFont());

        if (s_ControlsExpanded) {
            ImGui::TextUnformatted("PC Controls");

            ImGui::BeginTable("FreeCamControlsPc", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit);

            if (m_IsEditorStyleFreeCamEnabled) {
                for (auto& [s_Key, s_Description] : m_PcControlsEditorStyle) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(s_Key.c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(s_Description.c_str());
                }
            }
            else {
                for (auto& [s_Key, s_Description] : m_PcControls) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(s_Key.c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(s_Description.c_str());
                }
            }

            ImGui::EndTable();

            ImGui::TextUnformatted("Controller Controls");

            ImGui::BeginTable("FreeCamControlsController", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit);

            for (auto& [s_Key, s_Description] : m_ControllerControls) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(s_Key.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(s_Description.c_str());
            }

            ImGui::EndTable();
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }
}

void FreeCam::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) {
    static_cast<void>(p_UpdateEvent);

    if (m_ToggleFreeCamAction.Digital()) {
        ToggleFreecam();
    }

    if (m_ShouldToggle) {
        m_ShouldToggle = false;

        if (m_IsFreeCamActive) {
            EnableFreecam();
        }
        else {
            DisableFreecam();
        }
    }

    if (m_IsFreeCamActive) {
        if (m_TogglePauseGameAction.Digital()) {
            m_GamePaused = !m_GamePaused;
            SDK()->Globals()->GameTimeManager->m_bPaused = m_GamePaused;
        }

        if (m_ActivatePlayerInputAction.Digital() || m_ActivateGameControlAction.Digital()) {
            TogglePlayerInput();
            SetFreeCamFrozen(m_IsPlayerInputEnabled);
        }

        if (m_TeleportPlayerAction.Digital()) {
            TeleportPlayer();
        }
    }
}

void FreeCam::ToggleFreecam() {
    m_IsFreeCamActive = !m_IsFreeCamActive;
    m_ShouldToggle = true;
}

void FreeCam::EnableFreecam() {
    CleanupSpawnedEntities();
    Logger::Info("Enabling free camera.");

    m_FreeCamera = TEntityRef<ZCameraEntity>::SpawnEntity(ResId<"[modules:/zcameraentity.class].entitytype">);
    if (!m_FreeCamera) {
        Logger::Error("Failed to create free camera entity.");
        CleanupSpawnedEntities();
        return;
    }

    if (m_IsEditorStyleFreeCamEnabled) {
        m_FreeCameraControlEditorStyle =
            TEntityRef<ZFreeCameraControlEditorStyleEntity>::SpawnEntity(ResId<"[modules:/zfreecameracontroleditorstyleentity.class].entitytype">);
        if (!m_FreeCameraControlEditorStyle) {
            Logger::Error("Failed to create free camera control editor style entity.");
            CleanupSpawnedEntities();
            return;
        }
    }
    else {
        m_FreeCameraControl = TEntityRef<ZFreeCameraControlEntity>::SpawnEntity(ResId<"[modules:/zfreecameracontrolentity.class].entitytype">);
        if (!m_FreeCameraControl) {
            Logger::Error("Failed to create free camera control entity.");
            CleanupSpawnedEntities();
            return;
        }
    }

    m_BlockHumanoidPlayerMoveInput =
        TEntityRef<ZCLBlockHumanoidPlayerMoveInput>::SpawnEntity(ResId<"[modules:/zclblockhumanoidplayermoveinput.class].entitytype">);
    if (!m_BlockHumanoidPlayerMoveInput) {
        Logger::Error("Failed to create block humanoid player move input entity.");
        CleanupSpawnedEntities();
        return;
    }

    m_UnblockHumanoidPlayerMoveInput =
        TEntityRef<ZCLUnblockHumanoidPlayerMoveInput>::SpawnEntity(ResId<"[modules:/zclunblockhumanoidplayermoveinput.class].entitytype">);
    if (!m_UnblockHumanoidPlayerMoveInput) {
        Logger::Error("Failed to create unblock humanoid player move input entity.");
        CleanupSpawnedEntities();
        return;
    }

    m_BlockPlayerGadgetInput = TEntityRef<ZCLBlockPlayerGadgetInput>::SpawnEntity(ResId<"[modules:/zclblockplayergadgetinput.class].entitytype">);
    if (!m_BlockPlayerGadgetInput) {
        Logger::Error("Failed to create block player gadget input entity.");
        CleanupSpawnedEntities();
        return;
    }

    m_UnblockPlayerGadgetInput =
        TEntityRef<ZCLUnblockPlayerGadgetInput>::SpawnEntity(ResId<"[modules:/zclunblockplayergadgetinput.class].entitytype">);
    if (!m_UnblockPlayerGadgetInput) {
        Logger::Error("Failed to create unblock player gadget input entity.");
        CleanupSpawnedEntities();
        return;
    }

    m_BlockHumanoidPlayerCloseCombatInput =
        TEntityRef<ZCLBlockHumanoidPlayerCloseCombatInput>::SpawnEntity(ResId<"[modules:/zclblockhumanoidplayerclosecombatinput.class].entitytype">);
    if (!m_BlockHumanoidPlayerCloseCombatInput) {
        Logger::Error("Failed to create block humanoid player close combat input entity.");
        CleanupSpawnedEntities();
        return;
    }

    m_UnblockHumanoidPlayerCloseCombatInput = TEntityRef<ZCLUnblockHumanoidPlayerCloseCombatInput>::SpawnEntity(
        ResId<"[modules:/zclunblockhumanoidplayerclosecombatinput.class].entitytype">
    );
    if (!m_UnblockHumanoidPlayerCloseCombatInput) {
        Logger::Error("Failed to create unblock humanoid player close combat input entity.");
        CleanupSpawnedEntities();
        return;
    }

    m_GetLocalPlayer = TEntityRef<ZCLGetLocalPlayerID>::SpawnEntity(ResId<"[modules:/zclgetlocalplayerid.class].entitytype">);
    if (!m_GetLocalPlayer) {
        Logger::Error("Failed to create get player entity.");
        CleanupSpawnedEntities();
        return;
    }

    m_TeleportHumanoidEntity = TEntityRef<ZCLTeleportHumanoidEntity>::SpawnEntity(ResId<"[modules:/zclteleporthumanoidentity.class].entitytype">);
    if (!m_TeleportHumanoidEntity) {
        Logger::Error("Failed to create teleport humanoid entity.");
        CleanupSpawnedEntities();
        return;
    }

    m_TeleportTarget = TEntityRef<ZSpatialEntity>::SpawnEntity(ResId<"[modules:/zspatialentity.class].entitytype">);
    if (!m_TeleportTarget) {
        Logger::Error("Failed to create spatial entity for teleport target.");
        CleanupSpawnedEntities();
        return;
    }

    m_GetLocalPlayerHumanoidCharacter =
        TEntityRef<ZCLGetLocalPlayerHumanoidCharacter>::SpawnEntity(ResId<"[modules:/zclgetlocalplayerhumanoidcharacter.class].entitytype">);
    if (!m_GetLocalPlayerHumanoidCharacter) {
        Logger::Error("Failed to create get local player humanoid character entity.");
        CleanupSpawnedEntities();
        return;
    }

    if (m_IsEditorStyleFreeCamEnabled) {
        m_FreeCameraControlEditorStyle->SetCameraEntity(m_FreeCamera);
        m_FreeCameraControlEditorStyle->SetActive(true);
    }
    else {
        m_FreeCameraControl->SetCameraEntity(m_FreeCamera);
        m_FreeCameraControl->SetActive(true);
    }

    TEntityRef<IRenderDestinationEntity> s_RenderDest;
    SDK()->Globals()->CameraManagerMain->GetActiveRenderDestinationEntity(s_RenderDest);
    const auto s_CurrentCamera = SDK()->Functions()->GetCurrentCamera->Call();
    if (!s_CurrentCamera || !s_RenderDest) {
        Logger::Error("Failed to retrieve active camera or render destination.");
        CleanupSpawnedEntities();
        return;
    }

    m_FreeCamera->SetObjectToWorldMatrixFromEditor(s_CurrentCamera->GetObjectToWorldMatrix());
    m_PreviousCameraSource = s_RenderDest->GetSource();
    s_RenderDest->SetSource(m_FreeCamera.m_entityRef);
    Logger::Info("Enabled free camera!");

    // Set up player input (un)blocking entities and block player input.
    const auto s_IntRef = TInterfaceRef<IIntValue>::FromEntityRef(m_GetLocalPlayer.m_entityRef);

    const auto s_LocalPlayerHumanoidCharacter =
        TInterfaceRef<ITEntityRefValue<ZHumanoidCharacterEntity>>::FromEntityRef(m_GetLocalPlayerHumanoidCharacter.m_entityRef);

    m_BlockHumanoidPlayerMoveInput.m_entityRef.SetProperty("m_playerID", s_IntRef);
    m_UnblockHumanoidPlayerMoveInput.m_entityRef.SetProperty("m_playerID", s_IntRef);
    m_BlockPlayerGadgetInput.m_entityRef.SetProperty("m_playerID", s_IntRef);
    m_UnblockPlayerGadgetInput.m_entityRef.SetProperty("m_playerID", s_IntRef);
    m_BlockHumanoidPlayerCloseCombatInput.m_entityRef.SetProperty("m_playerID", s_IntRef);
    m_UnblockHumanoidPlayerCloseCombatInput.m_entityRef.SetProperty("m_playerID", s_IntRef);
    m_TeleportHumanoidEntity.m_entityRef.SetProperty("m_humanoid", s_LocalPlayerHumanoidCharacter);
    m_TeleportHumanoidEntity.m_entityRef.SetProperty("m_targetSpatial", m_TeleportTarget);

    if (!m_IsPlayerInputEnabled) {
        m_BlockHumanoidPlayerMoveInput.m_entityRef.SignalInputPin("Do");
        m_BlockPlayerGadgetInput.m_entityRef.SignalInputPin("Do");
        m_BlockHumanoidPlayerCloseCombatInput.m_entityRef.SignalInputPin("Do");
    }

    if (m_GamePaused) {
        SDK()->Globals()->GameTimeManager->m_bPaused = true;
    }

    m_Initialized = false;
}

void FreeCam::DisableFreecam() {
    if (HasSpawnedEntities()) {
        Logger::Info("Disabling existing free camera.");
        TEntityRef<IRenderDestinationEntity> s_RenderDest;
        SDK()->Globals()->CameraManagerMain->GetActiveRenderDestinationEntity(s_RenderDest);

        if (s_RenderDest && m_PreviousCameraSource) {
            s_RenderDest->SetSource(m_PreviousCameraSource);
        }
        else {
            Logger::Warn("No stored previous camera source while toggling off.");
        }

        CleanupSpawnedEntities();
        Logger::Info("Disabled free camera and restored previous camera source.");
    }

    SDK()->Globals()->GameTimeManager->m_bPaused = false;
}

void FreeCam::TogglePlayerInput() {
    m_IsPlayerInputEnabled = !m_IsPlayerInputEnabled;

    if (m_IsPlayerInputEnabled) {
        if (m_UnblockHumanoidPlayerMoveInput) {
            m_UnblockHumanoidPlayerMoveInput.m_entityRef.SignalInputPin("Do");
        }

        if (m_UnblockPlayerGadgetInput) {
            m_UnblockPlayerGadgetInput.m_entityRef.SignalInputPin("Do");
        }

        if (m_UnblockHumanoidPlayerCloseCombatInput) {
            m_UnblockHumanoidPlayerCloseCombatInput.m_entityRef.SignalInputPin("Do");
        }
    }
    else {
        if (m_BlockHumanoidPlayerMoveInput) {
            m_BlockHumanoidPlayerMoveInput.m_entityRef.SignalInputPin("Do");
        }

        if (m_BlockPlayerGadgetInput) {
            m_BlockPlayerGadgetInput.m_entityRef.SignalInputPin("Do");
        }

        if (m_BlockHumanoidPlayerCloseCombatInput) {
            m_BlockHumanoidPlayerCloseCombatInput.m_entityRef.SignalInputPin("Do");
        }
    }
}

void FreeCam::SetFreeCamFrozen(bool p_Frozen) {
    m_IsFreeCamFrozen = p_Frozen;

    if (m_IsEditorStyleFreeCamEnabled) {
        if (m_FreeCameraControlEditorStyle.m_pInterfaceRef) {
            m_FreeCameraControlEditorStyle.m_pInterfaceRef->m_bActive = !p_Frozen;
        }
    }
    else {
        if (m_FreeCameraControl.m_pInterfaceRef) {
            m_FreeCameraControl.m_pInterfaceRef->m_bActive = !p_Frozen;
        }
    }
}

void FreeCam::TeleportPlayer() {
    if (!m_FreeCamera) {
        return;
    }

    SMatrix s_WorldMatrix = m_FreeCamera.m_pInterfaceRef->GetObjectToWorldMatrix();
    float4 s_InvertedDirection = float4(-s_WorldMatrix.ZAxis.x, -s_WorldMatrix.ZAxis.y, -s_WorldMatrix.ZAxis.z, -s_WorldMatrix.ZAxis.w);
    float4 s_From = s_WorldMatrix.Trans;
    float4 s_To = s_WorldMatrix.Trans + s_InvertedDirection * 500.f;

    TResourcePtr<ZEntityRef> s_EntityFactory;
    SDK()->Globals()->ResourceManager->LoadResource(
        s_EntityFactory,
        ResId<"[assembly:/_knt/design/utility/collision_query_presets.template?/collisionquery_static_block.entitytemplate].entityresource">
    );

    SEntityResource* s_EntityResource = static_cast<SEntityResource*>(s_EntityFactory.GetResourceData());
    ZCollisionQueryPreset* s_CollisionQueryPreset = s_EntityResource->entityRef.QueryInterface<ZCollisionQueryPreset>();

    ZRayQueryInput s_RayQueryInput = {};
    s_RayQueryInput.m_BlockingChannelMask = s_CollisionQueryPreset->m_BlockingChannelMask;
    s_RayQueryInput.m_OverlapChannelMask = s_CollisionQueryPreset->m_OverlapChannelMask;
    s_RayQueryInput.m_RequiredAttributeMask = s_CollisionQueryPreset->m_RequiredAttributeMask;
    s_RayQueryInput.m_ForbiddenAttributeMask = s_CollisionQueryPreset->m_ForbiddenAttributeMask;
    s_RayQueryInput.m_eRayCollidables = s_CollisionQueryPreset->m_eCollidableTypes;
    s_RayQueryInput.m_vFrom = s_From;
    s_RayQueryInput.m_vTo = s_To;
    s_RayQueryInput.m_TypedQueryMask = s_CollisionQueryPreset->m_TypedQueryMask;
    s_RayQueryInput.m_eRayDetailLevel = ERayDetailLevel::RAYDETAILS_MESH;

    ZRayQueryOutput s_RayQueryOutput = {};

    if (!SDK()->Globals()->CollisionManager->RayCastClosestHit(&s_RayQueryOutput, s_RayQueryInput)) {
        Logger::Error("Raycast failed.");
        return;
    }

    if (m_TeleportHumanoidEntity) {
        auto s_LocalPlayer = SDK()->Globals()->LocalPlayerData->m_pCharacterImpl->m_pCharacter;

        if (!s_LocalPlayer) {
            return;
        }

        const auto s_PlayerSpatialEntity = s_LocalPlayer.m_entityRef.QueryInterface<ZSpatialEntity>();

        if (!s_PlayerSpatialEntity) {
            return;
        }

        SMatrix s_Transform = s_PlayerSpatialEntity->GetObjectToWorldMatrix();
        s_Transform.Trans = s_RayQueryOutput.m_vPosition;

        m_TeleportTarget->SetObjectToWorldMatrixFromEditor(s_Transform);
        m_TeleportHumanoidEntity.m_entityRef.SignalInputPin("Do");
    }
}

bool FreeCam::HasSpawnedEntities() const {
    return m_FreeCamera && (m_FreeCameraControl || m_FreeCameraControlEditorStyle);
}

void FreeCam::CleanupSpawnedEntities() {
    if (m_PreviousCameraSource) {
        TEntityRef<IRenderDestinationEntity> s_RenderDest;
        SDK()->Globals()->CameraManagerMain->GetActiveRenderDestinationEntity(s_RenderDest);

        if (s_RenderDest) {
            s_RenderDest->SetSource(m_PreviousCameraSource);
        }
    }

    if (m_FreeCameraControl) {
        m_FreeCameraControl->SetActive(false);
    }
    else if (m_FreeCameraControlEditorStyle) {
        m_FreeCameraControlEditorStyle->SetActive(false);
    }

    if (m_UnblockHumanoidPlayerMoveInput) {
        m_UnblockHumanoidPlayerMoveInput.m_entityRef.SignalInputPin("Do");
    }

    if (m_UnblockPlayerGadgetInput) {
        m_UnblockPlayerGadgetInput.m_entityRef.SignalInputPin("Do");
    }

    if (m_UnblockHumanoidPlayerCloseCombatInput) {
        m_UnblockHumanoidPlayerCloseCombatInput.m_entityRef.SignalInputPin("Do");
    }

    if (m_FreeCameraControl) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_FreeCameraControl.m_entityRef);
    }
    else if (m_FreeCameraControlEditorStyle) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_FreeCameraControlEditorStyle.m_entityRef);
    }

    if (m_FreeCamera) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_FreeCamera.m_entityRef);
    }

    if (m_BlockHumanoidPlayerMoveInput) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_BlockHumanoidPlayerMoveInput.m_entityRef);
    }

    if (m_UnblockHumanoidPlayerMoveInput) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_UnblockHumanoidPlayerMoveInput.m_entityRef);
    }

    if (m_BlockPlayerGadgetInput) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_BlockPlayerGadgetInput.m_entityRef);
    }

    if (m_UnblockPlayerGadgetInput) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_UnblockPlayerGadgetInput.m_entityRef);
    }

    if (m_BlockHumanoidPlayerCloseCombatInput) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_BlockHumanoidPlayerCloseCombatInput.m_entityRef);
    }

    if (m_UnblockHumanoidPlayerCloseCombatInput) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_UnblockHumanoidPlayerCloseCombatInput.m_entityRef);
    }

    if (m_GetLocalPlayer) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_GetLocalPlayer.m_entityRef);
    }

    if (m_TeleportHumanoidEntity) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_TeleportHumanoidEntity.m_entityRef);
    }

    if (m_TeleportTarget) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_TeleportTarget.m_entityRef);
    }

    if (m_GetLocalPlayerHumanoidCharacter) {
        SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, m_GetLocalPlayerHumanoidCharacter.m_entityRef);
    }

    m_FreeCameraControl = {};
    m_FreeCameraControlEditorStyle = {};
    m_FreeCamera = {};
    m_BlockHumanoidPlayerMoveInput = {};
    m_UnblockHumanoidPlayerMoveInput = {};
    m_BlockPlayerGadgetInput = {};
    m_UnblockPlayerGadgetInput = {};
    m_BlockHumanoidPlayerCloseCombatInput = {};
    m_UnblockHumanoidPlayerCloseCombatInput = {};
    m_GetLocalPlayer = {};
    m_TeleportHumanoidEntity = {};
    m_TeleportTarget = {};
    m_GetLocalPlayerHumanoidCharacter = {};
    m_PreviousCameraSource = {};
    m_Initialized = false;
}

DEFINE_PLUGIN_DETOUR(
    FreeCam, ZString*, ZFreeCameraControlEntity_GenerateActionBindingString, ZFreeCameraControlEntity* p_Th, ZString& p_Result, int32_t p_ControllerId
) {
    ZString* s_Res = p_Hook->CallOriginal(p_Th, p_Result, p_ControllerId);

    p_Result.m_pChars = "FreeCamControl={"
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
                        "AnalogCamXAxis=ana(gc,rightx);"
                        "AnalogCamYAxis=ana(gc,righty);"
                        "AnalogMoveXAxis=ana(gc,leftx);"
                        "AnalogMoveYAxis=ana(gc,lefty);"
                        "MoveInZDirection=| hold(gc,right_bumper) hold(gc,right1);"
                        "RollModifier2=| hold(gc,a) hold(gc,cross);"
                        "RollAxis=ana(gc,leftx);"
                        "ResetRoll2=| hold(gc,leftstick) hold(gc,left_thumb);"
                        "FovModifier2=| hold(gc,y) hold(gc,triangle);"
                        "FovAxis=ana(gc,lefty);"
                        "ResetFov2=| hold(gc,leftstick) hold(gc,left_thumb);"
                        "SpeedModifier2=| hold(gc,b) hold(gc,circle);"
                        "SpeedTranslationAxis=ana(gc,lefty);"
                        "SpeedRotationAxis=ana(gc,leftx);"
                        "ResetSpeed2=| hold(gc,leftstick) hold(gc,left_thumb);"
                        "MoveInWorldSpaceTrigger=ana(gc,left_analog_trigger);"
                        "MoveInWorldSpaceButton=hold(gc,left2);"
                        "MoveInWorldSpaceXAxis=ana(gc,leftx);"
                        "MoveInWorldSpaceYAxis=ana(gc,lefty);"
                        "MoveInWorldSpaceZAxis=ana(gc,righty);"
                        "ActivateGameControl=| hold(gc,left_bumper) hold(gc,left1);"
                        "TemporaryCamSpeedMultiplierTrigger=ana(gc,right_analog_trigger);"
                        "TemporaryCamSpeedMultiplierTrigger2=ana(gc,right2_analog);"
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

    if (!m_FreeCamera) {
        return {HookAction::Return()};
    }

    constexpr float c_MouseSensitivity = 0.0007f;
    constexpr float c_StickLookSpeed = 1.5f;
    constexpr float c_BaseMoveSpeed = 10.0f;
    constexpr float c_MaxPitch = 1.55334f; // bit less than 90deg

    const SVector3 s_WorldUp(0.0f, 0.0f, 1.0f);
    const float s_Dt = std::clamp(p_Dt, 0.0f, 0.1f);

    if (!m_Initialized) {
        const SMatrix s_Mat = m_FreeCamera->GetObjectToWorldMatrix();
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

    if (p_Th->ForceGetDigital(p_Th->m_ActivateGameControl)) {
        p_Th->m_bIsGameControlActive = true;
        return {HookAction::Return()};
    }

    p_Th->m_bIsGameControlActive = false;

    const int s_Controller = p_Th->m_nControllerId;
    const float s_MouseYaw = p_Th->m_TurnCamera.Analog();
    const float s_MousePitch = p_Th->m_TiltCamera.Analog();
    const float s_StickYaw = p_Th->m_AnalogCamXAxis.Analog();
    const float s_StickPitch = -p_Th->m_AnalogCamYAxis.Analog();

    constexpr float c_RollSpeed = 1.5f;
    constexpr float c_SpeedAdjustRate = 2.0f;
    constexpr float c_DegToRad = 0.01745329f;

    float s_MoveRight = p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, -p_Th->m_AnalogMoveXAxis.Analog());
    float s_MoveForward = p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_AnalogMoveYAxis.Analog());
    float s_MoveUp = p_Th->GetCombinedAxis(p_Th->m_MoveZPositive, p_Th->m_MoveZNegative, 0.0f);

    const float s_TempSpeed = std::max({
        p_Th->m_TemporaryCamSpeedMultiplierTrigger.Analog(),
        p_Th->m_TemporaryCamSpeedMultiplierTrigger2.Analog(),
        p_Th->m_TemporaryCamSpeedMultiplierLeftShift.Digital() ? 1.0f : 0.0f,
        p_Th->m_TemporaryCamSpeedMultiplierRightShift.Digital() ? 1.0f : 0.0f,
    });
    const float s_TempMultiplier = s_TempSpeed * 9.0f + 1.0f;

    const bool s_RollModifier = p_Th->ForceGetDigital(p_Th->m_RollModifier2) || p_Th->m_RollModifier.Digital();
    const bool s_FOVModifier = p_Th->ForceGetDigital(p_Th->m_FovModifier2) || p_Th->m_FovModifier.Digital();

    const bool s_SpeedModifier = p_Th->m_SpeedModifier.Digital() || p_Th->ForceGetDigital(p_Th->m_SpeedModifier2);
    const bool s_FixedModifier = p_Th->m_TiltTurnCameraFixedDegreeModifier.Digital();

    const bool s_PlanarMove =
        p_Th->m_MoveInWorldSpace.Digital() || p_Th->m_MoveInWorldSpaceButton.Digital() || p_Th->m_MoveInWorldSpaceTrigger.Analog() > 0.6f;

    if (s_PlanarMove) {
        s_MoveRight = p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, -p_Th->m_MoveInWorldSpaceXAxis.Analog());

        s_MoveForward = p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_MoveInWorldSpaceYAxis.Analog());

        s_MoveUp = p_Th->GetCombinedAxis(p_Th->m_MoveZPositive, p_Th->m_MoveZNegative, p_Th->m_MoveInWorldSpaceZAxis.Analog());
    }

    if (s_RollModifier || s_FOVModifier || s_SpeedModifier || s_FixedModifier) {
        s_MoveRight = 0.0f;
        s_MoveForward = 0.0f;
    }

    if (s_RollModifier) {
        if (p_Th->m_ResetRoll.Digital() || p_Th->ForceGetDigital(p_Th->m_ResetRoll2)) {
            m_Roll = 0.0f;
        }
        else {
            const float s_RollInput = p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, -p_Th->m_RollAxis.Analog());
            m_Roll += s_RollInput * c_RollSpeed * s_Dt;
        }
    }

    if (s_FOVModifier) {
        if (p_Th->m_ResetFov.Digital() || p_Th->ForceGetDigital(p_Th->m_ResetFov2)) {
            p_Th->m_fFov = p_Th->m_fInitialFov;
            p_Th->m_fDeltaFov = 0.0f;
        }
        else {
            p_Th->m_fDeltaFov = p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_FovAxis.Analog());
        }
    }

    if (s_SpeedModifier) {
        if (p_Th->m_ResetSpeed.Digital() || p_Th->ForceGetDigital(p_Th->m_ResetSpeed2)) {
            m_TranslationSpeedMul = 1.0f;
            m_RotationSpeedMul = 1.0f;
        }
        else {
            const float s_TransAdjust = p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_SpeedTranslationAxis.Analog());
            const float s_RotAdjust = p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, p_Th->m_SpeedRotationAxis.Analog());
            m_TranslationSpeedMul = std::clamp(m_TranslationSpeedMul * std::exp(s_TransAdjust * c_SpeedAdjustRate * s_Dt), 0.05f, 200.0f);
            m_RotationSpeedMul = std::clamp(m_RotationSpeedMul * std::exp(s_RotAdjust * c_SpeedAdjustRate * s_Dt), 0.1f, 10.0f);
        }
    }

    if (!s_PlanarMove) {
        if (!s_FixedModifier) {
            const float s_LookScale = m_RotationSpeedMul * p_Th->m_fFovDependentSpeedMultiplier;
            m_Yaw += (s_MouseYaw * c_MouseSensitivity + s_StickYaw * c_StickLookSpeed * s_Dt) * s_LookScale;
            m_Pitch += (-s_MousePitch * c_MouseSensitivity - s_StickPitch * c_StickLookSpeed * s_Dt) * s_LookScale;
            p_Th->m_bIsTiltTurnCamFixedDegreeModifierActive = false;
        }
        else {
            const float s_SnapX = p_Th->GetCombinedAxis(p_Th->m_MoveXPositive, p_Th->m_MoveXNegative, -p_Th->m_AnalogMoveXAxis.Analog());
            const float s_SnapY = p_Th->GetCombinedAxis(p_Th->m_MoveYPositive, p_Th->m_MoveYNegative, p_Th->m_AnalogMoveYAxis.Analog());

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
    }
    else {
        p_Th->m_bIsTiltTurnCamFixedDegreeModifierActive = false;
    }

    m_Pitch = std::clamp(m_Pitch, -c_MaxPitch, c_MaxPitch);

    const float s_CosYaw = std::cos(m_Yaw);
    const float s_SinYaw = std::sin(m_Yaw);
    const float s_CosPitch = std::cos(m_Pitch);
    const float s_SinPitch = std::sin(m_Pitch);

    const SVector3 s_Forward(s_SinYaw * s_CosPitch, s_CosYaw * s_CosPitch, s_SinPitch);
    const SVector3 s_Right = s_WorldUp.Cross(s_Forward).Normalized();
    const SVector3 s_Up = s_Forward.Cross(s_Right).Normalized();

    const float s_OldFovMultiplier = p_Th->m_fFovDependentSpeedMultiplier;
    const float s_NewFov = p_Th->m_fFov + c_BaseMoveSpeed * s_TempMultiplier * m_TranslationSpeedMul * s_OldFovMultiplier * p_Th->m_fDeltaFov * s_Dt;

    p_Th->m_fFov = std::clamp(s_NewFov, 5.0f, 170.0f);

    if (p_Th->m_cameraEntity.m_pInterfaceRef) {
        p_Th->m_cameraEntity.m_pInterfaceRef->SetFovYDeg(p_Th->m_fFov);
    }

    float s_FovDependentSpeedMultiplier;

    if (p_Th->m_fFov < p_Th->m_fInitialFov) {
        const float t = (p_Th->m_fFov - 5.0f) / (p_Th->m_fInitialFov - 5.0f);

        s_FovDependentSpeedMultiplier = t * 0.9f + 0.1f;
    }
    else {
        const float t = (p_Th->m_fFov - p_Th->m_fInitialFov) / (170.0f - p_Th->m_fInitialFov);

        s_FovDependentSpeedMultiplier = t + 1.0f;
    }

    p_Th->m_fFovDependentSpeedMultiplier = s_FovDependentSpeedMultiplier;

    const float s_Speed = c_BaseMoveSpeed * s_TempMultiplier * m_TranslationSpeedMul * p_Th->m_fFovDependentSpeedMultiplier * s_Dt;
    m_Position = m_Position + s_Right * (s_MoveRight * s_Speed) + s_Forward * (s_MoveForward * s_Speed) + s_WorldUp * (s_MoveUp * s_Speed);

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
    m_FreeCamera->SetObjectToWorldMatrixFromEditor(s_Out);

    // Overwrite transform manually because for some reason calling just
    // SetObjectToWorldMatrixFromEditor results in stepping / snapping when looking around.
    m_FreeCamera->m_mTransform = s_Out.ToMatrix43();
    m_FreeCamera->m_bWorldTransformDirty = false;

    return {HookAction::Return()};
}

DEFINE_PLUGIN_DETOUR(
    FreeCam, ZString*, ZFreeCameraControlEditorStyleEntity_GenerateActionBindingString, ZFreeCameraControlEditorStyleEntity* p_Th, ZString& p_Result,
    int32_t p_ControllerId
) {
    ZString* s_Res = p_Hook->CallOriginal(p_Th, p_Result, p_ControllerId);

    p_Result.m_pChars = "FreeCamControlEditorStyle={"
                        "MousePosX=rel(ms,x);"
                        "MousePosY=rel(ms,y);"
                        "MoveXPositive=| hold(kb,left) hold(kb,a);"
                        "MoveXNegative=| hold(kb,right) hold(kb,d);"
                        "MoveYPositive=| hold(kb,up) hold(kb,w);"
                        "MoveYNegative=| hold(kb,down) hold(kb,s);"
                        "MoveZPositive=| hold(kb,pgup) hold(kb,e);"
                        "MoveZNegative=| hold(kb,pgdn) hold(kb,q);"
                        "MoveFast=| hold(kb,lshift) hold(kb,rshift);"
                        "Zoom=rel(ms,wheel);"
                        "ZoomPrecision=| hold(kb,lalt) hold(kb,ralt);"
                        "ZoomToSelection=hold(kb,z);"
                        "OrbitCamera=| hold(kb,lalt) hold(kb,ralt);"
                        "ActivateRotate=hold(ms,right);"
                        "ActivateObjectHook=hold(ms,middle);"
                        "};";

    p_Result.m_nLength = static_cast<uint32_t>(std::strlen(p_Result.m_pChars)) | 0x80000000;

    return {HookAction::Return(), s_Res};
}

DEFINE_PLUGIN_DETOUR(
    FreeCam, void, ZFreeCameraControlEditorStyleEntity_HandleDrag, ZFreeCameraControlEditorStyleEntity* const th, bool bRotationIsActive,
    bool bObjectHookIsActive, bool bIsOrbitActive
) {
    if (!bRotationIsActive && !bObjectHookIsActive) {
        th->m_bDraggingIsActive = false;

        return {HookAction::Return()};
    }

    if (!th->m_bDraggingIsActive) {
        th->m_bDraggingIsActive = true;
    }

    /*
     * The original implementation uses anaraw(ms,posx/posy) and computes
     * dragDelta = m_vMousePos - m_vLastDragPoint.
     *
     * This implementation uses rel(ms,x/y), so m_vMousePos already contains
     * per-frame mouse deltas and m_vLastDragPoint is intentionally
     * unused.
     */
    const float s_DeltaX = th->m_vMousePos.x;
    const float s_DeltaY = th->m_vMousePos.y;

    if (s_DeltaX == 0.0f && s_DeltaY == 0.0f) {
        return {HookAction::Return()};
    }

    constexpr float c_OrbitSensitivity = 0.003f;

    if (bObjectHookIsActive) {
        if (!bIsOrbitActive) {
            SDK()->Functions()->ZCameraUtil_PanCamera->Call(
                TEntityRef<ICameraEntity>(th->m_pControlledCameraEntity.m_entityRef),
                TEntityRef<ZSpatialEntity>(th->m_pControlledCameraEntity.m_entityRef), {s_DeltaX, s_DeltaY, 0.f, 0.f}, th->m_vHookPoint
            );

            return {HookAction::Return()};
        }

        SDK()->Functions()->ZFreeCameraControlEditorStyleEntity_OrbitCamera->Call(
            th, float4(s_DeltaX * c_OrbitSensitivity, s_DeltaY * c_OrbitSensitivity, 0.f, 0.f), th->m_vHookPoint
        );

        return {HookAction::Return()};
    }

    if (bIsOrbitActive) {
        SDK()->Functions()->ZFreeCameraControlEditorStyleEntity_OrbitCamera->Call(
            th, float4(s_DeltaX * c_OrbitSensitivity, s_DeltaY * c_OrbitSensitivity, 0.f, 0.f), th->m_vHookPoint
        );

        return {HookAction::Return()};
    }

    if (!th->m_pControlledCameraEntity) {
        return {HookAction::Return()};
    }

    constexpr float c_MouseSensitivity = 0.16f;

    const float s_YawDelta = s_DeltaX * c_MouseSensitivity;
    const float s_PitchDelta = s_DeltaY * c_MouseSensitivity;

    th->Rotate(s_YawDelta, s_PitchDelta);

    return {HookAction::Return()};
}

DEFINE_PLUGIN_DETOUR(
    FreeCam, void, ZFreeCameraControlEditorStyleEntity_MoveCameraWithKey, ZFreeCameraControlEditorStyleEntity* th, float fDeltaTime
) {
    if (!th->m_pControlledCameraEntity) {
        return {HookAction::Return()};
    }

    float s_MoveX = 0.0f;
    float s_MoveY = 0.0f;
    float s_MoveZ = 0.0f;

    if (th->m_MoveXPositive.Digital()) {
        s_MoveX -= 1.0f;
    }

    if (th->m_MoveXNegative.Digital()) {
        s_MoveX += 1.0f;
    }

    if (th->m_MoveYPositive.Digital()) {
        s_MoveY += 1.0f;
    }

    if (th->m_MoveYNegative.Digital()) {
        s_MoveY -= 1.0f;
    }

    if (th->m_MoveZPositive.Digital()) {
        s_MoveZ += 1.0f;
    }

    if (th->m_MoveZNegative.Digital()) {
        s_MoveZ -= 1.0f;
    }

    if (s_MoveX == 0.0f && s_MoveY == 0.0f && s_MoveZ == 0.0f) {
        return {HookAction::Return()};
    }

    SMatrix s_CameraToWorld = th->m_pControlledCameraEntity.m_pInterfaceRef->GetObjectToWorldMatrix();

    if (s_MoveY != 0.0f) {
        th->MoveForwardBackward(s_CameraToWorld, s_MoveY > 0.0f ? fDeltaTime : -fDeltaTime);
    }

    if (s_MoveX != 0.0f) {
        th->MoveLeftRight(s_CameraToWorld, s_MoveX > 0.0f ? fDeltaTime : -fDeltaTime);
    }

    if (s_MoveZ != 0.0f) {
        th->MoveUpDown(s_CameraToWorld, s_MoveZ > 0.0f ? fDeltaTime : -fDeltaTime);
    }

    SDK()->Functions()->ZFreeCameraControlEditorStyleEntity_ApplyCameraMatrix->Call(th, s_CameraToWorld);

    return {HookAction::Return()};
}

DEFINE_ZKNT_PLUGIN(FreeCam)
