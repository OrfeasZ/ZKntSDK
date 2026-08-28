#include "D3D12DXGIFactory.hpp"

#include "D3D12SwapChain.hpp"
#include "RenderingHost.hpp"

#include "Util/D3DUtils.hpp"

namespace knt::rendering {
    D3D12DXGIFactory::D3D12DXGIFactory(IDXGIFactory4* p_Target) : m_Target(p_Target) {
        m_Target->AddRef();
    }

    D3D12DXGIFactory::~D3D12DXGIFactory() {
        m_Target->Release();
    }

    ULONG STDMETHODCALLTYPE D3D12DXGIFactory::AddRef() {
        return ++m_RefCount;
    }

    ULONG STDMETHODCALLTYPE D3D12DXGIFactory::Release() {
        const ULONG s_NewRefCount = --m_RefCount;
        if (s_NewRefCount == 0) {
            delete this;
        }
        return s_NewRefCount;
    }

    HRESULT STDMETHODCALLTYPE D3D12DXGIFactory::QueryInterface(REFIID p_Riid, void** p_OutObject) {
        if (!p_OutObject) {
            return E_INVALIDARG;
        }
        *p_OutObject = nullptr;
        if (p_Riid == IID_IUnknown || p_Riid == __uuidof(IDXGIObject) || p_Riid == __uuidof(IDXGIFactory) || p_Riid == __uuidof(IDXGIFactory1)
            || p_Riid == __uuidof(IDXGIFactory2) || p_Riid == __uuidof(IDXGIFactory3) || p_Riid == __uuidof(IDXGIFactory4)
            || p_Riid == __uuidof(IDXGIFactory5) || p_Riid == __uuidof(IDXGIFactory6) || p_Riid == __uuidof(IDXGIFactory7)) {
            *p_OutObject = this;
            AddRef();
            return S_OK;
        }

        // Pass through unknown IIDs (vendor / Streamline / IDXGIDebug / ...);
        // rejecting them breaks DLSS detection (and probably other things too).
        return m_Target->QueryInterface(p_Riid, p_OutObject);
    }

    HRESULT STDMETHODCALLTYPE D3D12DXGIFactory::CheckFeatureSupport(DXGI_FEATURE p_Feature, void* p_Data, UINT p_DataSize) {
        return WithFactory<IDXGIFactory5>([&](auto* p) { return p->CheckFeatureSupport(p_Feature, p_Data, p_DataSize); });
    }

    HRESULT STDMETHODCALLTYPE
    D3D12DXGIFactory::EnumAdapterByGpuPreference(UINT p_Adapter, DXGI_GPU_PREFERENCE p_GpuPreference, REFIID p_Riid, void** p_OutAdapter) {
        return WithFactory<IDXGIFactory6>([&](auto* p) { return p->EnumAdapterByGpuPreference(p_Adapter, p_GpuPreference, p_Riid, p_OutAdapter); });
    }

    HRESULT STDMETHODCALLTYPE D3D12DXGIFactory::RegisterAdaptersChangedEvent(HANDLE p_Event, DWORD* p_OutCookie) {
        return WithFactory<IDXGIFactory7>([&](auto* p) { return p->RegisterAdaptersChangedEvent(p_Event, p_OutCookie); });
    }

    HRESULT STDMETHODCALLTYPE D3D12DXGIFactory::UnregisterAdaptersChangedEvent(DWORD p_Cookie) {
        return WithFactory<IDXGIFactory7>([&](auto* p) { return p->UnregisterAdaptersChangedEvent(p_Cookie); });
    }

    void D3D12DXGIFactory::WrapAndPublishSwapChain(IUnknown* p_Device, IDXGISwapChain* p_RawSwapChain, IDXGISwapChain** p_OutSwapChain) {
        ScopedD3DRef<IDXGISwapChain3> s_SwapChain3;
        if (p_RawSwapChain->QueryInterface(REF_IID_PPV_ARGS(s_SwapChain3)) != S_OK) {
            // Not a SwapChain3 (D3D11 or older). Hand the raw object back.
            *p_OutSwapChain = p_RawSwapChain;
            return;
        }

        // The wrapper owns the QI'd IDXGISwapChain3 ref; drop the raw one.
        p_RawSwapChain->Release();

        auto* s_Wrapped = new D3D12SwapChain(s_SwapChain3.m_Ref);
        s_Wrapped->AddRef();
        *p_OutSwapChain = s_Wrapped;

        RenderingHost::Instance().NotifyNewSwapChain(s_SwapChain3.m_Ref, p_Device);
    }

    HRESULT STDMETHODCALLTYPE D3D12DXGIFactory::CreateSwapChain(IUnknown* p_Device, DXGI_SWAP_CHAIN_DESC* p_Desc, IDXGISwapChain** p_OutSwapChain) {
        return CreateSwapChainImpl(p_Device, p_OutSwapChain, [&](IDXGISwapChain** raw) { return m_Target->CreateSwapChain(p_Device, p_Desc, raw); });
    }

    HRESULT STDMETHODCALLTYPE D3D12DXGIFactory::CreateSwapChainForHwnd(
        IUnknown* p_Device, HWND p_Hwnd, const DXGI_SWAP_CHAIN_DESC1* p_Desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* p_FullscreenDesc,
        IDXGIOutput* p_RestrictToOutput, IDXGISwapChain1** p_OutSwapChain
    ) {
        return CreateSwapChainImpl(p_Device, p_OutSwapChain, [&](IDXGISwapChain1** raw) {
            return m_Target->CreateSwapChainForHwnd(p_Device, p_Hwnd, p_Desc, p_FullscreenDesc, p_RestrictToOutput, raw);
        });
    }

    HRESULT STDMETHODCALLTYPE D3D12DXGIFactory::CreateSwapChainForCoreWindow(
        IUnknown* p_Device, IUnknown* p_Window, const DXGI_SWAP_CHAIN_DESC1* p_Desc, IDXGIOutput* p_RestrictToOutput, IDXGISwapChain1** p_OutSwapChain
    ) {
        return CreateSwapChainImpl(p_Device, p_OutSwapChain, [&](IDXGISwapChain1** raw) {
            return m_Target->CreateSwapChainForCoreWindow(p_Device, p_Window, p_Desc, p_RestrictToOutput, raw);
        });
    }

    HRESULT STDMETHODCALLTYPE D3D12DXGIFactory::CreateSwapChainForComposition(
        IUnknown* p_Device, const DXGI_SWAP_CHAIN_DESC1* p_Desc, IDXGIOutput* p_RestrictToOutput, IDXGISwapChain1** p_OutSwapChain
    ) {
        return CreateSwapChainImpl(p_Device, p_OutSwapChain, [&](IDXGISwapChain1** raw) {
            return m_Target->CreateSwapChainForComposition(p_Device, p_Desc, p_RestrictToOutput, raw);
        });
    }
}
