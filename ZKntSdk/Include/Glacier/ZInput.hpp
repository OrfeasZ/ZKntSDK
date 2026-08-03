#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "TArray.hpp"

#include <IModSDK.hpp>

class ZInputContext;

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
    void* m_Unk0 = nullptr;
    ZInputContext* m_InputContext = nullptr;
    const char* m_szName;
};

class IDevice {
  public:
    virtual ~IDevice() = 0;
};

class ZDevice : public IDevice {
  public:
    PAD(0x168);                  // 0x8
    LPDIRECTINPUTDEVICE8 m_pDev; // 0x170
    LPDIRECTINPUT8 m_pDI;        // 0x178
};

class ZDeviceMouse_DI_Windows : public ZDevice {};

class ZDeviceKeyboard_DI_Windows : public ZDevice {};

class IInputPlatform {
  public:
    virtual ~IInputPlatform() = 0;
};

class ZInputPlatform_Windows : public IInputPlatform {
  public:
    PAD(0x60);                   // 0x8
    TArray<IDevice*> m_aDevices; // 0x68
};

class ZInputManager : public IComponentInterface {
  public:
    IInputPlatform* m_pInputPlatform; // 0x8
};
