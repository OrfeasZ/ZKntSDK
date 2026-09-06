#pragma once

#include "ZEntity.hpp"

class ZKntStartingCheckpoint : public ZEntityImpl {
  public:
    TEntityRef<ZKntCheckpointEntity> m_startupCheckpoint; // 0x18
};
