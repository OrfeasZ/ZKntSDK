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

class IAnimationPreviewTarget : public IBoneCollidable, public ISpawnableEntity, public IClothProvider {
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
