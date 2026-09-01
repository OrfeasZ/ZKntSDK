#pragma once

#include "ZEntity.hpp"

class ZGameplayTagListEntity : public ZEntityImpl {
  public:
    TArray<uint16> Tags; // 0x18
};
