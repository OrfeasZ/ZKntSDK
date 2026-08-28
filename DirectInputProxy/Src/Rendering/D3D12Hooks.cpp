#include "D3D12Hooks.hpp"

#include "D3D12DXGIFactory.hpp"
#include "Util/D3DUtils.hpp"

#include <MinHook.h>
#include <dxgi1_4.h>

namespace knt::rendering {
    namespace {
        using PFN_CreateDXGIFactory = HRESULT(WINAPI*)(REFIID, void**);
        using PFN_CreateDXGIFactory2 = HRESULT(WINAPI*)(UINT, REFIID, void**);

        PFN_CreateDXGIFactory g_OriginalCreateDXGIFactory = nullptr;
        PFN_CreateDXGIFactory g_OriginalCreateDXGIFactory1 = nullptr;
        PFN_CreateDXGIFactory2 g_OriginalCreateDXGIFactory2 = nullptr;

        HRESULT WrapDxgiFactory(REFIID p_Riid, void** p_OutFactory, IDXGIFactory* p_Raw) {
            ScopedD3DRef<IDXGIFactory4> s_Factory4;
            if (p_Raw->QueryInterface(REF_IID_PPV_ARGS(s_Factory4)) != S_OK) {
                const HRESULT s_Qi = p_Raw->QueryInterface(p_Riid, p_OutFactory);
                p_Raw->Release();
                return s_Qi;
            }
            auto* s_Wrapped = new D3D12DXGIFactory(s_Factory4.m_Ref);
            s_Wrapped->AddRef();
            p_Raw->Release();
            const HRESULT s_Result = s_Wrapped->QueryInterface(p_Riid, p_OutFactory);
            s_Wrapped->Release();
            return s_Result;
        }

        HRESULT WINAPI Detour_CreateDXGIFactory(REFIID p_Riid, void** p_OutFactory) {
            IDXGIFactory* s_Factory = nullptr;
            const HRESULT s_Result = g_OriginalCreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&s_Factory));
            if (FAILED(s_Result) || !s_Factory) {
                if (p_OutFactory) {
                    *p_OutFactory = nullptr;
                }
                return s_Result;
            }
            return WrapDxgiFactory(p_Riid, p_OutFactory, s_Factory);
        }

        HRESULT WINAPI Detour_CreateDXGIFactory1(REFIID p_Riid, void** p_OutFactory) {
            IDXGIFactory* s_Factory = nullptr;
            const HRESULT s_Result = g_OriginalCreateDXGIFactory1(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&s_Factory));
            if (FAILED(s_Result) || !s_Factory) {
                if (p_OutFactory) {
                    *p_OutFactory = nullptr;
                }
                return s_Result;
            }
            return WrapDxgiFactory(p_Riid, p_OutFactory, s_Factory);
        }

        HRESULT WINAPI Detour_CreateDXGIFactory2(UINT p_Flags, REFIID p_Riid, void** p_OutFactory) {
            IDXGIFactory* s_Factory = nullptr;
            const HRESULT s_Result = g_OriginalCreateDXGIFactory2(p_Flags, __uuidof(IDXGIFactory), reinterpret_cast<void**>(&s_Factory));
            if (FAILED(s_Result) || !s_Factory) {
                if (p_OutFactory) {
                    *p_OutFactory = nullptr;
                }
                return s_Result;
            }
            return WrapDxgiFactory(p_Riid, p_OutFactory, s_Factory);
        }

        template<typename TFunc> bool InstallHook(LPCWSTR p_Module, const char* p_Symbol, TFunc p_Detour, TFunc& p_OutOriginal) {
            HMODULE s_Module = LoadLibraryW(p_Module);
            if (!s_Module) {
                return false;
            }
            void* s_Target = reinterpret_cast<void*>(GetProcAddress(s_Module, p_Symbol));
            if (!s_Target) {
                return false;
            }
            void* s_Original = nullptr;
            if (MH_CreateHook(s_Target, reinterpret_cast<void*>(p_Detour), &s_Original) != MH_OK) {
                return false;
            }
            if (MH_EnableHook(s_Target) != MH_OK) {
                return false;
            }
            p_OutOriginal = reinterpret_cast<TFunc>(s_Original);
            return true;
        }
    }

    D3D12Hooks& D3D12Hooks::Instance() {
        static D3D12Hooks s_Instance;
        return s_Instance;
    }

    bool D3D12Hooks::Startup() {
        if (m_Installed) {
            return true;
        }

        const MH_STATUS s_InitResult = MH_Initialize();
        if (s_InitResult != MH_OK && s_InitResult != MH_ERROR_ALREADY_INITIALIZED) {
            return false;
        }

        bool s_Ok = true;
        s_Ok &= InstallHook(L"dxgi.dll", "CreateDXGIFactory", &Detour_CreateDXGIFactory, g_OriginalCreateDXGIFactory);
        s_Ok &= InstallHook(L"dxgi.dll", "CreateDXGIFactory1", &Detour_CreateDXGIFactory1, g_OriginalCreateDXGIFactory1);
        s_Ok &= InstallHook(L"dxgi.dll", "CreateDXGIFactory2", &Detour_CreateDXGIFactory2, g_OriginalCreateDXGIFactory2);

        m_Installed = s_Ok;
        return s_Ok;
    }

    void D3D12Hooks::Shutdown() {
        if (!m_Installed) {
            return;
        }
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        m_Installed = false;
    }
}
