#pragma once

#include <cstdint>

class ILightEntity {
  public:
    enum class ELightType : int8_t {
        LT_DIRECTIONAL = 0,
        LT_ENVIRONMENT = 1,
        LT_OMNI = 2,
        LT_SPOT = 3,
        LT_SQUARESPOT = 4,
        LT_CAPSULE = 5,
        LT_AREA_QUAD = 6,
    };
};
