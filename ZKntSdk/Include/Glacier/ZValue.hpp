#pragma once

#include "ZEntity.hpp"

class IValueChanged : public IComponentInterface {};

class IReplicatedCLValue : public IComponentInterface {};

class ZCLValue : public ZEntityImpl, public IValueChanged {
  public:
};

class ZCLSimpleValue : public ZCLValue, public IReplicatedCLValue {
  public:
};

class IBoolValue : public IComponentInterface {
  public:
    virtual bool GetValue() = 0;
};

class ZCLValueBoolEntity : public ZCLSimpleValue, public IBoolValue {};

class IFloatValue : public IComponentInterface {
  public:
    virtual float GetValue() = 0;
};

class ZCLValueFloatEntity : public ZCLSimpleValue, public IFloatValue {};

class IIntValue : public IComponentInterface {
  public:
    virtual int32_t GetValue() = 0;
};

class ZCLValueIntEntity : public ZCLSimpleValue, public IIntValue {};
