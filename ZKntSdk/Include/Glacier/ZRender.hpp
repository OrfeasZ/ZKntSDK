#pragma once

#include <directx/d3d12.h>
#include <dxgi.h>

#include "ZEntity.hpp"
#include "ZScene.hpp"
#include "TObjectPool.hpp"
#include "TRefCountPtr.hpp"
#include "ZRenderableEntity.hpp"
#include "ZValue.hpp"

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
class ZVertexDataResource;
class ZAnimationBoneData;
class ZBodyPartEntity;
class IBoneAnimator;
class IBoneWeightManagerEntity;
class ZRoomManagerRender;

enum ERenderFormat : int16 {
    RENDER_FORMAT_NONE = 0x0,
    RENDER_FORMAT_UNKNOWN = 0x0,
    RENDER_FORMAT_R32G32B32A32_TYPELESS = 0x1,
    RENDER_FORMAT_R32G32B32A32_FLOAT = 0x2,
    RENDER_FORMAT_R32G32B32A32_UINT = 0x3,
    RENDER_FORMAT_R32G32B32A32_SINT = 0x4,
    RENDER_FORMAT_R32G32B32_TYPELESS = 0x5,
    RENDER_FORMAT_R32G32B32_FLOAT = 0x6,
    RENDER_FORMAT_R32G32B32_UINT = 0x7,
    RENDER_FORMAT_R32G32B32_SINT = 0x8,
    RENDER_FORMAT_R16G16B16A16_TYPELESS = 0x9,
    RENDER_FORMAT_R16G16B16A16_FLOAT = 0xA,
    RENDER_FORMAT_R16G16B16A16_UNORM = 0xB,
    RENDER_FORMAT_R16G16B16A16_UINT = 0xC,
    RENDER_FORMAT_R16G16B16A16_SNORM = 0xD,
    RENDER_FORMAT_R16G16B16A16_SINT = 0xE,
    RENDER_FORMAT_R32G32_TYPELESS = 0xF,
    RENDER_FORMAT_R32G32_FLOAT = 0x10,
    RENDER_FORMAT_R32G32_UINT = 0x11,
    RENDER_FORMAT_R32G32_SINT = 0x12,
    RENDER_FORMAT_R32G8X24_TYPELESS = 0x13,
    RENDER_FORMAT_D32_FLOAT_S8X24_UINT = 0x14,
    RENDER_FORMAT_R32_FLOAT_X8X24_TYPELESS = 0x15,
    RENDER_FORMAT_X32_TYPELESS_G8X24_UINT = 0x16,
    RENDER_FORMAT_R10G10B10A2_TYPELESS = 0x17,
    RENDER_FORMAT_R10G10B10A2_UNORM = 0x18,
    RENDER_FORMAT_R10G10B10A2_UINT = 0x19,
    RENDER_FORMAT_R11G11B10_FLOAT = 0x1A,
    RENDER_FORMAT_R8G8B8A8_TYPELESS = 0x1B,
    RENDER_FORMAT_R8G8B8A8_UNORM = 0x1C,
    RENDER_FORMAT_R8G8B8A8_UNORM_SRGB = 0x1D,
    RENDER_FORMAT_R8G8B8A8_UINT = 0x1E,
    RENDER_FORMAT_R8G8B8A8_SNORM = 0x1F,
    RENDER_FORMAT_R8G8B8A8_SINT = 0x20,
    RENDER_FORMAT_B8G8R8A8_TYPELESS = 0x21,
    RENDER_FORMAT_B8G8R8A8_UNORM = 0x22,
    RENDER_FORMAT_B8G8R8A8_UNORM_SRGB = 0x23,
    RENDER_FORMAT_R16G16_TYPELESS = 0x24,
    RENDER_FORMAT_R16G16_FLOAT = 0x25,
    RENDER_FORMAT_R16G16_UNORM = 0x26,
    RENDER_FORMAT_R16G16_UINT = 0x27,
    RENDER_FORMAT_R16G16_SNORM = 0x28,
    RENDER_FORMAT_R16G16_SINT = 0x29,
    RENDER_FORMAT_R32_TYPELESS = 0x2A,
    RENDER_FORMAT_D32_FLOAT = 0x2B,
    RENDER_FORMAT_R32_FLOAT = 0x2C,
    RENDER_FORMAT_R32_UINT = 0x2D,
    RENDER_FORMAT_R32_SINT = 0x2E,
    RENDER_FORMAT_R24G8_TYPELESS = 0x2F,
    RENDER_FORMAT_D24_UNORM_S8_UINT = 0x30,
    RENDER_FORMAT_R24_UNORM_X8_TYPELESS = 0x31,
    RENDER_FORMAT_X24_TYPELESS_G8_UINT = 0x32,
    RENDER_FORMAT_R9G9B9E5_SHAREDEXP = 0x33,
    RENDER_FORMAT_R8G8_B8G8_UNORM = 0x34,
    RENDER_FORMAT_G8R8_G8B8_UNORM = 0x35,
    RENDER_FORMAT_R8G8_TYPELESS = 0x36,
    RENDER_FORMAT_R8G8_UNORM = 0x37,
    RENDER_FORMAT_R8G8_UINT = 0x38,
    RENDER_FORMAT_R8G8_SNORM = 0x39,
    RENDER_FORMAT_R8G8_SINT = 0x3A,
    RENDER_FORMAT_R16_TYPELESS = 0x3B,
    RENDER_FORMAT_R16_FLOAT = 0x3C,
    RENDER_FORMAT_D16_UNORM = 0x3D,
    RENDER_FORMAT_R16_UNORM = 0x3E,
    RENDER_FORMAT_R16_UINT = 0x3F,
    RENDER_FORMAT_R16_SNORM = 0x40,
    RENDER_FORMAT_R16_SINT = 0x41,
    RENDER_FORMAT_B5G6R5_UNORM = 0x42,
    RENDER_FORMAT_B5G5R5A1_UNORM = 0x43,
    RENDER_FORMAT_R8_TYPELESS = 0x44,
    RENDER_FORMAT_R8_UNORM = 0x45,
    RENDER_FORMAT_R8_UINT = 0x46,
    RENDER_FORMAT_R8_SNORM = 0x47,
    RENDER_FORMAT_R8_SINT = 0x48,
    RENDER_FORMAT_A8_UNORM = 0x49,
    RENDER_FORMAT_R1_UNORM = 0x4A,
    RENDER_FORMAT_BC1_TYPELESS = 0x4B,
    RENDER_FORMAT_BC1_UNORM = 0x4C,
    RENDER_FORMAT_BC1_UNORM_SRGB = 0x4D,
    RENDER_FORMAT_BC2_TYPELESS = 0x4E,
    RENDER_FORMAT_BC2_UNORM = 0x4F,
    RENDER_FORMAT_BC2_UNORM_SRGB = 0x50,
    RENDER_FORMAT_BC3_TYPELESS = 0x51,
    RENDER_FORMAT_BC3_UNORM = 0x52,
    RENDER_FORMAT_BC3_UNORM_SRGB = 0x53,
    RENDER_FORMAT_BC4_TYPELESS = 0x54,
    RENDER_FORMAT_BC4_UNORM = 0x55,
    RENDER_FORMAT_BC4_SNORM = 0x56,
    RENDER_FORMAT_BC5_TYPELESS = 0x57,
    RENDER_FORMAT_BC5_UNORM = 0x58,
    RENDER_FORMAT_BC5_SNORM = 0x59,
    RENDER_FORMAT_BC6H_TYPELESS = 0x5A,
    RENDER_FORMAT_BC6H_UF16 = 0x5B,
    RENDER_FORMAT_BC6H_SF16 = 0x5C,
    RENDER_FORMAT_BC7_TYPELESS = 0x5D,
    RENDER_FORMAT_BC7_UNORM = 0x5E,
    RENDER_FORMAT_BC7_UNORM_SRGB = 0x5F,
    NUM_RENDER_FORMATS = 0x60
};

enum ERenderResourceType {
    RENDER_RESOURCE_TYPE_TEXTURE2D = 1,
    RENDER_RESOURCE_TYPE_TEXTURE3D = 2,
    RENDER_RESOURCE_TYPE_BUFFER = 3,
};

template<class T, bool U> class TRenderReferencedCountedImpl : public T {
  public:
    int32_t m_ReferenceCount;
};

template<typename T, ERenderResourceType ResourceType> class TRenderResourceImpl : public TRenderReferencedCountedImpl<T, false> {};

class IRenderRefCount {
  public:
    virtual ~IRenderRefCount() = 0;
    virtual void AddRef() = 0;
    virtual uint32_t Release() = 0;
    virtual int32_t GetRefCount() const = 0;
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

struct SRenderTexture2DDesc {
    uint32 nWidth;         // 0x0
    uint32 nHeight;        // 0x4
    uint32 nMipLevels;     // 0x8
    PAD(0xE);              // 0xC
    uint16 nArraySize;     // 0x1A
    ERenderFormat eFormat; // 0x1C
    PAD(0x10);             // 0x20
};

class IRenderResource : public IRenderRefCount {
  public:
    virtual ERenderResourceType GetResourceType() const = 0;
};

class IRenderResourceD3D12 : public IRenderResource {
  public:
    ID3D12Resource* m_pResource; // 0x08

    uint32_t m_Unknown10; // 0x10
    uint32_t m_Unknown14; // 0x14

    uint64_t m_Unknown18; // 0x18
    uint64_t m_Unknown20; // 0x20
    uint64_t m_Unknown28; // 0x28
};

static_assert(sizeof(IRenderResourceD3D12) == 0x30);

class ZRenderTexture2D : public TRenderResourceImpl<IRenderResourceD3D12, ERenderResourceType::RENDER_RESOURCE_TYPE_TEXTURE2D> {
  public:
    virtual ~ZRenderTexture2D() = 0;

  public:
    SRenderTexture2DDesc m_Description; // 0x38
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
    PAD(0x4AAEF98);                             // 0x6B8
    ID3D12CommandQueue* m_pCommandQueue;        // 0x4AAF650
    PAD(0x21D8);                                // 0x4AAF658
    ID3D12DescriptorHeap* m_pDescriptorHeapDSV; // 0x4AB1830
};

class ZRenderSharedResources {
  public:
    PAD(0xF0);                                                           // 0x0
    ZRenderTexture2D* m_pBoxReflectionCubeTexture[2];                    // 0xF0
    ZRenderTexture2D* m_pBoxReflectionDiffuseCubeTexture[2];             // 0x100
    ZRenderShaderResourceView* m_pBoxReflectionCubeTextureSRV[2];        // 0x110
    ZRenderShaderResourceView* m_pBoxReflectionDiffuseCubeTextureSRV[2]; // 0x120
    PAD(0x38);                                                           // 0x130
    int32_t m_nBoxReflectionCubeRenderTargetChunks;                      // 0x168
    PAD(0x561C);                                                         // 0x16C
    uint32 m_nBoxReflectionMaxCubeMaps;                                  // 0x5788
    uint32 m_nBoxReflectionResolution;                                   // 0x578C
    uint32_t m_nBoxReflectionDiffuseResolution;                          // 0x5790
    uint32 m_nBoxReflectionRenderResolution;                             // 0x5794
    uint32 m_nBoxReflectionMips;                                         // 0x5798
    uint32 m_nBoxReflectionUseBC6;                                       // 0x579C
    PAD(0x3878);                                                         // 0x57A0
    bool m_SplitBoxReflectionCubeRenderTargets;                          // 0x9018
};

class IRenderManager : public IComponentInterface {};

class ZRenderManager : public IRenderManager, public ZSceneLifecycleListener {
  public:
    virtual ~ZRenderManager() = default;

  public:
    PAD(0x17C50);                               // 0x10
    ZRenderDevice* m_pRenderDevice;             // 0x17C60
    PAD(0x8);                                   // 0x17C68
    ZRenderSharedResources* m_pSharedResources; // 0x17C70
    PAD(0xA7D0);                                // 0x17C78
    ZRoomManagerRender* m_pRoomManagerRender;   // 0x22448
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

class IRenderPrimitiveMesh {
  public:
    virtual ~IRenderPrimitiveMesh() = 0;
};

class ZRenderPrimitiveMeshBase : public IRenderPrimitiveMesh {
  public:
    virtual ~ZRenderPrimitiveMeshBase() = 0;
};

class ZRenderPrimitive : public IRenderPrimitive {
  public:
    virtual ~ZRenderPrimitive() = 0;
};

class ZRenderPrimitiveMesh : public ZRenderPrimitiveMeshBase, public ZRenderPrimitive {
  public:
    virtual ~ZRenderPrimitiveMesh() = 0;
};

class IRenderPrimitiveListener : public IComponentInterface {};

class ZPrimitiveContainerEntity : public ZRenderableEntity, public IRenderPrimitiveListener {
  public:
    PAD(0x6C);                    // 0xF0
    float32 m_fLODScale;          // 0x15C
    float32 m_fLODOffset;         // 0x160
    PAD(0x2);                     // 0x164
    bool m_bCastShadows;          // 0x166
    bool m_bCastContactShadows;   // 0x167
    bool m_bDisableStaticShadows; // 0x168
    bool m_bCustomShaderData;     // 0x169
    bool m_bStaticSDF;            // 0x16A
    PAD(0x15);                    // 0x16B
};

enum class ESeamFixMode : int8_t {
    SEAMFIX_NONE = 0,
    SEAMFIX_X = 1,
    SEAMFIX_Y = 2,
    SEAMFIX_Z = 4,
    SEAMFIX_XZ = 5,
    SEAMFIX_XY = 3,
    SEAMFIX_XYZ = 7,
    SEAMFIX_YZ = 6
};

class ZGeomEntity : public ZPrimitiveContainerEntity {
  public:
    TResourcePtr<ZVertexDataResource> m_VertexPaintData; // 0x180
    ZString m_sVertexPaintSourceResourceId;              // 0x188
    ZResourcePtr m_ResourceID;                           // 0x198
    SVector3 m_PrimitiveScale;                           // 0x1A0
    ESeamFixMode m_eSeamFix;                             // 0x1AC
    PAD(0x13);                                           // 0x1AD
};

class ZLinkedEntity : public ZGeomEntity, public ITEntityRefValue<ZLinkedEntity> {
  public:
    TResourcePtr<ZAnimationBoneData> m_pBonesAndCollisionResource;  // 0x1C8
    TArray<TEntityRef<ZBodyPartEntity>> m_aBodyParts;               // 0x1D0
    SVector3 m_vBodypartBoundingMarginMax;                          // 0x1E8
    SVector3 m_vBodypartBoundingMarginMin;                          // 0x1F4
    uint32 m_nBodypartLODCutOff;                                    // 0x200
    ZResourcePtr m_BodyPartLODCutOffMeshResource;                   // 0x204
    TEntityRef<IBoneAnimator> m_BoneAnimator;                       // 0x210
    TEntityRef<IBoneWeightManagerEntity> m_BoneWeightManagerEntity; // 0x228
    PAD(0x120);                                                     // 0x240
};

class ZVTablePaddingRemover {
  public:
    virtual ~ZVTablePaddingRemover() = 0;
};

class ZRenderGraphNode : public ZVTablePaddingRemover {
  public:
    enum TYPE : uint8 {
        GEOM = 0,
        LINKED = 1,
        PARTICLEEMITTER = 2,
        TERRAIN = 3,
        SPATIAL = 4,
        LIGHT = 5,
        CAMERA = 6,
        MATERIAL = 7,
        POSTFILTER = 8,
        DESTINATION = 9,
        VIDEO_PLAYER = 10,
        UI = 11,
        CROWDENTITY = 12,
        SCATTER = 13,
        MIRROR = 14,
        BOXREFLECTION = 15,
        RENDERGLOBAL = 16,
        GLOBALLIGHT = 17,
        ATMOSPHERICSCATTERING = 18,
        VOLUMETRICLIGHTING = 19,
        FOGVOLUME = 20,
        SMOLDERVOLUME = 21,
        SMOLDERDYNAMICVOLUME = 22,
        SKINSETTINGS = 23,
        CINEMATICVOLUME = 24,
        TYPE_SIZE = 25,
        RENDERABLE_TYPE_MASK = 15,
        RENDERABLE_TYPE_FIRST = GEOM,
        RENDERABLE_TYPE_LAST = TERRAIN
    };

    PAD(0x8);                               // 0x8
    ZRenderableEntity* m_pRenderableEntity; // 0x10
    PAD(0x8);                               // 0x18
    int32 m_Base;                           // 0x20
    PAD(0xA);                               // 0x24
    TYPE m_nType;                           // 0x2E
};

class ZRenderGraphNodeBoxReflection : public ZRenderGraphNode {
  public:
    PAD(0x54);   // 0x30
    int32 m_nId; // 0x84
};

class IRenderGraphManager : public IComponentInterface {};

class ZRenderGraphManager : public IRenderGraphManager {
  public:
    PAD(0x9E8);                                                      // 0x8
    TMaxArray<ZRenderGraphNodeBoxReflection*, 682> m_BoxReflections; // 0x9F0
};
