#pragma once

#include <cstdint>
#include <algorithm>

struct SColorRGB {
    uint32_t GetAsUInt32() const {
        const uint32_t r = std::clamp(static_cast<int32_t>(this->r * 256.0f), 0, 255);
        const uint32_t g = std::clamp(static_cast<int32_t>(this->g * 256.0f), 0, 255);
        const uint32_t b = std::clamp(static_cast<int32_t>(this->b * 256.0f), 0, 255);

        return 0xFF000000 | (b << 16) | (g << 8) | r; // 0xFFBBGGRR
    }

    float r;
    float g;
    float b;
};
