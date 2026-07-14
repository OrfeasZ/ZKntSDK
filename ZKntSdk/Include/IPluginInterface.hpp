#pragma once

#include "Common.hpp"
#include "Hooks.hpp"
#include "IModSDK.hpp"
#include "IImGuiRenderer.hpp"
#include "IDirectXTKRenderer.hpp"

#include <cr.h>
#include <imgui.h>

namespace zknt {
    class IPluginInterface {
      public:
        virtual ~IPluginInterface() = default;

      private:
        virtual void SetupUI(IImGuiRenderer* p_Renderer) {
            auto* s_Context = p_Renderer->GetContext();

            if (!s_Context) {
                return;
            }

            ImGui::SetCurrentContext(s_Context);
            ImGui::SetAllocatorFunctions(p_Renderer->GetMemAlloc(), p_Renderer->GetMemFree(), p_Renderer->GetAllocatorUserData());
        }

      public:
        // Invoked on the game thread.
        virtual void Init() {}

        // Invoked on the game thread.
        virtual void OnEngineInitialized() {}

        // Invoked on the render thread.
        virtual void OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus) {}

        virtual void OnDrawMenu(IImGuiRenderer* p_Renderer) {}

        // Invoked on the render thread.
        virtual void OnDraw3D(IDirectXTKRenderer* p_Renderer) {}

        /**
         * Draw with a depth buffer active, so drawn objects can be occluded by objects in the scene.
         *
         * Invoked on the render thread.
         */
        virtual void OnDepthDraw3D(IDirectXTKRenderer* p_Renderer) {}

        // Invoked on the render thread.
        virtual void OnDraw2D(IDirectXTKRenderer* p_Renderer) {}

      private:
        virtual void CleanupUI() {
            ImGui::SetCurrentContext(nullptr);
        }

      public:
        /**
         * Set a setting string value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        void SetSetting(const ZString& p_Section, const ZString& p_Name, const ZString& p_Value) {
            SDK()->SetPluginSetting(this, p_Section, p_Name, p_Value);
        }

        /**
         * Set a setting integer value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        void SetSettingInt(const ZString& p_Section, const ZString& p_Name, int64_t p_Value) {
            SDK()->SetPluginSettingInt(this, p_Section, p_Name, p_Value);
        }

        /**
         * Set a setting unsigned integer value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        void SetSettingUInt(const ZString& p_Section, const ZString& p_Name, uint64_t p_Value) {
            SDK()->SetPluginSettingUInt(this, p_Section, p_Name, p_Value);
        }

        /**
         * Set a setting double value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        void SetSettingDouble(const ZString& p_Section, const ZString& p_Name, double p_Value) {
            SDK()->SetPluginSettingDouble(this, p_Section, p_Name, p_Value);
        }

        /**
         * Set a setting boolean value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        void SetSettingBool(const ZString& p_Section, const ZString& p_Name, bool p_Value) {
            SDK()->SetPluginSettingBool(this, p_Section, p_Name, p_Value);
        }

        /**
         * Get a setting string value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to use if the setting does not exist.
         * @param p_OutValue Receives the value of the setting, or p_DefaultValue if the setting does not exist.
         */
        void GetSetting(const ZString& p_Section, const ZString& p_Name, const ZString& p_DefaultValue, ZString& p_OutValue) {
            SDK()->GetPluginSetting(this, p_Section, p_Name, p_DefaultValue, p_OutValue);
        }

        /**
         * Get a setting integer value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to return if the setting does not exist or is not an integer.
         * @return The value of the setting, or the default value if the setting does not exist or is not an integer.
         */
        int64_t GetSettingInt(const ZString& p_Section, const ZString& p_Name, int64_t p_DefaultValue) {
            return SDK()->GetPluginSettingInt(this, p_Section, p_Name, p_DefaultValue);
        }

        /**
         * Get a setting unsigned integer value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to return if the setting does not exist or is not an unsigned integer.
         * @return The value of the setting, or the default value if the setting does not exist or is not an unsigned integer.
         */
        uint64_t GetSettingUInt(const ZString& p_Section, const ZString& p_Name, uint64_t p_DefaultValue) {
            return SDK()->GetPluginSettingUInt(this, p_Section, p_Name, p_DefaultValue);
        }

        /**
         * Get a setting double value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to return if the setting does not exist or is not a double.
         * @return The value of the setting, or the default value if the setting does not exist or is not a double.
         */
        double GetSettingDouble(const ZString& p_Section, const ZString& p_Name, double p_DefaultValue) {
            return SDK()->GetPluginSettingDouble(this, p_Section, p_Name, p_DefaultValue);
        }

        /**
         * Get a setting boolean value for the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to return if the setting does not exist or is not a boolean.
         * @return The value of the setting, or the default value if the setting does not exist or is not a boolean.
         */
        bool GetSettingBool(const ZString& p_Section, const ZString& p_Name, bool p_DefaultValue) {
            return SDK()->GetPluginSettingBool(this, p_Section, p_Name, p_DefaultValue);
        }

        /**
         * Check if a setting with the given name exists.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @return True if the setting exists, false otherwise.
         */
        bool HasSetting(const ZString& p_Section, const ZString& p_Name) {
            return SDK()->HasPluginSetting(this, p_Section, p_Name);
        }

        /**
         * Remove a setting with the given name.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         */
        void RemoveSetting(const ZString& p_Section, const ZString& p_Name) {
            SDK()->RemovePluginSetting(this, p_Section, p_Name);
        }

        /**
         * Reload the settings for the current plugin.
         */
        void ReloadSettings() {
            SDK()->ReloadPluginSettings(this);
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
