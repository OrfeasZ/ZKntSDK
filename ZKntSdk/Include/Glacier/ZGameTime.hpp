#pragma once

#include <cstdint>

#include "ZScene.hpp"

class ZGameTime {
  public:
    ZGameTime() = default;

    explicit ZGameTime(int64_t p_Ticks) {
        m_nTicks = p_Ticks;
    }

    explicit ZGameTime(float p_Seconds) {
        m_nTicks = static_cast<int64_t>(p_Seconds * (1 << FractionBits));
    }

    explicit ZGameTime(double p_Seconds) {
        m_nTicks = static_cast<int64_t>(p_Seconds * (1 << FractionBits));
    }

    explicit operator int64_t() const {
        return m_nTicks;
    }

    explicit operator float() const {
        return m_nTicks * TicksToSeconds;
    }

    ZGameTime operator+(const ZGameTime& p_Other) const {
        return ZGameTime(m_nTicks + p_Other.m_nTicks);
    }

    ZGameTime& operator+=(const ZGameTime& p_Other) {
        m_nTicks += p_Other.m_nTicks;

        return *this;
    }

    ZGameTime operator-(const ZGameTime& p_Other) const {
        return ZGameTime(m_nTicks - p_Other.m_nTicks);
    }

    ZGameTime& operator-=(const ZGameTime& p_Other) {
        m_nTicks -= p_Other.m_nTicks;

        return *this;
    }

    ZGameTime operator*(const ZGameTime& p_Other) const {
        return ZGameTime((m_nTicks * p_Other.m_nTicks) >> FractionBits);
    }

    ZGameTime& operator*=(const ZGameTime& p_Other) {
        m_nTicks = (m_nTicks * p_Other.m_nTicks) >> FractionBits;

        return *this;
    }

    ZGameTime operator/(const ZGameTime& p_Other) const {
        return ZGameTime((m_nTicks << FractionBits) / p_Other.m_nTicks);
    }

    ZGameTime& operator/=(const ZGameTime& p_Other) {
        m_nTicks = (m_nTicks << FractionBits) / p_Other.m_nTicks;

        return *this;
    }

    bool operator<(const ZGameTime& rhs) const {
        return m_nTicks < rhs.m_nTicks;
    }

    static ZGameTime FromSeconds(float p_Seconds) {
        return ZGameTime(static_cast<int64_t>(p_Seconds * (1 << FractionBits)));
    }

    static ZGameTime FromSeconds(double p_Seconds) {
        return ZGameTime(static_cast<int64_t>(p_Seconds * (1 << FractionBits)));
    }

    double ToSeconds() const {
        return static_cast<double>(m_nTicks) / (1 << FractionBits);
    }

    int64_t m_nTicks;

    static constexpr int FractionBits = 20;
    static constexpr float TicksToSeconds = 1.0f / (1 << FractionBits);
};

class ZGameTimeManager : public IComponentInterface, public ZSceneLifecycleListener {
  public:
    PAD(0x80);
    bool m_bPaused; // 0x90
};
