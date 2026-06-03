#pragma once

#include "Common.hpp"
#include "EngineFunction.hpp"
#include "Glacier/ZEntity.hpp"
#include "Glacier/ZDelegate.hpp"
#include "Glacier/EUpdateMode.hpp"

class ZEntityManager;
class IEntityFactory;
class ZCameraManagerMain;
class ZCameraEntity;
class ZSpatialEntity;
class ZUpdateEventContainer;
struct SGameUpdateEvent;
class ZInputAction;

namespace zknt {
    class Functions {
      public:
        Functions();
        ~Functions();

        Functions(const Functions&) = delete;
        Functions& operator=(const Functions&) = delete;

        EngineFunction<ZEntityRef*(ZEntityManager* th, ZEntityRef& result, SEntityCreateInfo& entityCreateInfo)>* ZEntityManager_NewEntity = nullptr;

        EngineFunction<void(ZEntityManager* th, const ZEntityRef& ent)>* ZEntityManager_DeleteEntity = nullptr;

        EngineFunction<SEntityCreateInfo*(
            SEntityCreateInfo* th, const ZString& sDebugName, ZResourcePtr& pEntityFactory, const ZEntityRef& transformParent, uint64_t entityId
        )>* SEntityCreateInfo_SEntityCreateInfo = nullptr;

        EngineFunction<TEntityRef<ZCameraEntity>*(ZCameraManagerMain* th, TEntityRef<ZCameraEntity>& result)>*
            ZCameraManagerMain_GetActiveMainCamera = nullptr;

        EngineFunction<void(const ZSpatialEntity* th)>* ZSpatialEntity_UpdateCachedWorldMat;

        EngineFunction<ZCameraEntity*()>* GetCurrentCamera;

        EngineFunction<void(
            ZUpdateEventContainer* th, const ZDelegate<void(const SGameUpdateEvent&)>& callback, int32_t nPriority, EUpdateMode eUpdateMode
        )>* ZUpdateEventContainer_AddDelegate;

        EngineFunction<void(
            ZUpdateEventContainer* th, const ZDelegate<void(const SGameUpdateEvent&)>& callback, int32_t nPriority, EUpdateMode eUpdateMode
        )>* ZUpdateEventContainer_RemoveDelegate;

        EngineFunction<float(ZInputAction* th)>* ZInputAction_Analog;

        EngineFunction<bool(ZInputAction* th)>* ZInputAction_Digital;
    };
}
