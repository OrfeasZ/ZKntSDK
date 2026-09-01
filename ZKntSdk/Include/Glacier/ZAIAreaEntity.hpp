#pragma once

#include "ZEntity.hpp"

class ITriggerVolume;

class ZAIAreaEntityBase : public ZEntityImpl {
  public:
    PAD(0x4);                    // 0x18
    bool m_bDbgAreaIsRegistered; // 0x1C
    uint32 m_nDbgConcept;        // 0x20
    uint32 m_nDbgParentConcept;  // 0x24
    PAD(0x18);                   // 0x28
};

class ZAIAreaEntity : public ZAIAreaEntityBase {
  public:
    TEntityRef<ZAIAreaEntityBase> m_rParentArea;       // 0x40
    TArray<TEntityRef<ITriggerVolume>> m_aAreaVolumes; // 0x58
    PAD(0x38);                                         // 0x70
};
