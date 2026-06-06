#pragma once

#include "Common.hpp"
#include "Hook.hpp"

class ZString;
class ZApplicationEngineWin32;
class ZEngineAppCommon;
class ZFreeCameraControlEntity;
class ZObjectRef;
class ZEntityRef;
class ZEntityType;

namespace zknt {
    class ModSDK;

    class Hooks {
      public:
        Hooks();
        ~Hooks() = default;

        Hooks(const Hooks&) = delete;
        Hooks& operator=(const Hooks&) = delete;

        Hook<bool(void*, void*)>* Engine_Init = nullptr;
        Hook<bool(const ZString& optionName, bool defaultValue)>* GetApplicationOptionBool = nullptr;
        Hook<LRESULT(ZApplicationEngineWin32*, HWND, UINT, WPARAM, LPARAM)>* ZApplicationEngineWin32_MainWindowProc = nullptr;
        Hook<ZString*(ZEngineAppCommon* th, ZString& result)>* ZEngineAppCommon_GetBootScene = nullptr;
        Hook<ZString*(ZFreeCameraControlEntity* th, ZString& result, int nControllerId)>* ZFreeCameraControlEntity_GenerateActionBindingString;
        Hook<void(ZFreeCameraControlEntity* th)>* ZFreeCameraControlEntity_UpdateMovementFromInput;
        Hook<void(ZFreeCameraControlEntity* th, float dt)>* ZFreeCameraControlEntity_UpdateCamera;
        Hook<void(ZEntityType**, uint32_t, const ZObjectRef&)>* SignalOutputPin;
        Hook<void(ZEntityType**, uint32_t, const ZObjectRef&)>* SignalInputPin;
        Hook<bool(void*, void*)>* Check_SSL_Cert = nullptr;

      private:
        void EnableAll();

        friend class ModSDK;
    };
}
