#pragma once

#include "Common.hpp"
#include "Hook.hpp"

class ZString;
class ZApplicationEngineWin32;
class ZEngineAppCommon;
class ZFreeCameraControlEntity;
class ZObjectRef;
class ZEntityRef;
class ZEntityType;
class ZFreeCameraControlEditorStyleEntity;
class ZKntLoadoutCollectionEntity;
class ZComponentCreateInfo;
struct SRenderCameraData;
struct SPassExecution;
class ZEntitySceneContext;
class ZTemplateEntityFactory;
template<typename T> class TResourcePtr;
class ZEntityManager;
class IEntityFactory;
struct SEntityIdentifier;
class ZPFObstacleEntity;
class ZPathfinder;
struct SPendingLoadedNavMesh;
class ZPathfinderConfiguration;
class ZLevelManager;
class ZRuntimeResourceID;
class ZKntGameProgressionManager;
class ZKntCheckpointEntity;
template<typename T> class TEntityRef;

namespace zknt {
    class ModSDK;

    class Hooks {
      public:
        Hooks();
        ~Hooks() = default;

        Hooks(const Hooks&) = delete;
        Hooks& operator=(const Hooks&) = delete;

        Hook<bool(void*, void*)>* Engine_Init = nullptr;

        Hook<bool(const ZString& optionName, bool defaultValue)>* GetApplicationOptionBool = nullptr;

        Hook<LRESULT(ZApplicationEngineWin32*, HWND, UINT, WPARAM, LPARAM)>* ZApplicationEngineWin32_MainWindowProc = nullptr;

        Hook<ZString*(ZEngineAppCommon* th, ZString& result)>* ZEngineAppCommon_GetBootScene = nullptr;

        Hook<ZString*(ZFreeCameraControlEntity* th, ZString& result, int32_t nControllerId)>* ZFreeCameraControlEntity_GenerateActionBindingString =
            nullptr;

        Hook<void(ZFreeCameraControlEntity* th)>* ZFreeCameraControlEntity_UpdateMovementFromInput = nullptr;

        Hook<void(ZFreeCameraControlEntity* th, float dt)>* ZFreeCameraControlEntity_UpdateCamera = nullptr;

        Hook<void(ZEntityType**, uint32_t, const ZObjectRef&)>* SignalOutputPin = nullptr;

        Hook<void(ZEntityType**, uint32_t, const ZObjectRef&)>* SignalInputPin = nullptr;

        Hook<ZString*(ZFreeCameraControlEditorStyleEntity* th, ZString& result, int32_t nControllerId)>*
            ZFreeCameraControlEditorStyleEntity_GenerateActionBindingString = nullptr;

        Hook<void(ZFreeCameraControlEditorStyleEntity* const th, bool bRotationIsActive, bool bObjectHookIsActive, bool bIsOrbitActive)>*
            ZFreeCameraControlEditorStyleEntity_HandleDrag = nullptr;

        Hook<void(ZFreeCameraControlEditorStyleEntity* th, float fDeltaTime)>* ZFreeCameraControlEditorStyleEntity_MoveCameraWithKey = nullptr;

        Hook<ZKntLoadoutCollectionEntity*(ZKntLoadoutCollectionEntity* th, const ZComponentCreateInfo& Info)>*
            ZKntLoadoutCollectionEntity_ZKntLoadoutCollectionEntity = nullptr;

        Hook<void(const SRenderCameraData& cameraData, uint32_t unk)>* UpdateStreamlineConstants = nullptr;

        Hook<void(SPassExecution* th, int32_t renderDeviceContextIndex)>* SPassExecution_ExecutePass = nullptr;

        Hook<void(ZEntitySceneContext*)>* ZEntitySceneContext_CreateScene = nullptr;

        Hook<void(ZEntitySceneContext*, bool bFullyUnloadScene)>* ZEntitySceneContext_ClearScene = nullptr;

        Hook<void(ZTemplateEntityFactory* th, ZEntityType** pEntity, void* unk0, void* unk1, void* unk2)>* ZTemplateEntityFactory_ConfigureEntity =
            nullptr;

        Hook<ZEntityRef*(
            ZEntityManager* th, ZEntityRef& result, const ZString& sDebugName, TResourcePtr<IEntityFactory>& entityFactory,
            const ZEntityRef& logicalParent, uint64_t entityID, void* unk0, bool unk1
        )>* ZEntityManager_NewUninitializedEntity = nullptr;

        Hook<uint64_t(SEntityIdentifier* p_Identifier)>* GenerateEntityIdentifier = nullptr;

        Hook<void(ZEntityManager* th, const ZEntityRef& entityRef)>* ZEntityManager_DeleteEntity = nullptr;

        Hook<void(ZPFObstacleEntity* th, bool forceUpdate)>* ZPFObstacleEntity_UpdateObstacle = nullptr;

        Hook<void(ZPathfinder* th, const SPendingLoadedNavMesh& pendingLoadedNavMesh)>* ZPathfinder_AddLoadedNavMesh = nullptr;

        Hook<bool(ZPathfinder* th, ZPathfinderConfiguration* pathfinderConfiguration)>* ZPathfinder_RemoveLoadedNavMesh = nullptr;

        Hook<void(ZLevelManager* th)>* ZLevelManager_SetStatePlaying = nullptr;

        Hook<void(ZEntitySceneContext* th, const ZRuntimeResourceID& runtimeResourceID, ZEntityRef entityRef, IEntityFactory* entityFactory)>*
            ZEntitySceneContext_LoadDynamicBrick = nullptr;

        Hook<void(ZEntitySceneContext* th, const ZRuntimeResourceID& runtimeResourceID)>* ZEntitySceneContext_UnloadDynamicBrick = nullptr;

        Hook<void(ZKntGameProgressionManager* th, const TEntityRef<ZKntCheckpointEntity>& checkpointEntity)>*
            ZKntGameProgressionManager_SetCurrentCheckpoint;

      private:
        void EnableAll();

        friend class ModSDK;
    };
}
