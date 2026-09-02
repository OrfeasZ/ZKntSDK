#pragma once

#include "ZEntity.hpp"

class ZRenderMaterialInstance;
class ZPhysicsObjectProxy;

enum class ERayDetailLevel : int8_t { RAYDETAILS_NONE = 0, RAYDETAILS_BONES = 1, RAYDETAILS_MESH = 2 };

enum class ECollidablesType : int8_t { ECST_STATIC_AND_DYNAMIC = 0, ECST_STATIC_ONLY = 1, ECST_DYNAMIC_ONLY = 2 };

enum class EObjectCollisionChannel : int32_t {
    eOCC_Static = 0,
    eOCC_Dynamic = 1,
    eOCC_Character = 2,
    eOCC_GameObject = 3,
    eOCC_Vehicle = 4,
    eOCC_Weapon = 5,
    eOCC_Item = 6,
    eOCC_Projectile = 7,
    eOCC_Interaction = 8,
    eOCC_Camera = 9,
    eOCC_Structure = 10,
    eOCC_Cosmetic = 11,
    eOCC_Vision = 13,
    eOCC_Vision_LowAttention = 14,
    eOCC_BlastField = 15
};

enum class EObjectCollisionAttribute : int32_t {
    eOCA_Transparent = 0,
    eOCA_IgnoredByCamera = 1,
    eOCA_KinematicContactNotification = 2,
    eOCA_SimulationFilterCallback = 3,
    eOCA_Destructible = 4,
    eOCA_StairsSlope = 5,
    eOCA_Player = 6,
    eOCA_OnlyBlockPlayer = 7,
    eOCA_Marker = 8,
    eOCA_IgnoreMarker = 9,
    eOCA_KinematicCharacterCapsule = 10,
    eOCA_AudioTransparent = 11,
    eOCA_ExcludedFromPhysicsTrigger = 12,
    eOCA_IgnoredByAgility = 13,
    eOCA_IgnoredByVehicleWheels = 14,
    eOCA_InvisibleWall = 15
};

class ZPhysicsObjectRef {
    ZPhysicsObjectProxy* m_pProxy;
};

class ZRayQueryInput {
  public:
    uint16_t m_BlockingChannelMask;                                           // 0x0
    uint16_t m_OverlapChannelMask;                                            // 0x2
    uint16_t m_RequiredAttributeMask;                                         // 0x4
    uint16_t m_ForbiddenAttributeMask;                                        // 0x6
    ECollidablesType m_eRayCollidables;                                       // 0x8
    PAD(0x7);                                                                 // 0x9
    void* m_Unk1;                                                             // 0x10
    THashSet<ZEntityRef>* m_pIgnoredEntities;                                 // 0x18
    uint16_t m_TypedQueryMask;                                                // 0x20
    float4 m_vFrom;                                                           // 0x30
    float4 m_vTo;                                                             // 0x40
    ZDelegate<bool(ZEntityRef, TEntityRef<ZSpatialEntity>)> m_FilterCallback; // 0x50
    ERayDetailLevel m_eRayDetailLevel;                                        // 0x70
    PAD(0x7);                                                                 // 0x71
    uint8_t m_Unk2;                                                           // 0x78
    uint32_t m_Unk3;                                                          // 0x7C
};

static_assert(sizeof(ZRayQueryInput) == 0x80);
static_assert(alignof(ZRayQueryInput) == 0x10);

static_assert(offsetof(ZRayQueryInput, m_BlockingChannelMask) == 0x00);
static_assert(offsetof(ZRayQueryInput, m_OverlapChannelMask) == 0x02);
static_assert(offsetof(ZRayQueryInput, m_RequiredAttributeMask) == 0x04);
static_assert(offsetof(ZRayQueryInput, m_ForbiddenAttributeMask) == 0x06);
static_assert(offsetof(ZRayQueryInput, m_eRayCollidables) == 0x08);
static_assert(offsetof(ZRayQueryInput, m_Unk1) == 0x10);
static_assert(offsetof(ZRayQueryInput, m_pIgnoredEntities) == 0x18);
static_assert(offsetof(ZRayQueryInput, m_TypedQueryMask) == 0x20);
static_assert(offsetof(ZRayQueryInput, m_vFrom) == 0x30);
static_assert(offsetof(ZRayQueryInput, m_vTo) == 0x40);
static_assert(offsetof(ZRayQueryInput, m_FilterCallback) == 0x50);
static_assert(offsetof(ZRayQueryInput, m_eRayDetailLevel) == 0x70);
static_assert(offsetof(ZRayQueryInput, m_Unk2) == 0x78);
static_assert(offsetof(ZRayQueryInput, m_Unk3) == 0x7C);

class ZRayQueryOutput {
  public:
    float4 m_vPosition;                                                  // 0x0
    float4 m_vNormal;                                                    // 0x10
    float32 m_nT;                                                        // 0x20
    TResourcePtr<ZRenderMaterialInstance> m_pBlockingMaterialDescriptor; // 0x24
    TResourcePtr<ZRenderMaterialInstance> m_pBlockingMaterial;           // 0x2C
    PAD(0x4);                                                            // 0x34
    ZEntityRef m_BlockingEntity;                                         // 0x38
    ZPhysicsObjectRef m_pBlockingPhysicsObject;                          // 0x48
    TEntityRef<ZSpatialEntity> m_pBlockingSpatialEntity;                 // 0x50
    PAD(0x48);                                                           // 0x68
};

static_assert(sizeof(ZRayQueryOutput) == 0xB0);
static_assert(alignof(ZRayQueryOutput) == 0x10);

static_assert(offsetof(ZRayQueryOutput, m_vPosition) == 0x00);
static_assert(offsetof(ZRayQueryOutput, m_vNormal) == 0x10);
static_assert(offsetof(ZRayQueryOutput, m_nT) == 0x20);
static_assert(offsetof(ZRayQueryOutput, m_pBlockingMaterialDescriptor) == 0x24);
static_assert(offsetof(ZRayQueryOutput, m_pBlockingMaterial) == 0x2C);
static_assert(offsetof(ZRayQueryOutput, m_BlockingEntity) == 0x38);
static_assert(offsetof(ZRayQueryOutput, m_pBlockingPhysicsObject) == 0x48);
static_assert(offsetof(ZRayQueryOutput, m_pBlockingSpatialEntity) == 0x50);

class ZCollisionQueryPreset : public ZEntityImpl {
  public:
    bool m_bTypedQuery;                                       // 0x18
    EObjectCollisionChannel m_eCollisionChannel;              // 0x1C
    TArray<EObjectCollisionChannel> m_aOverlapChannels;       // 0x20
    TArray<EObjectCollisionChannel> m_aBlockingChannels;      // 0x38
    TArray<EObjectCollisionAttribute> m_aRequiredAttributes;  // 0x50
    TArray<EObjectCollisionAttribute> m_aForbiddenAttributes; // 0x68
    ECollidablesType m_eCollidableTypes;                      // 0x80
    PAD(0x7);                                                 // 0x81
    uint16_t m_BlockingChannelMask;                           // 0x88
    uint16_t m_OverlapChannelMask;                            // 0x8A
    uint16_t m_RequiredAttributeMask;                         // 0x8C
    uint16_t m_ForbiddenAttributeMask;                        // 0x8E
    ECollidablesType m_eCollidableTypes2;                     // 0x90
    void* m_Unk;                                              // 0x98
    THashSet<ZEntityRef>* m_pIgnoredEntities;                 // 0xA0
    uint16_t m_TypedQueryMask;                                // 0xA8
};

class ICollisionManager : public IComponentInterface {
  public:
    virtual void ICollisionManager_Unk5() = 0;
    virtual void ICollisionManager_Unk6() = 0;
    virtual void ICollisionManager_Unk7() = 0;
    virtual void ICollisionManager_Unk8() = 0;
    virtual void ICollisionManager_Unk9() = 0;
    virtual void ICollisionManager_Unk10() = 0;
    virtual void ICollisionManager_Unk11() = 0;
    virtual void ICollisionManager_Unk12() = 0;
    virtual void ICollisionManager_Unk13() = 0;
    virtual void ICollisionManager_Unk14() = 0;
    virtual void ICollisionManager_Unk15() = 0;
    virtual void ICollisionManager_Unk16() = 0;
    virtual void ICollisionManager_Unk17() = 0;
    virtual void ICollisionManager_Unk18() = 0;
    virtual void ICollisionManager_Unk19() = 0;
    virtual void ICollisionManager_Unk20() = 0;
    virtual void ICollisionManager_Unk21() = 0;
    virtual void ICollisionManager_Unk22() = 0;
    virtual bool RayCastClosestHit(ZRayQueryOutput* pOutput, const ZRayQueryInput& sInput) = 0;
};

class ZCollisionManager : public ICollisionManager, public ZSceneLifecycleListener {};
