#pragma once

#include "ZGadget.hpp"

class ZKntPlayerLoadoutEntity;
class ZStreamingSetEntity;
class ZKntSpawnpointEntity;

enum class EKntCheckpointType : int32_t {
    None = 0,
    Session = 1,
    Permanent = 2,
};

enum class EKntCheckpointStateType : int32_t {
    Inactive = 0,
    Active = 1,
    WaitingPlayerSpawn = 2,
    Ready = 3,
};

class ZCheckpointEntity : public ZSpatialEntity {
  public:
    TEntityRef<ZStreamingSetEntity> m_streamingSet;        // 0xA8
    TEntityRef<ZStreamingSetEntity> m_preloadStreamingSet; // 0xC0
    bool m_activateOnPreload;                              // 0xD8
    ZResourcePtr m_pGizmo;                                 // 0xDC
};

class ZKntCheckpointEntity : public ZCheckpointEntity, public IBoolValue, public IValueChanged {
  public:
    ZString m_name;                                             // 0xF8
    TArray<TEntityRef<ZKntCheckpointEntity>> m_nextCheckpoints; // 0x108
    TInterfaceRef<IBoolValue> m_activationCondition;            // 0x120
    TInterfaceRef<IBoolValue> m_condition;                      // 0x130
    TEntityRef<ZKntPlayerLoadoutEntity> m_playerLoadout;        // 0x140
    TEntityRef<ZKntSpawnpointEntity> m_defaultSpawnpoint;       // 0x158
    EKntCheckpointType m_type;                                  // 0x170
    EKntCheckpointStateType m_state;                            // 0x174
    ZString m_rawTitle;                                         // 0x178
    TResourcePtr<ZTextLine> m_pTitle;                           // 0x188
    ZString m_rawDescription;                                   // 0x190
    TResourcePtr<ZTextLine> m_pDescription;                     // 0x1A0
    ZRuntimeResourceID m_pImage;                                // 0x1A8
    TArray<ZString> m_neededRootResources;                      // 0x1B0
    bool m_isDevCheckpoint;                                     // 0x1C8
    bool m_waitForUnloadsBeforeLoading;                         // 0x1C9
    bool m_allowAdvanceCheckpointBeforeReady;                   // 0x1CA
    bool m_resetPlayerSkillsState;                              // 0x1CB
    bool m_resetPlayerInvisibilityToNPCs;                       // 0x1CC
};

class ZKntCampaignMissionEntity : public ZEntityImpl, public IGadgetCollectionProvider {
  public:
    // bool IsActive;                                             // 0x0
    // bool IsCompleted;                                          // 0x0
    ZString m_rawTitle;                                        // 0x20
    TResourcePtr<ZTextLine> m_pTitle;                          // 0x30
    ZString m_rawDescription;                                  // 0x38
    TResourcePtr<ZTextLine> m_pDescription;                    // 0x48
    ZString m_rawShortDescription;                             // 0x50
    TResourcePtr<ZTextLine> m_pShortDescription;               // 0x60
    ZString m_rawInitialBriefingDescription;                   // 0x68
    TResourcePtr<ZTextLine> m_pInitialBriefingDescription;     // 0x78
    ZRuntimeResourceID m_pImage;                               // 0x80
    ZRuntimeResourceID m_pHighResImage;                        // 0x88
    ZRuntimeResourceID m_pPortraitImage;                       // 0x90
    ZRuntimeResourceID m_pLandscapeImage;                      // 0x98
    ZRuntimeResourceID m_pHeroImage;                           // 0xA0
    ZRuntimeResourceID m_pVideo;                               // 0xA8
    bool m_isInterludeMission;                                 // 0xB0
    bool m_includedInBoot;                                     // 0xB1
    TArray<TInterfaceRef<ZKntCheckpointEntity>> m_checkpoints; // 0xB8
    TEntityRef<ZKntPlayerLoadoutEntity> m_playerLoadout;       // 0xD0
    TArray<TResourcePtr<IEntityFactory>> m_requiredGadgets;    // 0xE8
    TArray<TResourcePtr<IEntityFactory>> m_recommendedGadgets; // 0x100
    TArray<TResourcePtr<IEntityFactory>> m_optionalGadgets;    // 0x118
    ZString m_codeName;                                        // 0x130
    TArray<ZString> m_neededRootResources;                     // 0x140
    ZGuid m_missionId;                                         // 0x158
    PAD(0xE0);                                                 // 0x168
    uint32 m_availableGadgetSlots;                             // 0x248
    // PAD(0x1C);                                                 // 0x24C
    PAD(0x18);   // 0x24C
    bool m_bUnk; // 0x264
};

class ZKntCampaignEntity : public ZEntityImpl {
  public:
    ZString m_rawTitle;                                          // 0x18
    TResourcePtr<ZTextLine> m_pTitle;                            // 0x28
    ZRuntimeResourceID m_pImage;                                 // 0x30
    TArray<TInterfaceRef<ZKntCampaignMissionEntity>> m_missions; // 0x38
    uint32 m_id;                                                 // 0x50
    PAD(0x3C);                                                   // 0x54
};

enum class EKntMissionType : int32_t {
    Operation = 0,
    Escalation = 1,
    Campaign = 2,
    Hub = 3,
};

struct SKntMissionData {
    ZGuid m_Id;             // 0x00
    PAD(0x28);              // 0x10
    EKntMissionType m_Type; // 0x38
    PAD(0x154);             // 0x3C
};
