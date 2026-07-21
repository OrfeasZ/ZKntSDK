#pragma once

#include "IModSDK.hpp"
#include "Hooks.hpp"
#include "Functions.hpp"
#include "Globals.hpp"
#include "Rendering/ImGuiRenderer.hpp"
#include "Rendering/DirectXTKRenderer.hpp"
#include "HostServices.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

class ZResourcePending;

struct ResourceMem;

namespace zknt {
    class ModLoader;
    class IPluginInterface;
}

namespace zknt::ui {
    class ModSelector;
    class MainMenu;
}

namespace zknt {
    class ModSDK : public IModSDK {
      private:
        static ModSDK* g_Instance;

      public:
        static ModSDK* GetInstance();
        static void DestroyInstance();

      public:
        ModSDK();
        ~ModSDK();

        void Startup(knt::host::HostServices* p_HostServices);

        [[nodiscard]] uintptr_t GetModuleBase() const {
            return m_ModuleBase;
        }

        [[nodiscard]] uint32_t GetSizeOfCode() const {
            return m_SizeOfCode;
        }

        [[nodiscard]] uint32_t GetImageSize() const {
            return m_ImageSize;
        }

        // Internal: called by ImGuiRenderer::Draw().
        void InvokeUiCallbacks(bool p_HasFocus);

        // IModSDK (mod-facing) methods.
        ::zknt::Hooks* Hooks() override;
        ::zknt::Functions* Functions() override;
        ::zknt::Globals* Globals() override;

        void Log(spdlog::level::level_enum p_Level, std::string_view p_Msg) override;

        bool PatchCode(const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_Offsp_TargetOffsetet) override;
        bool PatchCodeStoreOriginal(
            const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset, void* p_OriginalCode
        ) override;

        void AllocateZString(ZString* p_Target, const char* p_Str, uint32_t p_Size) override;
        void FreeZString(ZString* p_Target) override;

        bool LoadCppEntity(
            const ZString& p_BlueprintJson, const ZString& p_BlueprintMetaJson, const ZString& p_EntityJson, const ZString& p_EntityMetaJson,
            TResourcePtr<ZCppEntityBlueprintFactory>& p_OutBlueprintFactory, TResourcePtr<ZCppEntityFactory>& p_OutTemplateFactory
        ) override;

        // Plugin settings
        void SetPluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, const ZString& p_Value) override;

        void SetPluginSettingInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, int64_t p_Value) override;

        void SetPluginSettingUInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, uint64_t p_Value) override;

        void SetPluginSettingDouble(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, double p_Value) override;

        void SetPluginSettingBool(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, bool p_Value) override;

        void GetPluginSetting(
            IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, const ZString& p_DefaultValue, ZString& p_OutValue
        ) override;

        int64_t GetPluginSettingInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, int64_t p_DefaultValue) override;

        uint64_t GetPluginSettingUInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, uint64_t p_DefaultValue) override;

        double GetPluginSettingDouble(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, double p_DefaultValue) override;

        bool GetPluginSettingBool(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, bool p_DefaultValue) override;

        bool HasPluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name) override;
        void RemovePluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name) override;
        void ReloadPluginSettings(IPluginInterface* p_Plugin) override;

        bool WorldToScreen(const SVector3& p_WorldPos, SVector2& p_Out) override;
        bool ScreenToWorld(const SVector2& p_ScreenPos, SVector3& p_OutWorldPos, SVector3& p_OutDirection) override;

        SMatrix GetViewMatrix() const override;
        SMatrix GetProjectionMatrix() const override;

        // SDK-internal methods.
        IImGuiRenderer* GetImGuiRenderer() const;
        IDirectXTKRenderer* GetDirectXTKRenderer() const;
        ModLoader* GetModLoader() const;
        ui::ModSelector* GetUIModSelector() const;
        [[nodiscard]] bool IsEngineInitialized() const {
            return m_EngineInitialized;
        }
        bool PatchCodeInternal(
            const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset, void* p_OriginalCode
        );

        // SDK-internal UI callback registry.
        using UiCallback = std::function<void(bool p_HasFocus)>;
        void RegisterUiCallback(void* p_Token, UiCallback p_Callback);
        void UnregisterUiCallback(void* p_Token);

        // Called by ModLoader when a mod completes registration.
        void OnModLoaded(const std::string& p_Name, IPluginInterface* p_Plugin, bool p_LiveLoad) const;
        void OnModUnloading(const std::string& p_Name, IPluginInterface* p_Plugin);
        void OnModUnloaded(const std::string& p_Name) const;

      private:
        void HandleEngineInitialized(bool p_SyncHostState);

      private:
        static std::tuple<ZResourceIndex, ZRuntimeResourceID>
        LoadResourceFromBIN1(ResourceMem* p_ResourceMem, std::string_view p_MetaJson, std::function<void(ZResourcePending*)> p_Install);

      private:
        // Detours
        DECLARE_DETOUR_WITH_CONTEXT(ModSDK, bool, Engine_Init, void* th, void* a2);
        DECLARE_DETOUR_WITH_CONTEXT(ModSDK, void, SPassExecution_ExecutePass, SPassExecution* th, int32_t renderDeviceContextIndex);

      private:
        uintptr_t m_ModuleBase;
        uint32_t m_SizeOfCode;
        uint32_t m_ImageSize;

        bool m_StartedUp = false;
        bool m_EngineInitialized = false;

        knt::host::HostServices* m_HostServices = nullptr;
        std::unique_ptr<::zknt::Hooks> m_Hooks;
        std::unique_ptr<::zknt::Functions> m_Functions;
        std::unique_ptr<::zknt::Globals> m_Globals;
        std::unique_ptr<rendering::ImGuiRenderer> m_ImGuiRenderer;
        std::shared_ptr<rendering::DirectXTKRenderer> m_DirectXTKRenderer;
        std::unique_ptr<ModLoader> m_ModLoader;
        std::unique_ptr<ui::ModSelector> m_ModSelector;
        std::unique_ptr<ui::MainMenu> m_MainMenu;

        std::mutex m_UiCallbacksMutex;
        std::unordered_map<void*, UiCallback> m_UiCallbacks;
    };
}
