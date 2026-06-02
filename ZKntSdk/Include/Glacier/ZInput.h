#pragma once

#include <IModSDK.hpp>

class ZInputAction {
  public:
    ZInputAction() = default;
    ZInputAction(const char* p_Name) : m_szName(p_Name) {}

    float Analog() {
        return SDK()->Functions()->ZInputAction_Analog->Call(this);
    }

    bool Digital() {
        return SDK()->Functions()->ZInputAction_Digital->Call(this);
    }

  public:
    void* ptr = nullptr;
    void* ptr2 = nullptr;
    const char* m_szName;
};
