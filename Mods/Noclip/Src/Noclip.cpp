#include "Noclip.hpp"
#include <Glacier/ZPlayer.hpp>
#include <Glacier/ZGameLoopManager.hpp>
#include <Glacier/ZMath.hpp>
#include <Logging.hpp>

Noclip::Noclip()
    : m_ToggleNoclipAction("ToggleNoclip")
    , m_ForwardAction("Forward")
    , m_BackwardAction("Backward")
    , m_LeftAction("Left")
    , m_RightAction("Right")
    , m_FastAction("Fast") {}

void Noclip::OnEngineInitialized() {
    const ZMemberDelegate<Noclip, void(const SGameUpdateEvent&)> s_Delegate(this, &Noclip::OnFrameUpdate);

    SDK()->Globals()->GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdatePlayMode);

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

void Noclip::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) {
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

        if (m_NoclipEnabled) {
            m_PlayerPosition = s_PlayerSpatialEntity->GetObjectToWorldMatrix();
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

    /*SMatrix43 s_Transform;
    s_Transform.XAxis = m_PlayerPosition.XAxis;
    s_Transform.YAxis = m_PlayerPosition.YAxis;
    s_Transform.ZAxis = m_PlayerPosition.ZAxis;
    s_Transform.Trans = m_PlayerPosition.Trans;

    s_LocalHitman.m_entityRef.SetProperty<SMatrix43>("m_mTransform", s_Transform);*/

    s_PlayerSpatialEntity->SetObjectToWorldMatrixFromEditor(m_PlayerPosition);
}

DEFINE_ZKNT_PLUGIN(Noclip)
