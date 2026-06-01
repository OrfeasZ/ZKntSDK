#pragma once

#include "Common.hpp"
#include "ZScene.hpp"

class IModule : public IComponentInterface {};

class ZSimpleModuleBase : public IModule {};

class ZGameSceneflowModule : public ZSimpleModuleBase {
  public:
    PAD(0x58);
    ZEntitySceneContext* m_pEntitySceneContext; // 0x60
};
