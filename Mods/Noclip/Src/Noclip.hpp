#pragma once

#include <IPluginInterface.hpp>
#include <Glacier/ZCamera.hpp>

class Noclip : public zknt::IPluginInterface {
  public:
    Noclip();

    void OnEngineInitialized() override;

  private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    bool m_NoclipEnabled = false;
    SMatrix m_PlayerPosition = {};

    ZInputAction m_ToggleNoclipAction;
    ZInputAction m_ForwardAction;
    ZInputAction m_BackwardAction;
    ZInputAction m_LeftAction;
    ZInputAction m_RightAction;
    ZInputAction m_FastAction;
};

DECLARE_ZKNT_PLUGIN(Noclip)
