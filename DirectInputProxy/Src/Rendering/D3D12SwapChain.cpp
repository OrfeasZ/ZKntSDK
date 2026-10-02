#include "D3D12SwapChain.hpp"

#include "RenderingHost.hpp"
#include "WndProcSubclass.hpp"

namespace knt::rendering {
    D3D12SwapChain::D3D12SwapChain(IDXGISwapChain4* p_Target) : m_Target(p_Target) {
        m_Target->AddRef();
    }

    D3D12SwapChain::~D3D12SwapChain() {
        m_Target->Release();
    }

    ULONG STDMETHODCALLTYPE D3D12SwapChain::AddRef() {
        return ++m_RefCount;
    }

    ULONG STDMETHODCALLTYPE D3D12SwapChain::Release() {
        const ULONG s_NewRefCount = --m_RefCount;
        if (s_NewRefCount == 0) {
            delete this;
        }
        return s_NewRefCount;
    }

    HRESULT STDMETHODCALLTYPE D3D12SwapChain::QueryInterface(REFIID p_Riid, void** p_OutObject) {
        if (!p_OutObject) {
            return E_INVALIDARG;
        }
        *p_OutObject = nullptr;
        if (p_Riid == IID_IUnknown || p_Riid == __uuidof(IDXGIObject) || p_Riid == __uuidof(IDXGIDeviceSubObject)
            || p_Riid == __uuidof(IDXGISwapChain) || p_Riid == __uuidof(IDXGISwapChain1) || p_Riid == __uuidof(IDXGISwapChain2)
            || p_Riid == __uuidof(IDXGISwapChain3) || p_Riid == __uuidof(IDXGISwapChain4)) {
            *p_OutObject = this;
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE D3D12SwapChain::Present(UINT p_SyncInterval, UINT p_Flags) {
        WndProcSubclass::Instance().EnsureInstalled(m_Target);

        auto& s_Host = RenderingHost::Instance();
        s_Host.DispatchOnPresent(m_Target);
        const HRESULT s_Result = m_Target->Present(p_SyncInterval, p_Flags);
        s_Host.DispatchPostPresent(m_Target, s_Result);
        return s_Result;
    }

    HRESULT STDMETHODCALLTYPE D3D12SwapChain::Present1(UINT p_SyncInterval, UINT p_Flags, const DXGI_PRESENT_PARAMETERS* p_Params) {
        WndProcSubclass::Instance().EnsureInstalled(m_Target);

        auto& s_Host = RenderingHost::Instance();
        s_Host.DispatchOnPresent(m_Target);
        const HRESULT s_Result = m_Target->Present1(p_SyncInterval, p_Flags, p_Params);
        s_Host.DispatchPostPresent(m_Target, s_Result);
        return s_Result;
    }

    HRESULT STDMETHODCALLTYPE D3D12SwapChain::ResizeBuffers(UINT p_BufferCount, UINT p_Width, UINT p_Height, DXGI_FORMAT p_NewFormat, UINT p_Flags) {
        auto& s_Host = RenderingHost::Instance();
        s_Host.DispatchOnReset(m_Target);
        const HRESULT s_Result = m_Target->ResizeBuffers(p_BufferCount, p_Width, p_Height, p_NewFormat, p_Flags);
        s_Host.DispatchPostReset(m_Target);
        return s_Result;
    }

    HRESULT STDMETHODCALLTYPE D3D12SwapChain::ResizeBuffers1(
        UINT p_BufferCount, UINT p_Width, UINT p_Height, DXGI_FORMAT p_Format, UINT p_Flags, const UINT* p_CreationNodeMask,
        IUnknown* const* p_PresentQueue
    ) {
        auto& s_Host = RenderingHost::Instance();
        s_Host.DispatchOnReset(m_Target);
        const HRESULT s_Result = m_Target->ResizeBuffers1(p_BufferCount, p_Width, p_Height, p_Format, p_Flags, p_CreationNodeMask, p_PresentQueue);
        s_Host.DispatchPostReset(m_Target);
        return s_Result;
    }

    HRESULT STDMETHODCALLTYPE D3D12SwapChain::ResizeTarget(const DXGI_MODE_DESC* p_NewTargetParameters) {
        auto& s_Host = RenderingHost::Instance();
        s_Host.DispatchOnReset(m_Target);
        const HRESULT s_Result = m_Target->ResizeTarget(p_NewTargetParameters);
        s_Host.DispatchPostReset(m_Target);
        return s_Result;
    }
}
