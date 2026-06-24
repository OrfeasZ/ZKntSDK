#pragma once

#include "ZEntity.hpp"

class IBoolValue : public IComponentInterface {
  public:
    virtual bool GetValue() = 0;
};

class ZCLValueBoolEntity : public ZEntityImpl {};

class IFloatValue : public IComponentInterface {
  public:
    virtual float GetValue() = 0;
};

class IIntValue : public IComponentInterface {
  public:
    virtual int32_t GetValue() = 0;
};
