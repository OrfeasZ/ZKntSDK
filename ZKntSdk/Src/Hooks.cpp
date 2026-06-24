#include "Hooks.hpp"
#include "HookImpl.hpp"
#include "IModSDK.hpp"

#include <MinHook.h>

#include <Glacier/ZString.hpp>
#include <Glacier/ZEntity.hpp>

std::unordered_set<HookBase*>* HookRegistry::g_Hooks = nullptr;

DetourTrampoline* Trampolines::g_Trampolines = nullptr;
size_t Trampolines::g_TrampolineCount = 0;

zknt::Hooks::Hooks() {
    PATTERN_HOOK("\x48\x89\x54\x24\x10\x55\x56\x48\x8D\xAC\x24\xB8\xFE\xFF\xFF", "xxxxxxxxxxxxxxx", Engine_Init, bool(void*, void*));

    PATTERN_HOOK(
        "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x00\x0F\xB6\xFA\x48\x8B\xD9\xE8\x00\x00\x00\x00\x48\x8B\xD3", "xxxxxxxxx?xxxxxxx????xxx",
        GetApplicationOptionBool, bool(const ZString&, bool)
    );

    PATTERN_HOOK(
        "\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\x74\x24\x18\x48\x89\x7C\x24\x20\x41\x54\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48"
        "\x8B\xF9",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxx", ZApplicationEngineWin32_MainWindowProc, LRESULT(ZApplicationEngineWin32*, HWND, UINT, WPARAM, LPARAM)
    );

    PATTERN_RELATIVE_CALL_HOOK(
        "\xE8\x00\x00\x00\x00\x48\x8B\x0F\x48\x8D\x05\x00\x00\x00\x00\x4C\x8D\x44\x24\x40", "x????xxxxxx????xxxxx", ZEngineAppCommon_GetBootScene,
        ZString * (ZEngineAppCommon * th, ZString & result)
    );

    PATTERN_HOOK(
        "\x44\x89\x44\x24\x18\x48\x89\x54\x24\x10\x48\x89\x4C\x24\x08\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xC8\x48\x81\xEC"
        "\x00\x00\x00\x00\x48\x8D\x4C\x24\x70",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxx", ZFreeCameraControlEntity_GenerateActionBindingString,
        ZString * (ZFreeCameraControlEntity * th, ZString & result, int32_t nControllerId)
    );

    PATTERN_HOOK(
        "\x40\x53\x56\x57\x48\x81\xEC\x00\x00\x00\x00\x33\xC0", "xxxxxxx????xx", ZFreeCameraControlEntity_UpdateMovementFromInput,
        void(ZFreeCameraControlEntity * th)
    );

    PATTERN_HOOK(
        "\x48\x8B\xC4\x53\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\xD9\x48\x8B\x49\x40", "xxxxxxx????xxxxxxx", ZFreeCameraControlEntity_UpdateCamera,
        void(ZFreeCameraControlEntity * th, float dt)
    );

    PATTERN_HOOK(
        "\x48\x89\x74\x24\x20\x41\x56\x48\x83\xEC\x00\x48\x8B\x01\x49\x8B\xF0\x4C\x8B\xF1\x48\x8B\x40\x38", "xxxxxxxxxx?xxxxxxxxxxxxx",
        SignalOutputPin, void(ZEntityType**, uint32_t, const ZObjectRef&)
    );

    PATTERN_HOOK(
        "\x48\x89\x6C\x24\x18\x56\x48\x83\xEC\x00\x48\x8B\x01\x49\x8B\xE8", "xxxxxxxxx?xxxxxx", SignalInputPin,
        void(ZEntityType**, uint32_t, const ZObjectRef&)
    );

    PATTERN_HOOK(
        "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x18\x48\x89\x7C\x24\x20\x48\x89\x54\x24\x10\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\xC9\x48"
        "\x81\xEC\x00\x00\x00\x00\x33\xFF",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xx", ZFreeCameraControlEditorStyleEntity_GenerateActionBindingString,
        ZString * (ZFreeCameraControlEditorStyleEntity * th, ZString & result, int32_t nControllerId)
    );

    PATTERN_HOOK(
        "\x40\x53\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\xD9\x84\xD2", "xxxxx????xxxxx", ZFreeCameraControlEditorStyleEntity_HandleDrag,
        void(ZFreeCameraControlEditorStyleEntity* const th, bool bRotationIsActive, bool bObjectHookIsActive, bool bIsOrbitActive)
    );

    PATTERN_HOOK(
        "\x4C\x8B\xDC\x53\x48\x81\xEC\x00\x00\x00\x00\x48\x83\x79\x50", "xxxxxxx????xxxx", ZFreeCameraControlEditorStyleEntity_MoveCameraWithKey,
        void(ZFreeCameraControlEditorStyleEntity * th, float fDeltaTime)
    );

    PATTERN_HOOK(
        "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x00\x48\x8D\x05\x00\x00\x00\x00\x48\x8B\xF9\x48\x89\x01\x8B\xDA\x48\x81\xC1\x00\x00\x00\x00\xE8\x00\x00"
        "\x00\x00\x48\x8D\x8F\x98\x00\x00\x00\xE8",
        "xxxxxxxxx?xxx????xxxxxxxxxxx????x????xxxxxxxx", ZKntLoadoutCollectionEntity_ZKntLoadoutCollectionEntity,
        ZKntLoadoutCollectionEntity * (ZKntLoadoutCollectionEntity * th, bool unk)
    );
}

void zknt::Hooks::EnableAll() {
    const auto s_Result = MH_EnableHook(MH_ALL_HOOKS);
    if (s_Result != MH_OK) {
        Logger::Error("Failed to enable hooks. Error code: {}.", static_cast<int>(s_Result));
    }
}
