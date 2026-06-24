#pragma once

#include "ZSpatialEntity.hpp"

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
