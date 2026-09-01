#pragma once

#include "ZScene.hpp"
#include "ISavable.hpp"
#include "ZBoundedEntity.hpp"
#include "ZValue.hpp"

class ZBuffer;
struct SG2PointCloud;

enum class EPFObstacleBehavior : int32_t {
    ePFOB_Legacy = 0,
    ePFOB_Penalty = 1,
    ePFOB_Impassible = 2,
    ePFOB_ImpassibleOrPenalty = 3,
};

class IPFObstacleManager {
  public:
    virtual ~IPFObstacleManager() = 0;
};

class IPFObstacleInternal : public ZSharedPointerTarget {};

class ZPFObstacleHandle {
  public:
    TSharedPointer<IPFObstacleInternal> m_internal;
};

class ZPFObstacleInternal : public IPFObstacleInternal {
  public:
    SMatrix m_transform;            // 0x10
    float4 m_halfSize;              // 0x50
    float32 m_fPenaltyMultiplier;   // 0x60
    float32 m_fPenaltyAddend;       // 0x64
    EPFObstacleBehavior m_Behavior; // 0x68
    uint32 m_ObstacleBlockerFlags;  // 0x6C
    uint32 m_affectedNavLayerFlags; // 0x70
};

class ZPFObstacleManager : public IPFObstacleManager {
  public:
    TArray<ZPFObstacleHandle> m_obstacles;
};

class ZPFSeedPoint : public ZSpatialEntity {
  public:
    ZResourcePtr m_pHelper; // 0xA8
};

class ZPathfinderConfigurationBase : public ZBoundedEntity {
  public:
    PAD(0x34);                                          // 0xC0
    SVector3 m_vGlobalSize;                             // 0xF4
    TInterfaceRef<IBoolValue> m_IsEnabled;              // 0x100
    PAD(0x18);                                          // 0x110
    TArray<TEntityRef<ZPFSeedPoint>> m_aSeedPoints;     // 0x128
    TResourcePtr<SG2PointCloud> m_PointCloudSeedPoints; // 0x140
    TArray<uint32> m_OverrideExportedLayers;            // 0x148
    bool m_PreventAddToNightlyBuild;                    // 0x160
    ZResourcePtr m_pHelper;                             // 0x164
};

class ZPathfinderConfiguration : public ZPathfinderConfigurationBase {
  public:
    bool m_IsDynamic;                           // 0x171
    TResourcePtr<ZBuffer> m_NavpowerResourceID; // 0x174
};

struct SLoadedNavMesh {
    ZPathfinderConfiguration* m_pPathfinderConfiguration; // 0x0
    void* m_pNavpowerResource;                            // 0x8
    uint64_t m_nNavpowerResourceSize;                     // 0x10
    bool m_bHasTransform;                                 // 0x18
    SMatrix m_Transform;                                  // 0x20
    PAD(0x10);                                            // 0x60
    char* m_pNavpowerResourceCopy;                        // 0x70
    PAD(0x8);                                             // 0x78
};

struct SPendingLoadedNavMesh {
    ZPathfinderConfiguration* m_pPathfinderConfiguration; // 0x0
    void* m_pNavpowerResource;                            // 0x8
    uint64_t m_nNavpowerResourceSize;                     // 0x10
    bool m_bHasTransform;                                 // 0x18
    SMatrix m_Transform;                                  // 0x20
    PAD(0x10);                                            // 0x60
};

class ZPathfinder : public IComponentInterface, public ZSceneLifecycleListener {
  public:
    PAD(0x88);                                                               // 0x10
    TArray<SLoadedNavMesh> m_aLoadedNavMeshes;                               // 0x98
    PAD(0x8);                                                                // 0xB0
    IPFObstacleManager* m_obstacleManager;                                   // 0xB8
    PAD(0x68);                                                               // 0xC0
    THashMap<uint64_t, IPFObstacleInternal*> m_ObstacleIDToObstacleInternal; // 0x128
};

class IObstacleFlagsEntity : public IComponentInterface {};

class ZObstacleFlagEntity : public ZEntityImpl, public IObstacleFlagsEntity {
  public:
    enum class ENavmeshObstacleFlags : int32_t {
        PhysicalBlocker = 0,
        Flanking = 2,
        EnvironmentalHazardAreas = 3,
        CloseCombatTarget = 4,
        AgentsInLethalCombat = 5,
        StorylineAgents = 6,
        ObstacleBoundingArea = 7,
        Investigating = 8,
        CloseCombatEngagementArea = 9,
        CloseCombatThreatAreas = 10,
        EscortingOut = 11,
        CombatHotspot = 12,
        SocialAvoidance = 13
    };

  public:
    TArray<ENavmeshObstacleFlags> m_navigationFlags; // 0x20
};

class INavLayerFlagsEntity : public IComponentInterface {};

class ZNavLayerFlagsEntity : public ZEntityImpl, public INavLayerFlagsEntity {
  public:
    enum class ENavmeshLayerFlags : int32_t {
        Default = 1,
        Storylines = 2,
        Wreckie = 4,
    };

  public:
    uint32 m_layerFlags; // 0x20
};

class IObstacleUserDataProvider;

class ZPFObstacleEntity : public ZBoundedEntity, public ISavableEntity {
  public:
    SVector3 m_vGlobalSize;                                   // 0xC8
    EPFObstacleBehavior m_Behavior;                           // 0xD4
    float32 m_fPenaltyMultiplier;                             // 0xD8
    float32 m_fPenaltyAddend;                                 // 0xDC
    TEntityRef<IObstacleFlagsEntity> m_ObstacleBlockerFlags;  // 0xE0
    TEntityRef<INavLayerFlagsEntity> m_affectedNavLayerFlags; // 0xF8
    TInterfaceRef<IObstacleUserDataProvider> m_UserData;      // 0x110
    bool m_bEnabled;                                          // 0x120
    ZResourcePtr m_pHelper;                                   // 0x124
    uint64_t m_ObstacleID;                                    // 0x130
};
