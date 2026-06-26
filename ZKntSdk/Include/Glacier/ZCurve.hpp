#pragma once

#include "TArray.hpp"
#include "ZPrimitives.hpp"
#include "ZMath.hpp"

enum class ECurveInterpolationMode : int8_t {
    CONSTANT = 0,
    LINEAR = 1,
    SPLINE = 2,
    EXPONENTIAL1 = 3,
    EXPONENTIAL2 = 4,
    EXPONENTIAL3 = 5,
    LOGARITHMIC1 = 6,
    LOGARITHMIC2 = 7,
    LOGARITHMIC3 = 8,
    SC = 9,
    SINE = 10,
    SQRT1 = 11,
    SQRT2 = 12,
    SQRT3 = 13
};

enum class EContinuity : int8_t { C0 = 0, C1 = 1, C2 = 2 };

enum class ECurveVersion : int8_t { CRV01 = 0, CRV02 = 1, CRV03 = 2 };

struct SCurveKey {
    float32 Time;                              // 0x0
    float32 Value;                             // 0x4
    SVector2 TangentIn;                        // 0x8
    SVector2 TangentOut;                       // 0x10
    ECurveInterpolationMode InterpolationMode; // 0x18
    EContinuity ContinuityKind;                // 0x19
};

class ZCurve {
  public:
    TArray<TFixedArray<float32, 8>> data; // 0x0
    TArray<SCurveKey> m_controlPoints;    // 0x18
    ECurveVersion m_version;              // 0x30
    ZString m_name;                       // 0x38
};
