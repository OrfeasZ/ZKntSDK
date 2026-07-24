#pragma once

#include "ZString.hpp"
#include "IComponentInterface.hpp"
#include "ZEntity.hpp"

class ISceneEntity;
class IEntityBlueprintFactory;
struct SSceneInitParameters;

enum class ESceneLoadingStage : int32_t {
    eLoading_Start = 0,
    eLoading_SceneStopped = 1,
    eLoading_SceneDeleted = 2,
    eLoading_AssetsLoaded = 3,
    eLoading_SceneAllocated = 4,
    eLoading_SceneStarted = 5,
    eLoading_ScenePrecaching = 6,
    eLoading_SceneActivated = 7,
    eLoading_SceneStreaming = 8,
    eLoading_ScenePlaying = 9,
    eLoading_MAX = 10
};

class IEntitySceneContext : public IComponentInterface {
  public:
    virtual void CreateTransformParentScene() = 0;
    virtual void ZEntitySceneContext_unk6() = 0;
    virtual void ClearScene(bool bFullyUnloadScene) = 0;
    virtual void ZEntitySceneContext_unk8() = 0;
    virtual void ZEntitySceneContext_unk9() = 0;
    virtual void ZEntitySceneContext_unk10() = 0;
    virtual void ZEntitySceneContext_unk11() = 0;
    virtual void ZEntitySceneContext_unk12() = 0;
    virtual void ZEntitySceneContext_unk13() = 0;
    virtual void ZEntitySceneContext_unk14() = 0;
    virtual void ZEntitySceneContext_unk15() = 0;
    virtual void LoadDynamicBrick(const ZRuntimeResourceID& runtimeResourceID, ZEntityRef entityRef, IEntityFactory* entityFactory) = 0;
    virtual void UnloadDynamicBrick(const ZRuntimeResourceID& runtimeResourceID) = 0;
    virtual void LoadScene(const SSceneInitParameters& parameters) = 0;
    virtual void ZEntitySceneContext_unk19() = 0;
    virtual void ZEntitySceneContext_unk20() = 0;
    virtual void ZEntitySceneContext_unk21() = 0;
    virtual void ZEntitySceneContext_unk22() = 0;
    virtual void ZEntitySceneContext_unk23() = 0;
    virtual void ZEntitySceneContext_unk24() = 0;
    virtual void ZEntitySceneContext_unk25() = 0;
    virtual void ZEntitySceneContext_unk26() = 0;
    virtual void ZEntitySceneContext_unk27() = 0;
    virtual void ZEntitySceneContext_unk28() = 0;
    virtual void ZEntitySceneContext_unk29() = 0;
    virtual void SetLoadingStage(ESceneLoadingStage stage) = 0;
    virtual ESceneLoadingStage GetLoadingStage() = 0;
    virtual float GetLoadingProgress() = 0;
};

struct SBrickAllocationInfo {
    PAD(0x8);                               // 0x0
    ZEntityType** m_EntityType;             // 0x8
    ZRuntimeResourceID m_RuntimeResourceID; // 0x10
    IEntityFactory* m_BrickFactory;         // 0x18
    PAD(0x30);                              // 0x20
};

class ZSceneConfiguration {
  public:
    PAD(0x4);                                                   // 0x0
    ZRuntimeResourceID m_ridSceneFactory;                       // 0x4
    PAD(0x20);                                                  // 0xC
    TResourcePtr<IEntityFactory> m_sceneFactoryResource;        // 0x2C
    TArray<TResourcePtr<IEntityFactory>> m_aMainBrickFactories; // 0x38
    TArray<ZRuntimeResourceID> m_aMainBrickFactoryRIDs;         // 0x50
    TArray<SBrickAllocationInfo> m_aMainBricks;                 // 0x68
};

class ZEntitySceneContext : public IEntitySceneContext {
  public:
    PAD(0x8);
    TEntityRef<ISceneEntity> m_pScene;                                                        // 0x10
    ZString m_SceneResource;                                                                  // 0x28
    PAD(0x40);                                                                                // 0x38
    ZString m_CheckpointName;                                                                 // 0x78
    ZSceneConfiguration* m_SceneConfig;                                                       // 0x88
    PAD(0x8);                                                                                 // 0x90
    THashMap<ZRuntimeResourceID, ZEntityType**> m_aDynamicBrickEntities;                      // 0x98
    THashMap<ZRuntimeResourceID, IEntityBlueprintFactory*> m_aDynamicBrickBlueprintFactories; // 0xB8
    PAD(0xA8);                                                                                // 0xD8
    ESceneLoadingStage m_LoadingStage;                                                        // 0x180
};

class ZSceneLifecycleListener {
  public:
    virtual ~ZSceneLifecycleListener() = 0;
};
