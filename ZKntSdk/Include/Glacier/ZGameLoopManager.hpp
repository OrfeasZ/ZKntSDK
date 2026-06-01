#pragma once

#include "Common.hpp"
#include "IComponentInterface.hpp"
#include "SGameUpdateEvent.hpp"
#include "ZDelegate.hpp"
#include "EUpdateMode.hpp"
#include <IModSDK.hpp>
#include <Functions.hpp>

class ZUpdateEventContainer;

class ZGameLoopManager : public IComponentInterface {
  public:
    virtual ~ZGameLoopManager() = 0;

    void RegisterFrameUpdate(const ZDelegate<void(const SGameUpdateEvent&)>& p_Delegate, int32_t p_Priority, EUpdateMode p_UpdateMode) {
        SDK()->Functions()->ZUpdateEventContainer_AddDelegate->Call(m_pFrameUpdates, p_Delegate, p_Priority, p_UpdateMode);
    }

    void UnregisterFrameUpdate(const ZDelegate<void(const SGameUpdateEvent&)>& p_Delegate, int32_t p_Priority, EUpdateMode p_UpdateMode) {
        SDK()->Functions()->ZUpdateEventContainer_RemoveDelegate->Call(m_pFrameUpdates, p_Delegate, p_Priority, p_UpdateMode);
    }

    PAD(0x80);
    ZUpdateEventContainer* m_pFrameUpdates; // 0x88
    ZUpdateEventContainer* m_pGameUpdates;  // 0x90
};
