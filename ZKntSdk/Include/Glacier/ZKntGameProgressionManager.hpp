#pragma once

#include "ZKntCampaign.hpp"

class ZKntGameProgressionManager : public IComponentInterface {
  public:
    TEntityRef<ZKntCampaignEntity> m_rCurrentCampaignEntity;               // 0x8
    TEntityRef<ZKntCampaignMissionEntity> m_rCurrentCampaignMissionEntity; // 0x30
    TArray<TEntityRef<ZKntSpawnpointEntity>> m_aSpawnpointEntities;        // 0x38
    TEntityRef<ZKntCheckpointEntity> m_rCurrentCheckpointEntity;           // 0x50
};
