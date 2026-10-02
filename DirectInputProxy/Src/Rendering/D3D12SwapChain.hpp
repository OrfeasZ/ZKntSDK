#pragma once

#include <atomic>
#include <dxgi1_5.h>

namespace knt::rendering {
    // Passthrough wrapper for IDXGISwapChain4. Lives in the proxy DLL so the
    // game retains a stable vtable address across SDK hot-reloads.
    //
    // Intercepts Present/Present1 (to notify the SDK each frame) and the
    // ResizeBuffers* / ResizeTarget family (to let the SDK recreate its
    // backbuffer-dependent resources).
    class D3D12SwapChain final : public IDXGISwapChain4 {
      public:
        explicit D3D12SwapChain(IDXGISwapChain4* p_Target);
        ~D3D12SwapChain();

        D3D12SwapChain(const D3D12SwapChain&) = delete;
        D3D12SwapChain& operator=(const D3D12SwapChain&) = delete;

        IDXGISwapChain4* Target() const {
            return m_Target;
        }

        // IUnknown
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID p_Riid, void** p_OutObject) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;

        // IDXGIObject
        HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID p_Name, UINT p_DataSize, const void* p_Data) override {
            return m_Target->SetPrivateData(p_Name, p_DataSize, p_Data);
        }
        HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID p_Name, const IUnknown* p_Unknown) override {
            return m_Target->SetPrivateDataInterface(p_Name, p_Unknown);
        }
        HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID p_Name, UINT* p_DataSize, void* p_Data) override {
            return m_Target->GetPrivateData(p_Name, p_DataSize, p_Data);
        }
        HRESULT STDMETHODCALLTYPE GetParent(REFIID p_Riid, void** p_OutParent) override {
            return m_Target->GetParent(p_Riid, p_OutParent);
        }

        // IDXGIDeviceSubObject
        HRESULT STDMETHODCALLTYPE GetDevice(REFIID p_Riid, void** p_OutDevice) override {
            return m_Target->GetDevice(p_Riid, p_OutDevice);
        }

        // IDXGISwapChain
        HRESULT STDMETHODCALLTYPE Present(UINT p_SyncInterval, UINT p_Flags) override;
        HRESULT STDMETHODCALLTYPE GetBuffer(UINT p_Buffer, REFIID p_Riid, void** p_OutSurface) override {
            return m_Target->GetBuffer(p_Buffer, p_Riid, p_OutSurface);
        }
        HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL p_Fullscreen, IDXGIOutput* p_Target) override {
            return m_Target->SetFullscreenState(p_Fullscreen, p_Target);
        }
        HRESULT STDMETHODCALLTYPE GetFullscreenState(BOOL* p_OutFullscreen, IDXGIOutput** p_OutTarget) override {
            return m_Target->GetFullscreenState(p_OutFullscreen, p_OutTarget);
        }
        HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC* p_OutDesc) override {
            return m_Target->GetDesc(p_OutDesc);
        }
        HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT p_BufferCount, UINT p_Width, UINT p_Height, DXGI_FORMAT p_NewFormat, UINT p_Flags) override;
        HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC* p_NewTargetParameters) override;
        HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput** p_OutOutput) override {
            return m_Target->GetContainingOutput(p_OutOutput);
        }
        HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS* p_OutStats) override {
            return m_Target->GetFrameStatistics(p_OutStats);
        }
        HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT* p_OutCount) override {
            return m_Target->GetLastPresentCount(p_OutCount);
        }

        // IDXGISwapChain1
        HRESULT STDMETHODCALLTYPE GetDesc1(DXGI_SWAP_CHAIN_DESC1* p_OutDesc) override {
            return m_Target->GetDesc1(p_OutDesc);
        }
        HRESULT STDMETHODCALLTYPE GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* p_OutDesc) override {
            return m_Target->GetFullscreenDesc(p_OutDesc);
        }
        HRESULT STDMETHODCALLTYPE GetHwnd(HWND* p_OutHwnd) override {
            return m_Target->GetHwnd(p_OutHwnd);
        }
        HRESULT STDMETHODCALLTYPE GetCoreWindow(REFIID p_Riid, void** p_OutUnk) override {
            return m_Target->GetCoreWindow(p_Riid, p_OutUnk);
        }
        HRESULT STDMETHODCALLTYPE Present1(UINT p_SyncInterval, UINT p_Flags, const DXGI_PRESENT_PARAMETERS* p_Params) override;
        BOOL STDMETHODCALLTYPE IsTemporaryMonoSupported() override {
            return m_Target->IsTemporaryMonoSupported();
        }
        HRESULT STDMETHODCALLTYPE GetRestrictToOutput(IDXGIOutput** p_OutOutput) override {
            return m_Target->GetRestrictToOutput(p_OutOutput);
        }
        HRESULT STDMETHODCALLTYPE SetBackgroundColor(const DXGI_RGBA* p_Color) override {
            return m_Target->SetBackgroundColor(p_Color);
        }
        HRESULT STDMETHODCALLTYPE GetBackgroundColor(DXGI_RGBA* p_OutColor) override {
            return m_Target->GetBackgroundColor(p_OutColor);
        }
        HRESULT STDMETHODCALLTYPE SetRotation(DXGI_MODE_ROTATION p_Rotation) override {
            return m_Target->SetRotation(p_Rotation);
        }
        HRESULT STDMETHODCALLTYPE GetRotation(DXGI_MODE_ROTATION* p_OutRotation) override {
            return m_Target->GetRotation(p_OutRotation);
        }

        // IDXGISwapChain2
        HRESULT STDMETHODCALLTYPE SetSourceSize(UINT p_Width, UINT p_Height) override {
            return m_Target->SetSourceSize(p_Width, p_Height);
        }
        HRESULT STDMETHODCALLTYPE GetSourceSize(UINT* p_OutWidth, UINT* p_OutHeight) override {
            return m_Target->GetSourceSize(p_OutWidth, p_OutHeight);
        }
        HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(UINT p_MaxLatency) override {
            return m_Target->SetMaximumFrameLatency(p_MaxLatency);
        }
        HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(UINT* p_OutMaxLatency) override {
            return m_Target->GetMaximumFrameLatency(p_OutMaxLatency);
        }
        HANDLE STDMETHODCALLTYPE GetFrameLatencyWaitableObject() override {
            return m_Target->GetFrameLatencyWaitableObject();
        }
        HRESULT STDMETHODCALLTYPE SetMatrixTransform(const DXGI_MATRIX_3X2_F* p_Matrix) override {
            return m_Target->SetMatrixTransform(p_Matrix);
        }
        HRESULT STDMETHODCALLTYPE GetMatrixTransform(DXGI_MATRIX_3X2_F* p_OutMatrix) override {
            return m_Target->GetMatrixTransform(p_OutMatrix);
        }

        // IDXGISwapChain3
        UINT STDMETHODCALLTYPE GetCurrentBackBufferIndex() override {
            return m_Target->GetCurrentBackBufferIndex();
        }
        HRESULT STDMETHODCALLTYPE CheckColorSpaceSupport(DXGI_COLOR_SPACE_TYPE p_ColorSpace, UINT* p_OutSupport) override {
            return m_Target->CheckColorSpaceSupport(p_ColorSpace, p_OutSupport);
        }
        HRESULT STDMETHODCALLTYPE SetColorSpace1(DXGI_COLOR_SPACE_TYPE p_ColorSpace) override {
            return m_Target->SetColorSpace1(p_ColorSpace);
        }
        HRESULT STDMETHODCALLTYPE ResizeBuffers1(
            UINT p_BufferCount, UINT p_Width, UINT p_Height, DXGI_FORMAT p_Format, UINT p_Flags, const UINT* p_CreationNodeMask,
            IUnknown* const* p_PresentQueue
        ) override;

        // IDXGISwapChain4
        HRESULT STDMETHODCALLTYPE SetHDRMetaData(DXGI_HDR_METADATA_TYPE p_Type, UINT p_Size, void* p_MetaData) override {
            return m_Target->SetHDRMetaData(p_Type, p_Size, p_MetaData);
        }

      private:
        IDXGISwapChain4* m_Target;
        std::atomic<ULONG> m_RefCount{0};
    };
}
