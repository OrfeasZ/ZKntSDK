#pragma once

#include <Effects.h>
#include <GraphicsMemory.h>

#include <Glacier/ZMath.hpp>

struct CD3DX12_RESOURCE_DESC;

namespace zknt::rendering {
    class DebugEffect {
      public:
        struct Constants {
            DirectX::XMFLOAT4X4 m_World;
            DirectX::XMFLOAT4X4 m_View;
            DirectX::XMFLOAT4X4 m_Projection;

            DirectX::XMFLOAT4 m_PositionScale;
            DirectX::XMFLOAT4 m_PositionBias;
            DirectX::XMFLOAT4 m_TextureScaleBias;

            DirectX::XMFLOAT4 m_MaterialColor;
        };

        enum RootParameterIndex { ConstantBuffer, TextureSRV, TextureSampler, RootParameterCount };

        static_assert((sizeof(Constants) % 16) == 0, "Constant buffer size isn't padded correctly!");

        DebugEffect(
            ID3D12Device* p_Device, ID3D12GraphicsCommandList* p_CommandList, ID3D12CommandQueue* p_CommandQueue,
            const D3D12_INPUT_LAYOUT_DESC* p_InputLayoutDesc
        );
        ~DebugEffect();

        void CreateRootSignature(ID3D12Device* p_Device);
        void CreateTexture(ID3D12Device* p_Device, ID3D12GraphicsCommandList* p_CommandList, ID3D12CommandQueue* p_CommandQueue);
        void CreateShaderResourceView(ID3D12Device* p_Device, const CD3DX12_RESOURCE_DESC& p_TextureDesc, ID3D12Resource* p_TextureResource);
        void CreateSampler(ID3D12Device* p_Device);

        void Apply(ID3D12Device* p_Device, ID3D12GraphicsCommandList* p_CommandList);

        void SetWorld(const DirectX::XMFLOAT4X4& p_World);
        void SetView(const DirectX::XMFLOAT4X4& p_View);
        void SetProjection(const DirectX::XMFLOAT4X4& p_Projection);
        void SetPositionScale(const DirectX::XMFLOAT4& p_PositionScale);
        void SetPositionBias(const DirectX::XMFLOAT4& p_PositionBias);
        void SetTextureScaleBias(const DirectX::XMFLOAT4& p_TextureScaleBias);
        void SetMaterialColor(const DirectX::XMFLOAT4& p_MaterialColor);

        bool CompileShaderFromString(
            const std::string& p_ShaderCode, const std::string& p_EntryPoint, const std::string& p_ShaderModel, ID3DBlob** p_ShaderBlob
        );

      private:
        Constants m_Constants;
        DirectX::GraphicsResource m_ConstantBufferResource;
        ID3D12PipelineState* m_pPipelineState;
        ID3D12RootSignature* m_pRootSignature;
        ID3D12Resource* m_pTextureResource;
        ID3D12DescriptorHeap* m_pSRVHeap;
        ID3D12DescriptorHeap* m_pSamplerHeap;
    };
}
