#pragma once

#include "IComponentInterface.hpp"
#include "ZEntity.hpp"
#include "ZRenderableEntity.hpp"
#include "ZRender.hpp"

class IRenderDestinationEntity;

class IReflectSource {
  public:
    virtual ~IReflectSource() = 0;
};

class ICameraManager : public IComponentInterface {
  public:
    virtual TEntityRef<IRenderDestinationEntity>* GetActiveRenderDestinationEntity(TEntityRef<IRenderDestinationEntity>& result) = 0;
    virtual void ICameraManager_Unk6() = 0;
    virtual void ICameraManager_Unk7() = 0;
    virtual void ICameraManager_Unk8() = 0;
    virtual TEntityRef<IRenderDestinationEntity>* GetActiveRenderDestinationEntity2(TEntityRef<IRenderDestinationEntity>& result) = 0;
};

class ZCameraManagerMain : public ICameraManager, public IReflectSource {};

class ICameraEntity : public IRenderDestinationSource {};

class ZCameraEntity : public ZRenderableEntity, public ICameraEntity {};

class IFreeCameraControl {
  public:
    virtual void ICameraManager_Unk0() = 0;
    virtual bool SetActive(bool bActive) = 0;
    virtual void ICameraManager_Unk2() = 0;
    virtual void SetCameraEntity(const TEntityRef<ZCameraEntity>& cameraEntity) = 0;
    virtual void ICameraManager_Unk4() = 0;
};

class ZFreeCameraControlEntity : public ZEntityImpl, public IFreeCameraControl {};

class ZFreeCameraControlEditorStyleEntity : public ZEntityImpl, public IFreeCameraControl {};
