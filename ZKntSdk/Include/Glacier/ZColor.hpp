#pragma once

#include "ZMath.hpp"

class ZColor {
  public:
    static SVector4 UnpackUnsigned(const uint32_t p_PackedVector4) {
        const uint32_t s_Alpha = (p_PackedVector4 >> 24) & 0xFF;
        const uint32_t s_Blue = (p_PackedVector4 >> 16) & 0xFF;
        const uint32_t s_Green = (p_PackedVector4 >> 8) & 0xFF;
        const uint32_t s_Red = p_PackedVector4 & 0xFF;

        return SVector4(
            static_cast<float>(s_Red) / 255.f, static_cast<float>(s_Green) / 255.f, static_cast<float>(s_Blue) / 255.f,
            static_cast<float>(s_Alpha) / 255.f
        );
    }
};
