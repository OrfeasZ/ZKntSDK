#pragma once

#include "IModSDK.hpp"
#include "Hooks.hpp"
#include "Functions.hpp"
#include "Globals.hpp"
#include "Rendering/ImGuiRenderer.hpp"
#include "HostServices.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

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
        ImGuiContext* GetImGuiContext() override;
        ::zknt::Hooks* Hooks() override;
        ::zknt::Functions* Functions() override;
        ::zknt::Globals* Globals() override;
        void Log(spdlog::level::level_enum p_Level, std::string_view p_Msg) override;

        // SDK-internal methods.
        IRenderer* GetRenderer() const;
        ImFont* GetDefaultFont() const;
        ModLoader* GetModLoader() const;
        ui::ModSelector* GetUIModSelector() const;

        // SDK-internal UI callback registry.
        using UiCallback = std::function<void(bool p_HasFocus)>;
        void RegisterUiCallback(void* p_Token, UiCallback p_Callback);
        void UnregisterUiCallback(void* p_Token);

        // Called by ModLoader when a mod completes registration.
        void OnModLoaded(const std::string& p_Name, IPluginInterface* p_Plugin, bool p_LiveLoad) const;
        void OnModUnloading(const std::string& p_Name, IPluginInterface* p_Plugin);
        void OnModUnloaded(const std::string& p_Name) const;

      private:
        // Detours
        DECLARE_DETOUR_WITH_CONTEXT(ModSDK, bool, Engine_Init, void* th, void* a2);

      private:
        uintptr_t m_ModuleBase;
        uint32_t m_SizeOfCode;
        uint32_t m_ImageSize;

        bool m_StartedUp = false;

        knt::host::HostServices* m_HostServices = nullptr;
        std::unique_ptr<::zknt::Hooks> m_Hooks;
        std::unique_ptr<::zknt::Functions> m_Functions;
        std::unique_ptr<::zknt::Globals> m_Globals;
        std::unique_ptr<rendering::ImGuiRenderer> m_ImGuiRenderer;
        std::unique_ptr<ModLoader> m_ModLoader;
        std::unique_ptr<ui::ModSelector> m_ModSelector;
        std::unique_ptr<ui::MainMenu> m_MainMenu;

        std::mutex m_UiCallbacksMutex;
        std::unordered_map<void*, UiCallback> m_UiCallbacks;
    };
}
