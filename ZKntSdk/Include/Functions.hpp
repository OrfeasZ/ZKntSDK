#pragma once

#include "Common.hpp"
#include "EngineFunction.hpp"
#include "Glacier/ZDelegate.hpp"
#include "Glacier/EUpdateMode.hpp"
#include "Glacier/ZMath.hpp"

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
class ICameraEntity;
class ZFreeCameraControlEditorStyleEntity;
class ZEntityImpl;
class ZResourceManager;
class ZResourceIndex;
class ZConfigCommand;

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

        EngineFunction<bool(ZEntityType** entity, uint32_t propertyId, const ZObjectRef& value, bool invokeChangeHandlers)>* SetPropertyValue =
            nullptr;

        EngineFunction<ZInputContext*()>* GetGlobalInputContext = nullptr;

        EngineFunction<void(const char* binds, ZInputContext* inputContext)>* AddBindings = nullptr;

        EngineFunction<ZString*(ZString& result, const ZString& sText)>* ZTextListData_DecryptText = nullptr;

        EngineFunction<void*(const char* pszName)>* GetGlobalPointer = nullptr;

        EngineFunction<ZString::ZImpl*(const char* buf, size_t size)>* ZStringCollection_Allocate = nullptr;

        EngineFunction<void(ZString::ZImpl* th)>* ZString_ZImpl_Free = nullptr;

        EngineFunction<void(
            TEntityRef<ICameraEntity> pCamera, TEntityRef<ZSpatialEntity> pCameraSpatial, const float4& vMouseDelta, const float4& vHookPoint
        )>* ZCameraUtil_PanCamera = nullptr;

        EngineFunction<void(ZFreeCameraControlEditorStyleEntity* th, float4 orbitDelta, float4 orbitCenter)>*
            ZFreeCameraControlEditorStyleEntity_OrbitCamera = nullptr;

        EngineFunction<SMatrix*(SMatrix& result, SMatrix cameraToWorld, float yawDelta, float pitchDelta, const float4& orbitCenter)>*
            ZCameraUtil_RotateCameraToWorldMatrix = nullptr;

        EngineFunction<void(ZFreeCameraControlEditorStyleEntity* th, const SMatrix& mCameraToWorld)>*
            ZFreeCameraControlEditorStyleEntity_ApplyCameraMatrix = nullptr;

        EngineFunction<ZEntityType*(ZEntityImpl* th, uint32_t nUniqueMapMask)>* ZEntityImpl_EnsureUniqueType = nullptr;

        EngineFunction<void(ZResourceManager* th, ZResourceIndex index)>* ZResourceManager_UninstallResource = nullptr;

        EngineFunction<void(const char* pCommandName, const char* argv)>* ZConfigCommand_ExecuteCommand = nullptr;

        EngineFunction<ZConfigCommand*(uint32_t commandNameHash)>* ZConfigCommand_GetConfigCommand = nullptr;
    };
}
