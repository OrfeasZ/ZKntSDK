#pragma once

#include "ZSpatialEntity.hpp"

class ZDebugGizmoEntity : public ZSpatialEntity {
  public:
    enum class EDrawLayer : int32_t {
        DL_DEFAULT = 0,
        DL_LIGHT = 1,
        DL_PARTICLES = 2,
        DL_PARTITIONING = 3,
        DL_DECALS = 4,
        DL_CROWD = 5,
        DL_TERRAIN = 6,
        DL_PHYSICS = 7,
        DL_PROJECTILE = 8,
        DL_AI = 9,
        DL_GAME = 10,
        DL_DESIGN = 11,
        DL_CHARACTER = 12,
        DL_CAMERA = 13,
        DL_DEBUGCAMERA = 14,
        DL_INTERACTIONS = 15,
        DL_TRIGGERS = 16,
        DL_ENGINE = 17,
        DL_SPLINE = 18,
        DL_CORELOGIC = 19,
        DL_SOUND = 20,
        DL_ANIMATION = 21,
        DL_CLOTH = 22,
        DL_SOUND_PARTITIONING = 23,
        DL_UI = 24,
        DL_PERFORMANCE = 25,
        DL_VEHICLE = 26,
    };

    bool m_bSelectVisibleOnly;                  // 0xA8
    ZDebugGizmoEntity::EDrawLayer m_eDrawLayer; // 0xAC
    ZResourcePtr m_GizmoGeomRID;                // 0xB0
};
