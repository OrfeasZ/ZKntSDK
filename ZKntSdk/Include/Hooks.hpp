#pragma once

#include "Common.hpp"
#include "Hook.hpp"

class ZString;
class ZApplicationEngineWin32;
class ZEngineAppCommon;

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

      private:
        void EnableAll();

        friend class ModSDK;
    };
}
