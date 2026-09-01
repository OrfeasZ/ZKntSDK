#pragma once

#include "ZSpatialEntity.hpp"
#include "ZValue.hpp"

class ZHumanoidOutfitEntity;
class IAnimationNetwork;
class ZAudioEmitterEntity;

class ICharacterEntity : public IComponentInterface {
    virtual ~ICharacterEntity() = 0;
};

class ICrowdCoreProvider : public IComponentInterface {
    virtual ~ICrowdCoreProvider() = 0;
};

class IBoneCollidable {
    virtual ~IBoneCollidable() = 0;
};

class ISpawnableEntity {
    virtual ~ISpawnableEntity() = 0;
};

class IClothProvider : public IComponentInterface {
    virtual ~IClothProvider() = 0;
};

class IAnimationPreviewTarget {
  public:
    virtual ~IAnimationPreviewTarget() = 0;
};

class ZHumanoidCharacterEntity : public ZSpatialEntity,
                                 public ICharacterEntity,
                                 public ICrowdCoreProvider,
                                 public IAnimationPreviewTarget,
                                 public IBoneCollidable,
                                 public ISpawnableEntity,
                                 public IClothProvider {
    virtual ~ZHumanoidCharacterEntity() = 0;

  public:
    PAD(0x50);                                                     // 0xD8
    TEntityRef<IAnimationNetwork> m_rAnimationNetwork;             // 0x128
    TEntityRef<ZAudioEmitterEntity> m_rMouthAudioEmitter;          // 0x140
    TEntityRef<ZAudioEmitterEntity> m_rLeftFootstepAudioEmitter;   // 0x158
    TEntityRef<ZAudioEmitterEntity> m_rRightFootstepAudioEmitter;  // 0x170
    TEntityRef<ZAudioEmitterEntity> m_rCharacterFoleyAudioEmitter; // 0x188
    TEntityRef<ZAudioEmitterEntity> m_rCloseCombatAudioEmitter;    // 0x1A0
    TEntityRef<ZSpatialEntity> m_pHeadSpatial;                     // 0x1B8
    ZResourcePtr m_pFaceFXActor;                                   // 0x1D0
    ZString m_faceRootBoneName;                                    // 0x1D8
    bool m_deferredAnimationInitialization;                        // 0x1E8
    TResourcePtr<ZEntityRef> m_definition;                         // 0x1EC
    TResourcePtr<ZEntityRef> m_outfitSet;                          // 0x1F4
    int32 m_selectedOutfit;                                        // 0x1FC
    uint32_t m_StoreHandle;                                        // 0x200
    PAD(0xCC);                                                     // 0x204
    TEntityRef<ZHumanoidOutfitEntity> m_outfit;                    // 0x2D0
    PAD(0x120);                                                    // 0x2E8
};

class ZCLTeleportHumanoidEntity : public ZEntityImpl {};

class ZCLEnableDisableHumanoidCollision : public ZEntityImpl {};

class ZCLEnableDisableHumanoidDefenses : public ZEntityImpl {};

class ZCLSetHumanoidImmuneToDamage : public ZEntityImpl {};

class ZCLSetHumanoidUnkillableByDamage : public ZEntityImpl {};

class ZCLSetHumanoidInfiniteClipAmmo : public ZEntityImpl {};

class ZHumanoidOutfitReference : public ZEntityImpl {
  public:
    ZRuntimeResourceID m_outfit; // 0x18
};

class ZHumanoidOutfitSet : public ZEntityImpl {
  public:
    TArray<TInterfaceRef<ZHumanoidOutfitReference>> m_outfits; // 0x18
};

class ZCLGetHumanoidFromOutfitEntity : public ZEntityImpl, public ITEntityRefValue<ZHumanoidCharacterEntity> {
  public:
    TInterfaceRef<ITEntityRefValue<ZHumanoidOutfitEntity>> m_outfit; // 0x20
};

class ZCLGetOutfitRefFromOutfit : public ZEntityImpl, public ITEntityRefValue<ZHumanoidOutfitEntity> {
  public:
    TInterfaceRef<ZHumanoidOutfitEntity> m_outfit; // 0x20
};

class ZCLGetHumanoidHealth : public ZEntityImpl, public IFloatValue {
  public:
    TInterfaceRef<IEntityRefValue> m_rTarget; // 0x20
};

class ZCLGetHumanoidMaxHealth : public ZEntityImpl, public IFloatValue {
  public:
    TInterfaceRef<IEntityRefValue> m_rTarget; // 0x20
};

class ZCLGetHumanoidRefFromEntityRef : public ZEntityImpl, public ITEntityRefValue<ZHumanoidCharacterEntity> {
  public:
    TInterfaceRef<IEntityRefValue> m_entityRef; // 0x20
};

class ZCLGetCharacterRefFromEntityRef : public ZEntityImpl, public ITEntityRefValue<ICharacterEntity> {
  public:
    TEntityRef<IEntityRefValue> m_entityRef; // 0x20
};

class ZCLIsHumanoidAlive : public ZCLValue, public IBoolValue {
  public:
    TInterfaceRef<IEntityRefValue> m_rTarget; // 0x28
    PAD(0x8);                                 // 0x38
};

class ZCLIsHumanoidDead : public ZCLValue, public IBoolValue {
  public:
    TInterfaceRef<IEntityRefValue> m_rTarget; // 0x28
    PAD(0x8);                                 // 0x38
};

enum class EFirearmDisarmType : int32_t {
    EquippedOnly = 0,
    AllAttached = 1,
};
