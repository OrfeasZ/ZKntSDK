#pragma once

#include "Common.hpp"
#include <spdlog/spdlog.h>
#include <string_view>

struct ImGuiContext;

namespace zknt {
    class Hooks;
    class Functions;
    class Globals;

    class IModSDK {
      public:
        virtual ~IModSDK() = default;

        virtual ImGuiContext* GetImGuiContext() = 0;

        virtual zknt::Hooks* Hooks() = 0;
        virtual zknt::Functions* Functions() = 0;
        virtual zknt::Globals* Globals() = 0;

        virtual void Log(spdlog::level::level_enum p_Level, std::string_view p_Msg) = 0;
    };
}

// Per-DLL pointer to the active SDK instance. The SDK's own startup path
// sets it from inside `ZKntSdk.dll`; each mod sets its own copy in
// `cr_main(CR_LOAD)` from `HostServices::m_SDK`. `inline` guarantees each
// linkage unit (DLL) gets its own storage, so the pointer is never
// imported across modules.
inline zknt::IModSDK*& g_SDKInstance() {
    static zknt::IModSDK* s_Instance = nullptr;
    return s_Instance;
}

inline zknt::IModSDK* SDK() {
    return g_SDKInstance();
}

inline void SetSDK(zknt::IModSDK* p_Sdk) {
    g_SDKInstance() = p_Sdk;
}
