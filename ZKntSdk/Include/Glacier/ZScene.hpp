#pragma once

#include "ZString.hpp"
#include "IComponentInterface.hpp"
#include "ZEntity.hpp"

class ISceneEntity;

class IEntitySceneContext : public IComponentInterface {};

class ZEntitySceneContext : public IEntitySceneContext {
  public:
    PAD(0x8);
    TEntityRef<ISceneEntity> m_pScene; // 0x10
    ZString m_SceneResource;           // 0x28
};

class ZSceneLifecycleListener {
  public:
    virtual ~ZSceneLifecycleListener() = 0;
};
