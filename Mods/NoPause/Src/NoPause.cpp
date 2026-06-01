#include "NoPause.hpp"

#include <Logging.hpp>
#include <Glacier/ZString.hpp>

void NoPause::Init() {
    SDK()->Hooks()->GetApplicationOptionBool->AddDetour(this, &NoPause::GetOption);
    SDK()->Hooks()->ZApplicationEngineWin32_MainWindowProc->AddDetour(this, &NoPause::WndProc);
}

DEFINE_PLUGIN_DETOUR(NoPause, bool, GetOption, const ZString& p_OptionName, bool p_Default) {
    if (p_OptionName == "PauseOnFocusLoss") {
        return {HookAction::Return(), false};
    }

    if (p_OptionName == "NO_MINIMIZE_FOCUSLOSS") {
        return {HookAction::Return(), true};
    }

    return {HookAction::Continue()};
}

DEFINE_PLUGIN_DETOUR(NoPause, LRESULT, WndProc, ZApplicationEngineWin32* th, HWND p_Hwnd, UINT p_Message, WPARAM p_Wparam, LPARAM p_Lparam) {
    if (p_Message == WM_ACTIVATEAPP) {
        return {HookAction::Return(), DefWindowProcW(p_Hwnd, p_Message, p_Wparam, p_Lparam)};
    }

    return {HookAction::Continue()};
}

DEFINE_ZKNT_PLUGIN(NoPause)
