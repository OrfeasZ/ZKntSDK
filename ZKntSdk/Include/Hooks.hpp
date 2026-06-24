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
class ZFreeCameraControlEditorStyleEntity;
class ZKntLoadoutCollectionEntity;

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
        Hook<ZString*(ZFreeCameraControlEntity* th, ZString& result, int32_t nControllerId)>* ZFreeCameraControlEntity_GenerateActionBindingString =
            nullptr;
        Hook<void(ZFreeCameraControlEntity* th)>* ZFreeCameraControlEntity_UpdateMovementFromInput = nullptr;
        Hook<void(ZFreeCameraControlEntity* th, float dt)>* ZFreeCameraControlEntity_UpdateCamera = nullptr;
        Hook<void(ZEntityType**, uint32_t, const ZObjectRef&)>* SignalOutputPin = nullptr;
        Hook<void(ZEntityType**, uint32_t, const ZObjectRef&)>* SignalInputPin = nullptr;
        Hook<ZString*(ZFreeCameraControlEditorStyleEntity* th, ZString& result, int32_t nControllerId)>*
            ZFreeCameraControlEditorStyleEntity_GenerateActionBindingString = nullptr;
        Hook<void(ZFreeCameraControlEditorStyleEntity* const th, bool bRotationIsActive, bool bObjectHookIsActive, bool bIsOrbitActive)>*
            ZFreeCameraControlEditorStyleEntity_HandleDrag = nullptr;
        Hook<void(ZFreeCameraControlEditorStyleEntity* th, float fDeltaTime)>* ZFreeCameraControlEditorStyleEntity_MoveCameraWithKey = nullptr;
        Hook<ZKntLoadoutCollectionEntity*(ZKntLoadoutCollectionEntity* th, bool unk)>* ZKntLoadoutCollectionEntity_ZKntLoadoutCollectionEntity =
            nullptr;

      private:
        void EnableAll();

        friend class ModSDK;
    };
}
