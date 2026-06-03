#pragma once

#include "Common.hpp"
#include "EngineFunction.hpp"
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
struct SEntityCreateInfo;
class ZEntityRef;
class ZResourcePtr;
template<typename T> class TEntityRef;
class ZObjectRef;
class ZEntityType;
class ZInputContext;

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

        EngineFunction<void(const ZSpatialEntity* th)>* ZSpatialEntity_UpdateCachedWorldMat = nullptr;

        EngineFunction<ZCameraEntity*()>* GetCurrentCamera = nullptr;

        EngineFunction<void(
            ZUpdateEventContainer* th, const ZDelegate<void(const SGameUpdateEvent&)>& callback, int32_t nPriority, EUpdateMode eUpdateMode
        )>* ZUpdateEventContainer_AddDelegate = nullptr;

        EngineFunction<void(
            ZUpdateEventContainer* th, const ZDelegate<void(const SGameUpdateEvent&)>& callback, int32_t nPriority, EUpdateMode eUpdateMode
        )>* ZUpdateEventContainer_RemoveDelegate = nullptr;

        EngineFunction<float(ZInputAction* th)>* ZInputAction_Analog = nullptr;

        EngineFunction<bool(ZInputAction* th)>* ZInputAction_Digital = nullptr;

        EngineFunction<bool(ZEntityType* entity, uint32_t propertyId, const ZObjectRef& value, bool invokeChangeHandlers)>* SetPropertyValue =
            nullptr;

        EngineFunction<ZInputContext*()>* GetGlobalInputContext = nullptr;

        EngineFunction<void(const char* binds, ZInputContext* inputContext)>* AddBindings = nullptr;
    };
}
