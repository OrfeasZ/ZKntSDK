#include "ModSDK.hpp"

#include <MinHook.h>

#include "HookImpl.hpp"
#include "IPluginInterface.hpp"
#include "Logging.hpp"
#include "ModLoader.hpp"
#include "UI/MainMenu.hpp"
#include "UI/ModSelector.hpp"
#include "Util/ProcessUtils.hpp"
#include <Globals.hpp>

extern void SetupLogging(spdlog::level::level_enum p_LogLevel);
extern void FlushLoggers();
extern void ClearLoggers();
extern void DispatchLog(spdlog::level::level_enum p_Level, std::string_view p_Msg);

extern "C" __declspec(dllexport) const char* SDKVersion() {
    return "0.1.0";
}

namespace zknt {
    ModSDK* ModSDK::g_Instance = nullptr;

    ModSDK* ModSDK::GetInstance() {
        if (g_Instance == nullptr) {
            g_Instance = new ModSDK();
        }

        return g_Instance;
    }

    void ModSDK::DestroyInstance() {
        if (g_Instance == nullptr) {
            return;
        }

        delete g_Instance;
        g_Instance = nullptr;
    }

    ModSDK::ModSDK() {
        HMODULE s_Module = GetModuleHandleA(nullptr);

        m_ModuleBase = reinterpret_cast<uintptr_t>(s_Module) + Util::ProcessUtils::GetBaseOfCode(s_Module);
        m_SizeOfCode = Util::ProcessUtils::GetSizeOfCode(s_Module);
        m_ImageSize = Util::ProcessUtils::GetSizeOfImage(s_Module);

        SetSDK(this);

#if _DEBUG
        SetupLogging(spdlog::level::trace);
#else
        SetupLogging(spdlog::level::info);
#endif
    }

    namespace {
        void Cb_SetSwapChain(IDXGISwapChain3* p_SwapChain) {
            if (auto* s_Sdk = ModSDK::GetInstance(); s_Sdk && s_Sdk->GetRenderer()) {
                static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetRenderer())->SetSwapChain(p_SwapChain);
            }
        }
        void Cb_SetCommandQueue(ID3D12CommandQueue* p_CommandQueue) {
            if (auto* s_Sdk = ModSDK::GetInstance(); s_Sdk && s_Sdk->GetRenderer()) {
                static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetRenderer())->SetCommandQueue(p_CommandQueue);
            }
        }
        void Cb_OnPresent(IDXGISwapChain3* p_SwapChain) {
            if (auto* s_Sdk = ModSDK::GetInstance(); s_Sdk && s_Sdk->GetRenderer()) {
                static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetRenderer())->OnPresent(p_SwapChain);
            }
        }
        void Cb_PostPresent(IDXGISwapChain3* p_SwapChain, HRESULT p_PresentResult) {
            if (auto* s_Sdk = ModSDK::GetInstance(); s_Sdk && s_Sdk->GetRenderer()) {
                static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetRenderer())->PostPresent(p_SwapChain, p_PresentResult);
            }
        }
        void Cb_OnReset(IDXGISwapChain3* p_SwapChain) {
            if (auto* s_Sdk = ModSDK::GetInstance(); s_Sdk && s_Sdk->GetRenderer()) {
                static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetRenderer())->OnReset(p_SwapChain);
            }
        }
        void Cb_PostReset(IDXGISwapChain3* p_SwapChain) {
            if (auto* s_Sdk = ModSDK::GetInstance(); s_Sdk && s_Sdk->GetRenderer()) {
                static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetRenderer())->PostReset(p_SwapChain);
            }
        }
        knt::host::WndProcResult Cb_OnWndProc(HWND p_Hwnd, UINT p_Msg, WPARAM p_Wparam, LPARAM p_Lparam) {
            if (auto* s_Sdk = ModSDK::GetInstance(); s_Sdk && s_Sdk->GetRenderer()) {
                const auto s_Result =
                    static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetRenderer())->OnWndProc(p_Hwnd, p_Msg, p_Wparam, p_Lparam);
                return {s_Result.m_Handled, s_Result.m_Value};
            }
            return {false, 0};
        }

        constexpr knt::host::RenderingCallbacks g_RenderingCallbacks{
            &Cb_SetSwapChain, &Cb_SetCommandQueue, &Cb_OnPresent, &Cb_PostPresent, &Cb_OnReset, &Cb_PostReset, &Cb_OnWndProc,
        };
    }

    ModSDK::~ModSDK() {
        Logger::Info("ModSDK is unloading...");

        // Tear mods down first; they may have registered hooks/callbacks.
        if (m_ModLoader) {
            m_ModLoader->UnloadAllMods();
        }

        m_ModSelector.reset();
        m_MainMenu.reset();
        m_ModLoader.reset();

        // Pull the renderer out of the proxy first; UnregisterRenderingCallbacks
        // drains in-flight dispatches before returning.
        if (m_HostServices && m_HostServices->UnregisterRenderingCallbacks) {
            m_HostServices->UnregisterRenderingCallbacks();
        }

        m_ImGuiRenderer.reset();

        // Close the hook gate and drain in-flight detours before tearing MinHook down.
        zknt::g_HookGate.BeginReload();

        HookRegistry::ClearAllDetours();
        HookRegistry::DestroyHooks();

        MH_Uninitialize();

        // The Hook<>* members of `m_Hooks` now point at freed memory (DestroyHooks
        // deletes them through HookBase); drop the owning unique_ptr to mirror.
        m_Hooks.reset();

        m_Functions.reset();
        m_Globals.reset();

        Trampolines::ClearTrampolines();

        FlushLoggers();
        ClearLoggers();
    }

    void ModSDK::Startup(knt::host::HostServices* p_HostServices) {
        if (m_StartedUp) {
            return;
        }

        m_StartedUp = true;
        m_HostServices = p_HostServices;

        if (m_HostServices && m_HostServices->IsEngineInitialized) {
            m_EngineInitialized = m_HostServices->IsEngineInitialized();
        }

        Logger::Info("SDK starting up.");

        m_Hooks = std::make_unique<zknt::Hooks>();
        m_Hooks->EnableAll();

        m_Globals = std::make_unique<zknt::Globals>();
        m_Functions = std::make_unique<zknt::Functions>();
        m_ImGuiRenderer = std::make_unique<zknt::rendering::ImGuiRenderer>();

        if (m_HostServices && m_HostServices->RegisterRenderingCallbacks) {
            m_HostServices->RegisterRenderingCallbacks(&g_RenderingCallbacks);
        }
        else {
            Logger::Warn("HostServices not provided -- ImGui will not render.");
        }

        m_ModSelector = std::make_unique<zknt::ui::ModSelector>();
        m_MainMenu = std::make_unique<zknt::ui::MainMenu>();
        m_ModLoader = std::make_unique<zknt::ModLoader>();
        m_ModLoader->Startup();
        m_ModSelector->UpdateAvailableMods(m_ModLoader->GetAvailableMods(), {}, m_ModLoader->GetActiveMods());

        // Single SDK-owned UI callback that fans out to the SDK chrome (main
        // menu + mod selector) and each loaded plugin's OnDrawUI. The main
        // menu itself is responsible for invoking per-plugin OnDrawMenu so
        // they're laid out inside its horizontal bar.
        RegisterUiCallback(this, [this](bool p_HasFocus) {
            if (m_MainMenu) {
                m_MainMenu->Draw(p_HasFocus);
            }
            if (m_ModSelector) {
                m_ModSelector->Draw(p_HasFocus);
            }
            if (m_ModLoader) {
                for (auto* s_Plugin : m_ModLoader->GetLoadedMods()) {
                    if (!s_Plugin) {
                        continue;
                    }
                    s_Plugin->OnDrawUI(p_HasFocus);
                }
            }
        });

        Hooks()->Engine_Init->AddDetour(this, &ModSDK::Engine_Init);

        if (m_EngineInitialized) {
            Logger::Info("Engine was already initialized before SDK load; replaying engine-init flow.");
            HandleEngineInitialized(false);
        }
    }

    zknt::Hooks* ModSDK::Hooks() {
        return m_Hooks.get();
    }

    zknt::Functions* ModSDK::Functions() {
        return m_Functions.get();
    }

    zknt::Globals* ModSDK::Globals() {
        return m_Globals.get();
    }

    void ModSDK::Log(spdlog::level::level_enum p_Level, std::string_view p_Msg) {
        DispatchLog(p_Level, p_Msg);
    }

    zknt::IRenderer* ModSDK::GetRenderer() const {
        return m_ImGuiRenderer.get();
    }

    ImGuiContext* ModSDK::GetImGuiContext() {
        return m_ImGuiRenderer ? m_ImGuiRenderer->GetImGuiContext() : nullptr;
    }

    ImFont* ModSDK::GetDefaultFont() const {
        return m_ImGuiRenderer ? m_ImGuiRenderer->GetDefaultFont() : nullptr;
    }

    zknt::ModLoader* ModSDK::GetModLoader() const {
        return m_ModLoader.get();
    }

    zknt::ui::ModSelector* ModSDK::GetUIModSelector() const {
        return m_ModSelector.get();
    }

    void ModSDK::OnModLoaded(const std::string& p_Name, IPluginInterface* p_Plugin, bool /*p_LiveLoad*/) const {
        Logger::Info("Mod '{}' loaded.", p_Name);

        p_Plugin->SetupUI();

        if (m_EngineInitialized && p_Plugin) {
            p_Plugin->OnEngineInitialized();
        }

        if (m_ModSelector && m_ModLoader) {
            m_ModSelector->UpdateAvailableMods(m_ModLoader->GetAvailableMods(), {}, m_ModLoader->GetActiveMods());
        }
    }

    void ModSDK::OnModUnloading(const std::string& p_Name, IPluginInterface* /*p_Plugin*/) {
        Logger::Info("Mod '{}' unloading.", p_Name);
    }

    void ModSDK::OnModUnloaded(const std::string& p_Name) const {
        Logger::Info("Mod '{}' unloaded.", p_Name);
        if (m_ModSelector && m_ModLoader) {
            m_ModSelector->UpdateAvailableMods(m_ModLoader->GetAvailableMods(), {}, m_ModLoader->GetActiveMods());
        }
    }

    void ModSDK::RegisterUiCallback(void* p_Token, UiCallback p_Callback) {
        std::lock_guard s_Lock(m_UiCallbacksMutex);
        m_UiCallbacks[p_Token] = std::move(p_Callback);
    }

    void ModSDK::UnregisterUiCallback(void* p_Token) {
        std::lock_guard s_Lock(m_UiCallbacksMutex);
        m_UiCallbacks.erase(p_Token);
    }

    void ModSDK::InvokeUiCallbacks(bool p_HasFocus) {
        // Snapshot under the lock so callbacks can (un)register safely.
        std::vector<UiCallback> s_Callbacks;
        {
            std::lock_guard s_Lock(m_UiCallbacksMutex);
            s_Callbacks.reserve(m_UiCallbacks.size());
            for (auto& [s_Token, s_Cb] : m_UiCallbacks) {
                s_Callbacks.push_back(s_Cb);
            }
        }
        for (auto& s_Cb : s_Callbacks) {
            if (s_Cb) {
                s_Cb(p_HasFocus);
            }
        }
    }

    void ModSDK::HandleEngineInitialized(bool p_SyncHostState) {
        if (!m_EngineInitialized) {
            m_EngineInitialized = true;
            if (p_SyncHostState && m_HostServices && m_HostServices->SetEngineInitialized) {
                m_HostServices->SetEngineInitialized(true);
            }
        }

        if (m_ModLoader) {
            for (auto* s_Plugin : m_ModLoader->GetLoadedMods()) {
                if (s_Plugin) {
                    s_Plugin->OnEngineInitialized();
                }
            }
        }
    }

    DEFINE_DETOUR_WITH_CONTEXT(ModSDK, bool, Engine_Init, void* th, void* a2) {
        auto s_Result = p_Hook->CallOriginal(th, a2);

        Logger::Info("Engine has initialized!");
        HandleEngineInitialized(true);

        return {HookAction::Return(), s_Result};
    }

}
