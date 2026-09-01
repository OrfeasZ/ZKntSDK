#pragma once

#include "ZSpatialEntity.hpp"

class ZBoundedEntity : public ZSpatialEntity {
  public:
    SVector3 m_vCenter;   // 0xA8
    SVector3 m_vHalfSize; // 0xB4
};
