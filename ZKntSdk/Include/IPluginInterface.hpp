#pragma once

#include "Common.hpp"
#include "Hooks.hpp"
#include "IModSDK.hpp"

#include <cr.h>
#include <imgui.h>

namespace zknt {
    class IPluginInterface {
      public:
        virtual ~IPluginInterface() = default;

      private:
        virtual void SetupUI() {
            auto* s_Context = SDK()->GetImGuiContext();

            if (!s_Context) {
                return;
            }

            ImGui::SetCurrentContext(s_Context);
            ImGui::SetAllocatorFunctions(SDK()->GetImGuiAlloc(), SDK()->GetImGuiFree(), SDK()->GetImGuiAllocatorUserData());
        }

      public:
        // Invoked on the game thread.
        virtual void Init() {}

        // Invoked on the game thread.
        virtual void OnEngineInitialized() {}

        // Invoked on the render thread.
        virtual void OnDrawUI(bool p_HasFocus) {}

        virtual void OnDrawMenu() {}

      private:
        virtual void CleanupUI() {
            ImGui::SetCurrentContext(nullptr);
        }

        friend class ModSDK;
    };
}

namespace knt::mod {
    struct HostServices {
        zknt::IModSDK* m_SDK;
        void (*RegisterPlugin)(void* p_Token, zknt::IPluginInterface* p_Plugin);
        void (*UnregisterPlugin)(void* p_Token);
    };
}

#define DECLARE_ZKNT_PLUGIN(PluginClass) inline PluginClass* Plugin();

#define DEFINE_ZKNT_PLUGIN(PluginClass)                                                                                      \
    static PluginClass* g_##PluginClass##_Instance = nullptr;                                                                \
                                                                                                                             \
    inline PluginClass* Plugin() {                                                                                           \
        return g_##PluginClass##_Instance;                                                                                   \
    }                                                                                                                        \
                                                                                                                             \
    extern "C" __declspec(dllexport) int cr_main(struct cr_plugin* p_Ctx, enum cr_op p_Op) {                                 \
        auto* s_Services = static_cast<knt::mod::HostServices*>(p_Ctx && p_Ctx->userdata ? p_Ctx->userdata : nullptr);       \
        switch (p_Op) {                                                                                                      \
        case CR_LOAD:                                                                                                        \
            /* Publish the SDK pointer into this mod's per-DLL slot so `SDK()` resolves correctly from any TU in the mod. */ \
            if (s_Services) {                                                                                                \
                SetSDK(s_Services->m_SDK);                                                                                   \
            }                                                                                                                \
            if (!g_##PluginClass##_Instance) {                                                                               \
                g_##PluginClass##_Instance = new PluginClass();                                                              \
            }                                                                                                                \
            if (s_Services && s_Services->RegisterPlugin) {                                                                  \
                s_Services->RegisterPlugin(&g_##PluginClass##_Instance, g_##PluginClass##_Instance);                         \
            }                                                                                                                \
            return 0;                                                                                                        \
        case CR_UNLOAD:                                                                                                      \
        case CR_CLOSE:                                                                                                       \
            if (s_Services && s_Services->UnregisterPlugin) {                                                                \
                s_Services->UnregisterPlugin(&g_##PluginClass##_Instance);                                                   \
            }                                                                                                                \
            delete g_##PluginClass##_Instance;                                                                               \
            g_##PluginClass##_Instance = nullptr;                                                                            \
            SetSDK(nullptr);                                                                                                 \
            return 0;                                                                                                        \
        default:                                                                                                             \
            return 0;                                                                                                        \
        }                                                                                                                    \
    }

// Detour helpers re-exported for parity with ZHMModSDK plugins.
#define DECLARE_PLUGIN_DETOUR(PluginClass, ReturnType, DetourName, ...) DECLARE_DETOUR_WITH_CONTEXT(PluginClass, ReturnType, DetourName, __VA_ARGS__)
#define DEFINE_PLUGIN_DETOUR(PluginClass, ReturnType, DetourName, ...) DEFINE_DETOUR_WITH_CONTEXT(PluginClass, ReturnType, DetourName, __VA_ARGS__)
