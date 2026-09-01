#pragma once

#include "Common.hpp"
#include <cstdint>

class ZGameSceneflowModule;
class ZEntityManager;
class ZResourceContainer;
class ZMemoryManager;
class ZCameraManagerMain;
class ZApplicationEngineWin32;
class ZResourceManager;
class ZTypeRegistry;
class ZGameLoopManager;
class ZLocalPlayerData;
class ZGameTimeManager;
class ZCollisionManager;
class ZPackageManagerBase;
class ZComponentManager;
class ZRenderManager;
struct SD3D12ObjectPools;
struct SPrimDrawData;
struct SEntitySlotMetadata;
class ZObjectPool;
class ZSelectionForFreeCameraEditorStyleEntity;
struct SDefaultAllocatorProvider;
template<typename T> class TArray;
template<typename T> class TEntityRef;
class ZGameKeywordManager;
class ZPathfinder;
class ZInputManager;
class ZRoomManagerMain;
class ZKntGameProgressionManager;
class ZUIText;
struct SKntMissionData;
class ZRenderGraphManager;

namespace Gameplay {
    class ZKntGPWStores;
    class ZGPWTransientStores;
}

namespace zknt {
    class Globals {
      public:
        Globals();
        ~Globals() = default;

        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;

        ZGameSceneflowModule* GameSceneflowModule = nullptr;
        ZEntityManager* EntityManager = nullptr;
        ZResourceContainer** ResourceContainer = nullptr;
        ZMemoryManager** MemoryManager = nullptr;
        ZCameraManagerMain* CameraManagerMain = nullptr;
        ZApplicationEngineWin32** ApplicationEngineWin32 = nullptr;
        ZResourceManager* ResourceManager = nullptr;
        ZTypeRegistry** TypeRegistry = nullptr;
        ZGameLoopManager* GameLoopManager = nullptr;
        ZLocalPlayerData* LocalPlayerData = nullptr;
        ZGameTimeManager* GameTimeManager = nullptr;
        ZCollisionManager* CollisionManager = nullptr;
        ZPackageManagerBase** PackageManager = nullptr;
        void* ZTemplateEntityFactory_vtbl = nullptr;
        void* ZAspectEntityFactory_vtbl = nullptr;
        void* ZCppEntityFactory_vtbl = nullptr;
        void* ZExtendedCppEntityFactory_vtbl = nullptr;
        void* ZUIControlEntityFactory_vtbl = nullptr;
        void* ZRenderMaterialEntityFactory_vtbl = nullptr;
        void* ZAudioSwitchEntityFactory_vtbl = nullptr;
        void* ZAudioStateEntityFactory_vtbl = nullptr;
        void* ZPadEntityFactory_vtbl = nullptr;
        void* ZShadernodeEntityFactory_vtbl = nullptr;
        void* ZTemplateEntityBlueprintFactory_vtbl = nullptr;
        void* ZAspectEntityBlueprintFactory_vtbl = nullptr;
        void* ZCppEntityBlueprintFactory_vtbl = nullptr;
        void* ZExtendedCppEntityBlueprintFactory_vtbl = nullptr;
        void* ZUIControlBlueprintFactory_vtbl = nullptr;
        void* ZRenderMaterialEntityBlueprintFactory_vtbl = nullptr;
        void* ZAudioSwitchBlueprintFactory_vtbl = nullptr;
        void* ZAudioStateBlueprintFactory_vtbl = nullptr;
        void* ZPadEntityBlueprintFactory_vtbl = nullptr;
        void* ZShadernodeEntityBlueprintFactory_vtbl = nullptr;
        ZComponentManager** ComponentManager = nullptr;
        ZRenderManager* RenderManager = nullptr;
        SD3D12ObjectPools** D3D12ObjectPools = nullptr;
        SPrimDrawData** PrimDrawData = nullptr;
        SEntitySlotMetadata** EntitySlotMetadata = nullptr;
        ZObjectPool* Variant8BytePool;
        ZObjectPool* Variant32BytePool;
        Gameplay::ZKntGPWStores* KntGPWStores = nullptr;
        Gameplay::ZGPWTransientStores* GPWTransientStores = nullptr;
        TArray<TEntityRef<ZSelectionForFreeCameraEditorStyleEntity>>* Selections = nullptr;
        ZGameKeywordManager* GameKeywordManager = nullptr;
        ZPathfinder* Pathfinder = nullptr;
        ZInputManager* InputManager = nullptr;
        ZRoomManagerMain* RoomManagerMain = nullptr;
        ZKntGameProgressionManager* KntGameProgressionManager = nullptr;
        ZUIText* UIText = nullptr;
        TArray<SKntMissionData>* Missions = nullptr;
        ZRenderGraphManager* RenderGraphManager;
    };
}
