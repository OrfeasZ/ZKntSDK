#pragma once

#include <directx/d3d12.h>
#include <dxgi.h>

#include "ZEntity.hpp"
#include "ZScene.hpp"
#include "TObjectPool.hpp"
#include "TRefCountPtr.hpp"

class ZRenderPipelineState;
class ZRenderInputLayout;
class ZRenderDepthStencilState;
class ZRenderBlendState;
class ZRenderRasterizerState;
class ZRenderTexture3D;
class ZRenderBuffer;
class ZRenderShaderResourceView;
class ZRenderUnorderedAccessView;
class ZUploadBufferResource;

template<class T, bool U> class TRenderReferencedCountedImpl : public T {
  public:
    int32_t m_ReferenceCount;
};

class IRenderRefCount {
  public:
    virtual ~IRenderRefCount() = 0;
    virtual void AddRef() = 0;
    virtual uint32_t Release() = 0;
};

class IRenderDestinationEntity : public IComponentInterface {
  public:
    virtual ZEntityRef& GetSource() = 0;
    virtual void IRenderDestinationEntity_unk6() = 0;
    virtual void IRenderDestinationEntity_unk7() const = 0;
    virtual void IRenderDestinationEntity_unk8() = 0;
    virtual void SetSource(ZEntityRef& rSource) = 0;
};

class IRenderDestinationSource : public IComponentInterface {
  public:
    virtual ~IRenderDestinationSource() = 0;
};

class ZRenderTexture2D {
  public:
    virtual ~ZRenderTexture2D() = 0;

  public:
    ID3D12Resource* m_pResource; // 0x8
};

class ZRenderTargetView {
  public:
    virtual ~ZRenderTargetView() = 0;

  public:
    PAD(0x20);                    // 0x8
    ZRenderTexture2D* m_pTexture; // 0x28
    PAD(0x8);
};

class ZRenderDepthStencilView {
  public:
    virtual ~ZRenderDepthStencilView() = 0;

  public:
    PAD(0x20);                    // 0x8
    ZRenderTexture2D* m_pTexture; // 0x28
    PAD(0x8);                     // 0x30
};

class ZRenderSwapChain {
  public:
    virtual ~ZRenderSwapChain() = 0;

  public:
    IDXGIFactory1* m_pFactory;    // 0x8
    IDXGISwapChain* m_pSwapChain; // 0x10
};

class ZRenderDeviceContext {
  public:
    PAD(0x18);                                    // 0x0
    uint32_t m_nNumRenderTargetViews;             // 0x18
    ZRenderTargetView* m_pRenderTargetViews[8];   // 0x20
    ZRenderDepthStencilView* m_pDepthStencilView; // 0x60
    PAD(0x1AD0);                                  // 0x68
    ID3D12GraphicsCommandList* m_pCommandList;    // 0x1B38
};

class IRenderDevice {
    virtual ~IRenderDevice() = 0;
};

class ZRenderDeviceBase : public IRenderDevice {
    virtual ~ZRenderDeviceBase() = 0;
};

class ZRenderDevice : public ZRenderDeviceBase {
  public:
    virtual ~ZRenderDevice() = 0;

  public:
    PAD(0x308);                                 // 0x8
    ZRenderSwapChain* m_pSwapChain;             // 0x310
    PAD(0x398);                                 // 0x318
    ZRenderDeviceContext* m_pMainContext;       // 0x6B0
    PAD(0x4AB1178);                             // 0x6B8
    ID3D12DescriptorHeap* m_pDescriptorHeapDSV; // 0x4AB1830
};

class IRenderManager : public IComponentInterface {};

class ZRenderManager : public IRenderManager, public ZSceneLifecycleListener {
  public:
    virtual ~ZRenderManager() = default;

  public:
    PAD(0x17C50);                   // 0x10
    ZRenderDevice* m_pRenderDevice; // 0x17C60
};

class RenderReferencedCountedBaseStub {
  public:
    virtual ~RenderReferencedCountedBaseStub() = 0;
};

struct SSemanticStringPair {
    ZString m_MaterialPropertyName; // 0x0
    ZString m_ShaderParameterName;  // 0x10
    int32_t m_Unk0;                 // 0x20
    bool m_Unk1;                    // 0x24
};

enum class ERenderConstBufferType {
    RENDER_CONST_BUFFER_VECTOR_1D = 1,
    RENDER_CONST_BUFFER_VECTOR_2D,
    RENDER_CONST_BUFFER_VECTOR_3D,
    RENDER_CONST_BUFFER_TRANSFORM_2D,
    RENDER_CONST_BUFFER_MATRIX_4X4 = 8,
    RENDER_CONST_BUFFER_TEXTURE_2D,
    RENDER_CONST_BUFFER_TEXTURE_3D,
    RENDER_CONST_BUFFER_TEXTURE_CUBE,
    RENDER_CONST_BUFFER_TEXTURE_CUBE_ARRAY = 13,
    RENDER_CONST_BUFFER_BUFFER = 15,
    RENDER_CONST_BUFFER_TEXTURE_2D_ARRAY
};

struct SRenderConstDesc {
    ERenderConstBufferType nType; // 0x0
    ZString Name;                 // 0x8
    uint16_t nOffset;             // 0x18
    uint32_t nSize;               // 0x1C
};

struct SRenderTextureDesc : SRenderConstDesc {};

struct SRenderConstBufferDesc {
    PAD(0x10);                           // 0x0
    uint32_t nNumConstants;              // 0x10
    uint32_t nNumTextures;               // 0x14
    TArray<SRenderConstDesc> Constants;  // 0x18
    TArray<SRenderTextureDesc> Textures; // 0x30
};

enum class EFX2ShaderType {
    FX2_SHADER_TYPE_VERTEX_SHADER,
    FX2_SHADER_TYPE_PIXEL_SHADER,
    FX2_SHADER_TYPE_DOMAIN_SHADER,
    FX2_SHADER_TYPE_HULL_SHADER,
    FX2_SHADER_TYPE_COMPUTE_SHADER,
    FX2_SHADER_TYPE_RAYTRACING_SHADER,
    FX2_SHADER_TYPE_SIZE
};

class ZRenderShader {
  public:
    EFX2ShaderType m_eShaderType;  // 0x0
    SRenderConstBufferDesc m_Desc; // 0x8
    PAD(0x10);                     // 0x50
    uint32 m_nByteCodeSize;        // 0x60
    PAD(0x4);                      // 0x64
    const uint8_t* m_pByteCode;    // 0x68
    PAD(0x8);                      // 0x70
    ZString m_Name;                // 0x78
};

class ZRenderEffectTechnique;

class ZRenderEffectPass {
  public:
    virtual ~ZRenderEffectPass() = 0;

    ZRenderEffectTechnique* m_pTechnique;                                                // 0x8
    ZRenderShader* m_pShader[static_cast<size_t>(EFX2ShaderType::FX2_SHADER_TYPE_SIZE)]; // 0x10
};

class ZRenderEffect;

class ZRenderEffectTechnique {
  public:
    virtual ~ZRenderEffectTechnique() = 0;

    TArray<ZRenderEffectPass*> m_Passes; // 0x8
    ZRenderEffect* m_pEffect;            // 0x20
};

class ZRenderEffect : public TRenderReferencedCountedImpl<RenderReferencedCountedBaseStub, false> {
  public:
    int32_t m_nId;                                                                           // 0x10
    THashMap<ZString, ZRenderEffectTechnique*, TDefaultHashMapPolicy<ZString>> m_Techniques; // 0x18
    TArray<ZRenderShader*> m_Programs;                                                       // 0x38
    TArray<SSemanticStringPair> m_SemanticStringPairs;                                       // 0x50
};

struct STextureInfo {
    uint16_t nNameOffset;  // 0x0
    uint8 nInterpretAs;    // 0x2
    uint8 nResourceOffset; // 0x3
};

class ZRenderMaterialEffectData {
  public:
    virtual ~ZRenderMaterialEffectData() = 0;

    ZRenderEffect* m_pRenderEffect; // 0x8
};

class ZRenderMaterialInstance : public TRenderReferencedCountedImpl<RenderReferencedCountedBaseStub, false> {
  public:
    struct SMaterialData {
        uint8_t m_Data[1]; // Packed material data containing reference types and property names
    };

    enum class ERefereceType : uint8_t { Texture, Gradient, Curve, MaterialEffect, MaterialDescriptor };

    const ERefereceType* GetReferenceTypes() const {
        return reinterpret_cast<const ERefereceType*>(m_MaterialData->m_Data);
    }

    const char* GetPropertyNames() const {
        return reinterpret_cast<const char*>(m_MaterialData->m_Data + m_ReferenceCount);
    }

    const char* GetPropertyName(uint16_t p_NameOffset) const {
        return GetPropertyNames() + p_NameOffset;
    }

    PAD(0x6D4);                                            // 0x10
    TMaxArray<STextureInfo, 32> m_TextureInfo;             // 0x6E4
    PAD(0x38);                                             // 0x768
    TResourcePtr<ZRenderMaterialEffectData> m_pEffectData; // 0x7A0
    ZRenderEffect* m_pEffect;                              // 0x7A8
    ZResourcePtr m_pMaterialDescriptor;                    // 0x7B0
    PAD(0x17);                                             // 0x7B8
    uint8_t m_ReferenceCount;                              // 0x7CF
    PAD(0x40);                                             // 0x7D0
    SMaterialData* m_MaterialData;                         // 0x810
};

struct SRenderCameraData {
    PAD(0x30);                    // 0x0
    SVector4 m_CameraPos;         // 0x30
    SVector4 m_CameraUp;          // 0x40
    SVector4 m_CameraRight;       // 0x50
    SVector4 m_CameraFwd;         // 0x60
    PAD(0x80);                    // 0x70
    SMatrix44 m_CameraViewToClip; // 0xF0
};

class FrameGraphInternal {
  public:
    struct SEventDescriptor {
        const char* m_ShortName;   // 0x0
        const char* m_DisplayName; // 0x8
        uint32_t m_Color;          // 0x10
        const char* m_Function;    // 0x18
        const char* m_File;        // 0x20
        uint32_t m_Line;           // 0x28
    };

    struct SEvent {
        virtual ~SEvent() = 0;

        uint32_t m_Unk0;                       // 0x8
        const char* m_DisplayName;             // 0x10
        const char* m_ShortName;               // 0x18
        SEventDescriptor** m_EventDescriptors; // 0x20
        uint32_t m_EventDescriptorCount;       // 0x28
        int32_t m_Unk1;                        // 0x2C
        int32_t m_Unk2;                        // 0x30
    };

    struct SPassNode : SEvent {
        virtual ~SPassNode() = default;

        PAD(0x48);                               // 0x38
        uint32_t m_RenderTargetCount;            // 0x80
        ZRenderTargetView* m_RenderTargets[8];   // 0x88
        ZRenderDepthStencilView* m_DepthStencil; // 0xC8
    };
};

class ZFrameGraph {
  public:
    PAD(0x20);                                                         // 0x0
    TArray<FrameGraphInternal::SEvent*> m_Events;                      // 0x20
    PAD(0x218);                                                        // 0x38
    TMaxArray<FrameGraphInternal::SEventDescriptor*, 16> m_EventStack; // 0x250
};

struct SD3D12ObjectPools {
    PAD(0xC0);                                                          // 0x0
    TObjectPool<ZRenderPipelineState> RenderPipelineStates;             // 0xC0
    TObjectPool<ZRenderInputLayout> RenderInputLayouts;                 // 0x138
    PAD(0x48);                                                          // 0x1B0
    TObjectPool<ZRenderDepthStencilState> RenderDepthStencilStates;     // 0x1F8
    PAD(0x48);                                                          // 0x270
    TObjectPool<ZRenderBlendState> RenderBlendStates;                   // 0x2B8
    PAD(0x48);                                                          // 0x330
    TObjectPool<ZRenderRasterizerState> RenderRasterizerStates;         // 0x378
    PAD(0x108);                                                         // 0x3F0
    TObjectPool<ZRenderTexture2D> RenderTexture2Ds;                     // 0x4F8
    TObjectPool<ZRenderTexture3D> RenderTexture3Ds;                     // 0x570
    TObjectPool<ZRenderBuffer> RenderBuffers;                           // 0x5E8
    TObjectPool<ZRenderTargetView> RenderTargetViews;                   // 0x660
    TObjectPool<ZRenderDepthStencilView> RenderDepthStencilViews;       // 0x6D8
    TObjectPool<ZRenderShaderResourceView> RenderShaderResourceViews;   // 0x750
    TObjectPool<ZRenderUnorderedAccessView> RenderUnorderedAccessViews; // 0x7C8
};

struct SPassExecutionContext {
    ZFrameGraph* m_pFrameGraph;                       // 0x0
    FrameGraphInternal::SPassNode* m_pPassNode;       // 0x8
    ZRenderDeviceContext* m_pRenderDeviceContexts[8]; // 0x10
};

struct SPassExecution {
    SPassExecutionContext* m_pPassExecutionContext; // 0x0
};

class IRenderPrimitive : public TRenderReferencedCountedImpl<IRenderRefCount, false> {
  public:
    PAD(0x20);                                         // 0x10
    TResourcePtr<ZRenderMaterialInstance> m_pMaterial; // 0x30
    PAD(0x30);                                         // 0x38
    uint32_t m_nPrimDrawDataIndex;                     // 0x68
};

struct SRenderPrimitiveMeshDesc {
    uint32_t nNumVertices;     // 0x0
    uint32_t mNumIndices;      // 0x4
    PAD(0x10);                 // 0x8
    uint8_t nNumStreams;       // 0x18
    uint8_t anStreamStride[5]; // 0x19
};

class ZRenderGeometryBuffer {
  public:
    virtual ~ZRenderGeometryBuffer() = default;

    PAD(0x08);                   // 0x08
    uint32_t m_nSize;            // 0x10
    PAD(0xC);                    // 0x14
    uint32_t m_nStride;          // 0x20
    PAD(0x8);                    // 0x24
    uint32_t m_nOffset;          // 0x2C
    PAD(0x8);                    // 0x30
    ID3D12Resource* m_pResource; // 0x38
    PAD(0x18);                   // 0x40
    char* m_pCPUBuffer;          // 0x58
    PAD(0x10);                   // 0x60
};

class ZRenderVertexBuffer : public ZRenderGeometryBuffer {};

class ZRenderIndexBuffer : public ZRenderGeometryBuffer {};

struct SPrimDrawData {
    SRenderPrimitiveMeshDesc m_Description;          // 0x0
    float4 vTextureScaleBias;                        // 0x20
    float4 vPositionScale;                           // 0x30
    float4 vPositionBias;                            // 0x40
    uint32_t nNumIndicesToDraw;                      // 0x50
    PAD(0xC);                                        // 0x54
    ZRenderInputLayout* m_pInputLayout;              // 0x60
    ZRenderIndexBuffer* m_pIndexBuffer;              // 0x68
    ZRenderVertexBuffer* m_pVertexBuffers[5];        // 0x70
    ZUploadBufferResource* m_pUploadBufferResource0; // 0x98
    ZUploadBufferResource* m_pUploadBufferResource1; // 0xA0
    IRenderPrimitive* m_Primitive;                   // 0xA8
};

class ZRenderPrimitiveResource {
  public:
    virtual ~ZRenderPrimitiveResource() = 0;

    TArray<TRefCountPtr<IRenderPrimitive>> m_Primitives;
    SVector3 m_vMin;
    SVector3 m_vMax;
};
