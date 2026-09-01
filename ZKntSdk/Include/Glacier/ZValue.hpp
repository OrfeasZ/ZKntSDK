#pragma once

#include "ZEntity.hpp"

class IValueChanged : public IComponentInterface {};

class IReplicatedCLValue : public IComponentInterface {};

class ZCLValue : public ZEntityImpl, public IValueChanged {};

class ZCLSimpleValue : public ZCLValue, public IReplicatedCLValue {
  public:
    PAD(0x8);
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

class IEntityRefValue : public IComponentInterface {
  public:
    virtual ZEntityRef* GetValue(ZEntityRef& result) = 0;
};

template<typename T> class ITEntityRefValue : public IEntityRefValue {};

class ZCLValueEntityRefEntity : public ZCLValue, public IEntityRefValue, public IReplicatedCLValue {
  public:
    ZEntityRef m_vValue; // 0x30
    PAD(0x10);           // 0x40
    bool m_isReplicated; // 0x50
};
