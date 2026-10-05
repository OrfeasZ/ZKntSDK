#pragma once

#include "ZItem.hpp"
#include "Gameplay.hpp"

class ZGadgetItemDefinition : public ZItemCharacterDefinitionBase {};

class IGadgetCollectionProvider {
  public:
    virtual ~IGadgetCollectionProvider() = 0;
};
