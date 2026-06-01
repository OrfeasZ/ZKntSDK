#pragma once

#include "ZEntity.hpp"

class IRenderDestinationEntity : public IComponentInterface {
  public:
    virtual ZEntityRef& GetSource() = 0;
    virtual void IRenderDestinationEntity_unk6() = 0;
    virtual void IRenderDestinationEntity_unk7() const = 0;
    virtual void IRenderDestinationEntity_unk8() = 0;
    virtual void SetSource(ZEntityRef& rSource) = 0;
};

class IRenderDestinationSource : public IComponentInterface {
  public:
    virtual ~IRenderDestinationSource() = 0;
};
