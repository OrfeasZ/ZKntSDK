#include "ModSDK.hpp"

#include <MinHook.h>

#include <Glacier/ZModule.hpp>
#include <Glacier/ZResource.hpp>
#include <Glacier/ZRender.hpp>
#include <Glacier/ZConfigCommand.h>

#include "HookImpl.hpp"
#include "IPluginInterface.hpp"
#include "Logging.hpp"
#include "ModLoader.hpp"
#include "UI/MainMenu.hpp"
#include "UI/ModSelector.hpp"
#include "UI/Console.hpp"
#include "Util/ProcessUtils.hpp"
#include "Globals.hpp"
#include "Events.hpp"

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

        LoadConfiguration();

#if _DEBUG
        SetupLogging(spdlog::level::trace);
#else
        SetupLogging(spdlog::level::info);
#endif
    }

    namespace {
        void Cb_SetSwapChain(IDXGISwapChain3* p_SwapChain) {
            auto* s_Sdk = ModSDK::GetInstance();

            if (s_Sdk) {
                if (s_Sdk->GetImGuiRenderer()) {
                    static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetImGuiRenderer())->SetSwapChain(p_SwapChain);
                }

                if (s_Sdk->GetDirectXTKRenderer()) {
                    static_cast<zknt::rendering::DirectXTKRenderer*>(s_Sdk->GetDirectXTKRenderer())->SetSwapChain(p_SwapChain);
                }
            }
        }

        void Cb_SetCommandQueue(ID3D12CommandQueue* p_CommandQueue) {
            auto* s_Sdk = ModSDK::GetInstance();

            if (s_Sdk) {
                if (s_Sdk->GetImGuiRenderer()) {
                    static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetImGuiRenderer())->SetCommandQueue(p_CommandQueue);
                }

                if (s_Sdk->GetDirectXTKRenderer()) {
                    static_cast<zknt::rendering::DirectXTKRenderer*>(s_Sdk->GetDirectXTKRenderer())->SetCommandQueue(p_CommandQueue);
                }
            }
        }

        void Cb_OnPresent(IDXGISwapChain3* p_SwapChain) {
            auto* s_Sdk = ModSDK::GetInstance();

            if (s_Sdk) {
                if (s_Sdk->GetImGuiRenderer()) {
                    static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetImGuiRenderer())->OnPresent(p_SwapChain);
                }

                if (s_Sdk->GetDirectXTKRenderer()) {
                    static_cast<zknt::rendering::DirectXTKRenderer*>(s_Sdk->GetDirectXTKRenderer())->OnPresent(p_SwapChain);
                }
            }
        }

        void Cb_PostPresent(IDXGISwapChain3* p_SwapChain, HRESULT p_PresentResult) {
            auto* s_Sdk = ModSDK::GetInstance();

            if (s_Sdk) {
                if (s_Sdk->GetImGuiRenderer()) {
                    static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetImGuiRenderer())->PostPresent(p_SwapChain, p_PresentResult);
                }

                if (s_Sdk->GetDirectXTKRenderer()) {
                    static_cast<zknt::rendering::DirectXTKRenderer*>(s_Sdk->GetDirectXTKRenderer())->PostPresent(p_SwapChain, p_PresentResult);
                }
            }
        }

        void Cb_OnReset(IDXGISwapChain3* p_SwapChain) {
            auto* s_Sdk = ModSDK::GetInstance();

            if (s_Sdk) {
                if (s_Sdk->GetImGuiRenderer()) {
                    static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetImGuiRenderer())->OnReset(p_SwapChain);
                }

                if (s_Sdk->GetDirectXTKRenderer()) {
                    static_cast<zknt::rendering::DirectXTKRenderer*>(s_Sdk->GetDirectXTKRenderer())->OnReset(p_SwapChain);
                }
            }
        }

        void Cb_PostReset(IDXGISwapChain3* p_SwapChain) {
            auto* s_Sdk = ModSDK::GetInstance();

            if (s_Sdk) {
                if (s_Sdk->GetImGuiRenderer()) {
                    static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetImGuiRenderer())->PostReset(p_SwapChain);
                }

                if (s_Sdk->GetDirectXTKRenderer()) {
                    static_cast<zknt::rendering::DirectXTKRenderer*>(s_Sdk->GetDirectXTKRenderer())->PostReset(p_SwapChain);
                }
            }
        }

        knt::host::WndProcResult Cb_OnWndProc(HWND p_Hwnd, UINT p_Msg, WPARAM p_Wparam, LPARAM p_Lparam) {
            if (auto* s_Sdk = ModSDK::GetInstance(); s_Sdk && s_Sdk->GetImGuiRenderer()) {
                const auto s_Result =
                    static_cast<zknt::rendering::ImGuiRenderer*>(s_Sdk->GetImGuiRenderer())->OnWndProc(p_Hwnd, p_Msg, p_Wparam, p_Lparam);

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
        m_DirectXTKRenderer.reset();

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
        m_Events.reset();

        Trampolines::ClearTrampolines();

        FlushLoggers();
        ClearLoggers();
    }

    void ModSDK::Startup(knt::host::HostServices* p_HostServices) {
        if (m_StartedUp) {
            return;
        }

        m_Console = std::make_unique<zknt::ui::Console>();

        Logger::Info("Applying startup patches...");

        uint8_t s_Nop[0x59] = {};
        memset(s_Nop, 0x90, sizeof(s_Nop));

        if (!SDK()->PatchCode("\x48\x85\xFF\x74\x00\x80\xB9\xD0\x01\x00\x00", "xxxx?xxxxxx", s_Nop, sizeof(s_Nop), 0)) {
            Logger::Error("Could not patch ZTemplateEntityBlueprintFactory data freeing.");
        }

        uint8_t s_Nop2[0xB] = {};
        memset(s_Nop2, 0x90, sizeof(s_Nop2));

        if (!SDK()->PatchCode("\x48\xC7\x81\xC8\x01\x00\x00\x00\x00\x00\x00\x48\x83\xC4", "xxxxxxx????xxx", s_Nop2, sizeof(s_Nop2), 0)) {
            Logger::Error("Could not patch ZTemplateEntityBlueprintFactory data freeing.");
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
        m_Events = std::make_unique<zknt::Events>();
        m_ImGuiRenderer = std::make_unique<zknt::rendering::ImGuiRenderer>();
        m_DirectXTKRenderer = std::make_unique<zknt::rendering::DirectXTKRenderer>();

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
        RegisterUICallback(this, [this](bool p_HasFocus) {
            if (m_MainMenu) {
                m_MainMenu->Draw(p_HasFocus);
            }

            if (m_ModSelector) {
                m_ModSelector->Draw(p_HasFocus);
            }

            if (m_Console) {
                m_Console->Draw(GetImGuiRenderer(), p_HasFocus);
            }

            if (m_ModLoader) {
                for (auto* s_Plugin : m_ModLoader->GetLoadedMods()) {
                    if (!s_Plugin) {
                        continue;
                    }
                    s_Plugin->OnDrawUI(m_ImGuiRenderer.get(), p_HasFocus);
                }
            }
        });

        Hooks()->Engine_Init->AddDetour(this, &ModSDK::Engine_Init);
        Hooks()->SPassExecution_ExecutePass->AddDetour(this, &ModSDK::SPassExecution_ExecutePass);

        Events()->OnConsoleCommand->AddListener(this, &OnConsoleCommand);

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

    ::zknt::Events* ModSDK::Events() {
        return m_Events.get();
    }

    void ModSDK::Log(spdlog::level::level_enum p_Level, std::string_view p_Msg) {
        DispatchLog(p_Level, p_Msg);
    }

    bool ModSDK::PatchCode(const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset) {
        return PatchCodeInternal(p_Pattern, p_Mask, p_NewCode, p_CodeSize, p_TargetOffset, nullptr);
    }

    bool ModSDK::PatchCodeStoreOriginal(
        const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset, void* p_OriginalCode
    ) {
        return PatchCodeInternal(p_Pattern, p_Mask, p_NewCode, p_CodeSize, p_TargetOffset, p_OriginalCode);
    }

    void ModSDK::AllocateZString(ZString* p_Target, const char* p_Str, uint32_t p_Size) {
        if (Globals() && Globals()->GameSceneflowModule && Globals()->GameSceneflowModule->IsEngineInitialized()) {
            // If engine is initialized, allocate the normal way.
            p_Target->m_nLength = p_Size;
            p_Target->m_pChars = Functions()->ZStringCollection_Allocate->Call(p_Str, p_Size)->m_pDataStart;
        }
        else {
            // Otherwise, allocate ourselves and make the game think it's a static allocation.
            // This will leak memory, but best we can do for now before the engine is initialized.
            auto* s_String = new char[p_Size + 1]{};
            memcpy(s_String, p_Str, p_Size);
            s_String[p_Size] = '\0';

            *p_Target = ZString(std::string_view(s_String, p_Size));
        }
    }

    void ModSDK::FreeZString(ZString* p_Target) {
        if (p_Target->IsAllocated() && Functions()) {
            SDK()->Functions()->ZString_ZImpl_Free->Call(
                reinterpret_cast<ZString::ZImpl*>(reinterpret_cast<uintptr_t>(p_Target->m_pChars) - sizeof(ZString::ZImpl))
            );
        }
    }

    void ModSDK::SetPluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, const ZString& p_Value) {
        if (!p_Plugin) {
            return;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return;
        }

        s_Settings->SetSetting(p_Section.c_str(), p_Name.c_str(), p_Value.c_str());
    }

    void ModSDK::SetPluginSettingInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, int64_t p_Value) {
        if (!p_Plugin) {
            return;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return;
        }

        s_Settings->SetSetting(p_Section.c_str(), p_Name.c_str(), std::to_string(p_Value));
    }

    void ModSDK::SetPluginSettingUInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, uint64_t p_Value) {
        if (!p_Plugin) {
            return;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return;
        }

        s_Settings->SetSetting(p_Section.c_str(), p_Name.c_str(), std::to_string(p_Value));
    }

    void ModSDK::SetPluginSettingDouble(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, double p_Value) {
        if (!p_Plugin) {
            return;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return;
        }

        s_Settings->SetSetting(p_Section.c_str(), p_Name.c_str(), std::to_string(p_Value));
    }

    void ModSDK::SetPluginSettingBool(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, bool p_Value) {
        if (!p_Plugin) {
            return;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return;
        }

        s_Settings->SetSetting(p_Section.c_str(), p_Name.c_str(), p_Value ? "true" : "false");
    }

    void ModSDK::GetPluginSetting(
        IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, const ZString& p_DefaultValue, ZString& p_OutValue
    ) {
        if (!p_Plugin) {
            p_OutValue = p_DefaultValue;
            return;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            p_OutValue = p_DefaultValue;
            return;
        }

        p_OutValue = s_Settings->GetSetting(p_Section.c_str(), p_Name.c_str(), p_DefaultValue.c_str());
    }

    int64_t ModSDK::GetPluginSettingInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, int64_t p_DefaultValue) {
        if (!p_Plugin) {
            return p_DefaultValue;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return p_DefaultValue;
        }

        const auto s_Value = s_Settings->GetSetting(p_Section.c_str(), p_Name.c_str(), std::to_string(p_DefaultValue));

        try {
            return std::stoll(s_Value);
        }
        catch (const std::exception&) {
            return p_DefaultValue;
        }
    }

    uint64_t ModSDK::GetPluginSettingUInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, uint64_t p_DefaultValue) {
        if (!p_Plugin) {
            return p_DefaultValue;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return p_DefaultValue;
        }

        const auto s_Value = s_Settings->GetSetting(p_Section.c_str(), p_Name.c_str(), std::to_string(p_DefaultValue));

        try {
            return std::stoull(s_Value);
        }
        catch (const std::exception&) {
            return p_DefaultValue;
        }
    }

    double ModSDK::GetPluginSettingDouble(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, double p_DefaultValue) {
        if (!p_Plugin) {
            return p_DefaultValue;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return p_DefaultValue;
        }

        const auto s_Value = s_Settings->GetSetting(p_Section.c_str(), p_Name.c_str(), std::to_string(p_DefaultValue));

        try {
            return std::stod(s_Value);
        }
        catch (const std::exception&) {
            return p_DefaultValue;
        }
    }

    bool ModSDK::GetPluginSettingBool(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, bool p_DefaultValue) {
        if (!p_Plugin) {
            return p_DefaultValue;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return p_DefaultValue;
        }

        const auto s_Value = s_Settings->GetSetting(p_Section.c_str(), p_Name.c_str(), p_DefaultValue ? "true" : "false");

        if (s_Value == "true" || s_Value == "1" || s_Value == "yes" || s_Value == "on" || s_Value == "y") {
            return true;
        }
        else if (s_Value == "false" || s_Value == "0" || s_Value == "no" || s_Value == "off" || s_Value == "n") {
            return false;
        }
        else {
            return p_DefaultValue;
        }
    }

    bool ModSDK::HasPluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name) {
        if (!p_Plugin) {
            return false;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return false;
        }

        return s_Settings->HasSetting(p_Section.c_str(), p_Name.c_str());
    }

    void ModSDK::RemovePluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name) {
        if (!p_Plugin) {
            return;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return;
        }

        s_Settings->RemoveSetting(p_Section.c_str(), p_Name.c_str());
    }

    void ModSDK::ReloadPluginSettings(IPluginInterface* p_Plugin) {
        if (!p_Plugin) {
            return;
        }

        auto s_Settings = m_ModLoader->GetModSettings(p_Plugin);

        if (!s_Settings) {
            return;
        }

        s_Settings->Reload();
    }

    bool ModSDK::WorldToScreen(const SVector3& p_WorldPos, SVector2& p_Out) {
        return m_DirectXTKRenderer->WorldToScreen(p_WorldPos, p_Out);
    }

    bool ModSDK::ScreenToWorld(const SVector2& p_ScreenPos, SVector3& p_OutWorldPos, SVector3& p_OutDirection) {
        return m_DirectXTKRenderer->ScreenToWorld(p_ScreenPos, p_OutWorldPos, p_OutDirection);
    }

    SMatrix ModSDK::GetViewMatrix() const {
        return m_DirectXTKRenderer->GetViewMatrix();
    }

    SMatrix ModSDK::GetProjectionMatrix() const {
        return m_DirectXTKRenderer->GetProjectionMatrix();
    }

    void ModSDK::GetEntityName(const ZEntityRef& p_EntityRef, ZString& p_OutEntityName) const {
        uint64_t s_EntityIndex;
        const auto s_ContainingBlueprintFactory = GetContainingBlueprintFactory(p_EntityRef, s_EntityIndex);

        if (s_ContainingBlueprintFactory && s_ContainingBlueprintFactory->IsTemplateEntityBlueprintFactory()) {
            const auto s_TemplateEntityBlueprintFactory = static_cast<ZTemplateEntityBlueprintFactory*>(s_ContainingBlueprintFactory);

            p_OutEntityName = s_TemplateEntityBlueprintFactory->m_pTemplateEntityBlueprint->subEntities[s_EntityIndex].entityName;
        }
    }

    IEntityBlueprintFactory* ModSDK::GetBlueprintFactory(const ZEntityRef& p_EntityRef) const {
        uint64_t s_EntityIndex;
        auto* s_ContainingBlueprintFactory = GetContainingBlueprintFactory(p_EntityRef, s_EntityIndex);

        return s_ContainingBlueprintFactory ? s_ContainingBlueprintFactory->GetSubEntityBlueprint(s_EntityIndex) : nullptr;
    }

    IEntityBlueprintFactory* ModSDK::GetContainingBlueprintFactory(const ZEntityRef& p_EntityRef, uint64_t& p_OutSubEntityIndex) const {
        p_OutSubEntityIndex = static_cast<uint64_t>(-1);

        ZEntityRef s_OwningEntity = p_EntityRef.GetOwningEntity();

        if (s_OwningEntity) {
            std::unordered_map<ZEntityRef, IEntityBlueprintFactory*> s_EntityRefToEntityBlueprintFactory;

            const auto s_EntitySceneContext = SDK()->Globals()->GameSceneflowModule->m_pEntitySceneContext;

            for (const auto& s_Brick : s_EntitySceneContext->m_SceneConfig->m_aMainBricks) {
                if (!s_Brick.m_EntityType) {
                    continue;
                }

                s_EntityRefToEntityBlueprintFactory.emplace(s_Brick.m_EntityType, s_Brick.m_BrickFactory->GetBlueprint());
            }

            for (const auto& s_Brick : s_EntitySceneContext->m_aDynamicBrickEntities) {
                if (!s_Brick.second) {
                    continue;
                }

                s_EntityRefToEntityBlueprintFactory.emplace(s_Brick.second, s_EntitySceneContext->m_aDynamicBrickBlueprintFactories[s_Brick.first]);
            }

            auto s_EntityIt = s_EntityRefToEntityBlueprintFactory.find(s_OwningEntity);

            if (s_EntityIt != s_EntityRefToEntityBlueprintFactory.end()) {
                IEntityBlueprintFactory* s_BlueprintFactory = s_EntityIt->second;

                for (uint64_t i = 0; i < s_BlueprintFactory->GetSubEntitiesCount(); ++i) {
                    ZEntityRef s_SubEntity = s_BlueprintFactory->GetSubEntity(s_OwningEntity.m_pObj, i);

                    if (s_SubEntity == p_EntityRef) {
                        p_OutSubEntityIndex = i;
                        return s_BlueprintFactory;
                    }
                }
            }

            std::vector<ZEntityRef> s_Path;

            ZEntityRef s_CurrentEntity = p_EntityRef;
            ZEntityRef s_BrickEntity;
            IEntityBlueprintFactory* s_BrickBlueprintFactory = nullptr;

            while (true) {
                ZEntityRef s_ParentEntity = s_CurrentEntity.GetLogicalParent();

                if (!s_ParentEntity) {
                    break;
                }

                auto s_EntityIt = s_EntityRefToEntityBlueprintFactory.find(s_ParentEntity);

                if (s_EntityIt != s_EntityRefToEntityBlueprintFactory.end()) {
                    s_BrickEntity = s_ParentEntity;
                    s_BrickBlueprintFactory = s_EntityIt->second;
                    break;
                }

                s_Path.push_back(s_ParentEntity);
                s_CurrentEntity = s_ParentEntity;
            }

            if (!s_BrickBlueprintFactory) {
                return nullptr;
            }

            IEntityBlueprintFactory* s_CurrentBlueprintFactory = s_BrickBlueprintFactory;
            ZEntityRef s_CurrentBlueprintEntity = s_BrickEntity;

            ZEntityRef s_PreviousEntity;
            uint64_t s_PreviousSubEntityIndex = static_cast<uint64_t>(-1);

            // Walk from the brick towards the entity.
            for (auto s_PathIt = s_Path.rbegin(); s_PathIt != s_Path.rend(); ++s_PathIt) {
                ZEntityRef s_PathEntity = *s_PathIt;

                bool s_Found = false;

                for (uint64_t i = 0; i < s_CurrentBlueprintFactory->GetSubEntitiesCount(); ++i) {
                    ZEntityRef s_SubEntity = s_CurrentBlueprintFactory->GetSubEntity(s_CurrentBlueprintEntity.m_pObj, i);

                    if (s_SubEntity == s_PathEntity) {
                        s_PreviousEntity = s_SubEntity;
                        s_PreviousSubEntityIndex = i;
                        s_Found = true;
                        break;
                    }
                }

                if (s_Found) {
                    continue;
                }

                // The next logical parent isn't in the current blueprint.
                // Try entering the blueprint referenced by the previous entity.
                if (s_PreviousSubEntityIndex == static_cast<uint64_t>(-1)) {
                    return nullptr;
                }

                IEntityBlueprintFactory* s_SubBlueprintFactory = s_CurrentBlueprintFactory->GetSubEntityBlueprint(s_PreviousSubEntityIndex);

                if (!s_SubBlueprintFactory) {
                    return nullptr;
                }

                s_CurrentBlueprintFactory = s_SubBlueprintFactory;
                s_CurrentBlueprintEntity = s_PreviousEntity;
                s_PreviousEntity = {};
                s_PreviousSubEntityIndex = static_cast<uint64_t>(-1);

                // Resolve the same path entity again, now inside the referenced blueprint.
                for (uint64_t i = 0; i < s_CurrentBlueprintFactory->GetSubEntitiesCount(); ++i) {
                    ZEntityRef s_SubEntity = s_CurrentBlueprintFactory->GetSubEntity(s_CurrentBlueprintEntity.m_pObj, i);

                    if (s_SubEntity == s_PathEntity) {
                        s_PreviousEntity = s_SubEntity;
                        s_PreviousSubEntityIndex = i;
                        s_Found = true;
                        break;
                    }
                }

                if (!s_Found) {
                    return nullptr;
                }
            }

            // Check whether the target belongs to the current blueprint.
            for (uint64_t i = 0; i < s_CurrentBlueprintFactory->GetSubEntitiesCount(); ++i) {
                ZEntityRef s_SubEntity = s_CurrentBlueprintFactory->GetSubEntity(s_CurrentBlueprintEntity.m_pObj, i);

                if (s_SubEntity == p_EntityRef) {
                    p_OutSubEntityIndex = i;
                    return s_CurrentBlueprintFactory;
                }
            }

            // Check whether the target belongs to the blueprint referenced by its logical parent.
            if (s_PreviousSubEntityIndex != static_cast<uint64_t>(-1)) {
                IEntityBlueprintFactory* s_SubBlueprintFactory = s_CurrentBlueprintFactory->GetSubEntityBlueprint(s_PreviousSubEntityIndex);

                if (s_SubBlueprintFactory) {
                    for (uint64_t i = 0; i < s_SubBlueprintFactory->GetSubEntitiesCount(); ++i) {
                        ZEntityRef s_SubEntity = s_SubBlueprintFactory->GetSubEntity(s_PreviousEntity.m_pObj, i);

                        if (s_SubEntity == p_EntityRef) {
                            p_OutSubEntityIndex = i;
                            return s_SubBlueprintFactory;
                        }
                    }
                }
            }

            return nullptr;
        }

        auto s_DynamicEntityIt = SDK()->Globals()->EntityManager->m_DynamicEntities.find(p_EntityRef->GetType()->m_nEntityID);

        if (s_DynamicEntityIt == SDK()->Globals()->EntityManager->m_DynamicEntities.end()) {
            return nullptr;
        }

        auto* s_BlueprintFactory = s_DynamicEntityIt->second.second.GetResource()->GetBlueprint();

        if (s_BlueprintFactory && s_BlueprintFactory->IsTemplateEntityBlueprintFactory()) {
            const auto* s_TemplateBlueprintFactory = static_cast<ZTemplateEntityBlueprintFactory*>(s_BlueprintFactory);

            p_OutSubEntityIndex = s_TemplateBlueprintFactory->m_pTemplateEntityBlueprint->rootEntityIndex;
        }

        return s_BlueprintFactory;
    }

    void ModSDK::RequestUIFocus() {
        if (!m_UIEnabled) {
            return;
        }

        m_ImGuiRenderer.get()->SetFocus(true);
    }

    void ModSDK::ReleaseUIFocus() {
        if (!m_UIEnabled) {
            return;
        }

        m_ImGuiRenderer.get()->SetFocus(false);
    }

    zknt::IImGuiRenderer* ModSDK::GetImGuiRenderer() const {
        return m_ImGuiRenderer.get();
    }

    zknt::IDirectXTKRenderer* ModSDK::GetDirectXTKRenderer() const {
        return m_DirectXTKRenderer.get();
    }

    zknt::ModLoader* ModSDK::GetModLoader() const {
        return m_ModLoader.get();
    }

    zknt::ui::ModSelector* ModSDK::GetUIModSelector() const {
        return m_ModSelector.get();
    }

    ui::Console* ModSDK::GetUIConsole() const {
        return m_Console.get();
    }

    void ModSDK::SetHasShownUIToggleWarning() {
        m_HasShownUIToggleWarning.store(true, std::memory_order_release);

        UpdateSDKIni("ui", "shown_ui_toggle_warning", "true");
    }

    bool ModSDK::PatchCodeInternal(
        const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset, void* p_OriginalCode
    ) {
        if (!p_Pattern || !p_Mask || !p_NewCode || p_CodeSize == 0) {
            Logger::Error("Invalid parameters provided to PatchCode call.");
            return false;
        }

        const auto* s_Pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
        const auto s_Target = Util::ProcessUtils::SearchPattern(GetModuleBase(), GetSizeOfCode(), s_Pattern, p_Mask);

        if (s_Target == 0) {
            Logger::Error("Could not find pattern in call to PatchCode. Game might have been updated.");
            return false;
        }

        auto* s_TargetPtr = reinterpret_cast<void*>(s_Target + p_TargetOffset);

        if (p_OriginalCode != nullptr) {
            memcpy(p_OriginalCode, s_TargetPtr, p_CodeSize);
        }

        Logger::Debug("Patching {} bytes of code at {} with new code from {}.", p_CodeSize, fmt::ptr(s_TargetPtr), p_NewCode);

        DWORD s_OldProtect;
        VirtualProtect(s_TargetPtr, p_CodeSize, PAGE_EXECUTE_READWRITE, &s_OldProtect);

        memcpy(s_TargetPtr, p_NewCode, p_CodeSize);

        VirtualProtect(s_TargetPtr, p_CodeSize, s_OldProtect, nullptr);

        return true;
    }

    void ModSDK::UpdateSDKIni(const std::string& p_Section, const std::string& p_Key, const std::string& p_Value) {
        char s_ExePathStr[MAX_PATH];
        const auto s_PathSize = GetModuleFileNameA(nullptr, s_ExePathStr, MAX_PATH);

        if (s_PathSize == 0) {
            return;
        }

        const std::filesystem::path s_ExePath(s_ExePathStr);
        const auto s_ExeDir = s_ExePath.parent_path();
        const auto s_IniPath = absolute(s_ExeDir / "sdk.ini");

        mINI::INIFile s_File(s_IniPath.string());
        mINI::INIStructure s_Ini;

        if (is_regular_file(s_IniPath)) {
            s_File.read(s_Ini);
        }

        s_Ini[p_Section].set(p_Key, p_Value);

        s_File.generate(s_Ini, true);
    }

    void ModSDK::OnModLoaded(const std::string& p_Name, IPluginInterface* p_Plugin, bool /*p_LiveLoad*/) const {
        Logger::Info("Mod '{}' loaded.", p_Name);

        p_Plugin->SetupUI(m_ImGuiRenderer.get());

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

    void ModSDK::RegisterUICallback(void* p_Token, UICallback p_Callback) {
        std::lock_guard s_Lock(m_UICallbacksMutex);
        m_UICallbacks[p_Token] = std::move(p_Callback);
    }

    void ModSDK::UnregisterUICallback(void* p_Token) {
        std::lock_guard s_Lock(m_UICallbacksMutex);
        m_UICallbacks.erase(p_Token);
    }

    void ModSDK::InvokeUICallbacks(bool p_HasFocus) {
        // Snapshot under the lock so callbacks can (un)register safely.
        std::vector<UICallback> s_Callbacks;
        {
            std::lock_guard s_Lock(m_UICallbacksMutex);
            s_Callbacks.reserve(m_UICallbacks.size());
            for (auto& [s_Token, s_Cb] : m_UICallbacks) {
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

        m_DirectXTKRenderer->OnEngineInitialized();

        if (m_ModLoader) {
            for (auto* s_Plugin : m_ModLoader->GetLoadedMods()) {
                if (s_Plugin) {
                    s_Plugin->OnEngineInitialized();
                }
            }
        }
    }

    void ModSDK::LoadConfiguration() {
        char s_ExePathStr[MAX_PATH];
        const auto s_PathSize = GetModuleFileNameA(nullptr, s_ExePathStr, MAX_PATH);

        if (s_PathSize == 0) {
            return;
        }

        const std::filesystem::path s_ExePath(s_ExePathStr);
        const auto s_IniPath = absolute(s_ExePath.parent_path() / "sdk.ini");

        mINI::INIFile s_File(s_IniPath.string());
        mINI::INIStructure s_Ini;

        if (!is_regular_file(s_IniPath)) {
            return;
        }

        s_File.read(s_Ini);

        const auto& s_UI = s_Ini["ui"];

        if (s_UI.has("noui") && s_UI.get("noui") == "true") {
            m_UIEnabled = false;

            MessageBoxA(
                nullptr,
                "WARNING: The mod SDK UI is currently disabled!\n\n"
                "If you want to re-enable it, set 'noui = false' in sdk.ini "
                "and restart your game.",
                "Mod SDK Warning", MB_OK | MB_ICONWARNING
            );
        }

        if (s_UI.has("console_key") && !s_UI.get("console_key").empty()) {
            try {
                m_ConsoleScanCode = std::stoul(s_UI.get("console_key"), nullptr, 0);
            }
            catch (const std::exception&) {
                Logger::Error("Could not parse console_key value from sdk.ini. Using default value.");
            }
        }

        if (s_UI.has("ui_toggle_key") && !s_UI.get("ui_toggle_key").empty()) {
            try {
                m_UIToggleScanCode = std::stoul(s_UI.get("ui_toggle_key"), nullptr, 0);
            }
            catch (const std::exception&) {
                Logger::Error("Could not parse ui_toggle_key value from sdk.ini. Using default value.");
            }
        }

        if (s_UI.has("shown_ui_toggle_warning")) {
            m_HasShownUIToggleWarning = true;
        }
    }

    void ModSDK::OnConsoleCommand(void* p_Context, TArray<ZString>& p_Args) {
        if (p_Args.size() == 1) {
            if (p_Args[0] == "unloadall") {
                ModSDK::GetInstance()->GetModLoader()->UnloadAllMods();
            }
            else if (p_Args[0] == "reloadall") {
                ModSDK::GetInstance()->GetModLoader()->ReloadAllMods();
            }
        }

        if (p_Args.size() == 2) {
            if (p_Args[0] == "load") {
                ModSDK::GetInstance()->GetModLoader()->LoadMod(p_Args[1].c_str(), true);
            }
            else if (p_Args[0] == "unload") {
                ModSDK::GetInstance()->GetModLoader()->UnloadMod(p_Args[1].c_str());
            }
            else if (p_Args[0] == "reload") {
                ModSDK::GetInstance()->GetModLoader()->ReloadMod(p_Args[1].c_str());
            }
            else if (p_Args[0] == "config") {
                ZConfigCommand* s_ConfigCommand = ZConfigCommand::Get(p_Args[1]);

                if (!s_ConfigCommand) {
                    Logger::Error("[ZConfigCommand] Invalid command.");
                    return;
                }

                switch (s_ConfigCommand->GetType()) {
                case ZConfigCommand::ECLASSTYPE::ECLASS_FLOAT:
                    Logger::Info("[ZConfigCommand] {} - float - {}", p_Args[1], s_ConfigCommand->As<ZConfigFloat>()->m_Value);
                    return;
                case ZConfigCommand::ECLASSTYPE::ECLASS_INT:
                    Logger::Info("[ZConfigCommand] {} - int - {}", p_Args[1], s_ConfigCommand->As<ZConfigInt>()->m_Value);
                    return;
                case ZConfigCommand::ECLASSTYPE::ECLASS_STRING:
                    Logger::Info("[ZConfigCommand] {} - string - \"{}\"", p_Args[1], s_ConfigCommand->As<ZConfigString>()->m_szValue);
                    return;
                case ZConfigCommand::ECLASSTYPE::ECLASS_UNKNOWN:
                    Logger::Error("[ZConfigCommand] Unsupported command type (ECLASS_UNKNOWN).");
                    return;
                }
            }
        }

        if (p_Args.size() == 3) {
            if (p_Args[0] == "config") {
                ZConfigCommand* s_ConfigCommand = ZConfigCommand::Get(p_Args[1]);

                if (!s_ConfigCommand) {
                    Logger::Info("[ZConfigCommand] Invalid command.");
                    return;
                }

                switch (s_ConfigCommand->GetType()) {
                case ZConfigCommand::ECLASSTYPE::ECLASS_FLOAT: {
                    try {
                        size_t s_ParsedLength;
                        static_cast<void>(std::stof(p_Args[2].c_str(), &s_ParsedLength));

                        if (s_ParsedLength != p_Args[2].size()) {
                            Logger::Error("[ZConfigCommand] Invalid input (float), not all characters provided were processed.");
                            return;
                        }
                    }
                    catch (const std::invalid_argument&) {
                        Logger::Error("[ZConfigCommand] Invalid input (float), input does not represent a float.");
                        return;
                    }
                    catch (const std::out_of_range&) {
                        Logger::Error("[ZConfigCommand] Invalid input (float), float is out of range.");
                        return;
                    }

                    break;
                }
                case ZConfigCommand::ECLASSTYPE::ECLASS_INT: {
                    try {
                        size_t s_ParsedLength;
                        unsigned long s_Value = std::stoul(p_Args[2].c_str(), &s_ParsedLength);

                        if (s_ParsedLength != p_Args[2].size()) {
                            Logger::Error("[ZConfigCommand] Invalid input (integer), not all characters provided were processed.");
                            return;
                        }

                        if (s_Value > (std::numeric_limits<unsigned int>::max)()) {
                            Logger::Error("[ZConfigCommand] Invalid input (integer), out of u32 range.");
                            return;
                        }
                    }
                    catch (const std::invalid_argument&) {
                        Logger::Error("[ZConfigCommand] Invalid input (integer), input does not represent a integer.");
                        return;
                    }
                    catch (const std::out_of_range&) {
                        Logger::Error("[ZConfigCommand] Invalid input (integer), integer is out of range.");
                        return;
                    }

                    break;
                }
                case ZConfigCommand::ECLASSTYPE::ECLASS_STRING: {
                    if (p_Args[2].size() >= 256) {
                        return Logger::Error("[ZConfigCommand] Invalid input (string), maximum length of 255 exceeded.");
                    }

                    break;
                }
                case ZConfigCommand::ECLASSTYPE::ECLASS_UNKNOWN:
                    Logger::Error("[ZConfigCommand] Unsupported command type (ECLASS_UNKNOWN).");
                    return;
                }

                SDK()->Functions()->ZConfigCommand_ExecuteCommand->Call(p_Args[1].c_str(), p_Args[2].c_str());

                Logger::Info(R"([ZConfigCommand] Set "{}" to "{}")", p_Args[1], p_Args[2]);
            }
        }
    }

    DEFINE_DETOUR_WITH_CONTEXT(ModSDK, bool, Engine_Init, void* th, void* a2) {
        auto s_Result = p_Hook->CallOriginal(th, a2);

        Logger::Info("Engine has initialized!");
        HandleEngineInitialized(true);

        return {HookAction::Return(), s_Result};
    }

    DEFINE_DETOUR_WITH_CONTEXT(ModSDK, void, SPassExecution_ExecutePass, SPassExecution* th, int32_t renderDeviceContextIndex) {
        if (th->m_pPassExecutionContext->m_pPassNode && th->m_pPassExecutionContext->m_pPassNode->m_DepthStencil
            && th->m_pPassExecutionContext->m_pPassNode->m_DepthStencil->m_pTexture->m_pResource && m_DirectXTKRenderer) {
            // Copy the depth buffer after the OpaqueAdvanced pass.
            if (std::string(th->m_pPassExecutionContext->m_pPassNode->m_ShortName) == "Decals") {
                m_DirectXTKRenderer->SetDepthBuffer(th->m_pPassExecutionContext->m_pPassNode->m_DepthStencil->m_pTexture->m_pResource);
                m_DirectXTKRenderer->CopyDepthBuffer(th->m_pPassExecutionContext->m_pRenderDeviceContexts[renderDeviceContextIndex]->m_pCommandList);
            }
        }

        return {HookAction::Continue()};
    }
}
