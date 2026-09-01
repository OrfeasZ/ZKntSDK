#pragma once

#include <IPluginInterface.hpp>

#include <Glacier/ZKntGameProgressionManager.hpp>

namespace discord {
    class Core;
}

class ZKntCampaignMissionEntity;
enum class EKntMissionType;

class DiscordRichPresence : public zknt::IPluginInterface {
  public:
    ~DiscordRichPresence() override;

    void Init() override;
    void OnEngineInitialized() override;

  private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    void UpdateActivity();

    static ZKntCampaignMissionEntity* GetCampaignMissionEntity(ZKntCheckpointEntity* p_CheckpointEntity);
    static std::optional<EKntMissionType> GetMissionType(ZKntCampaignMissionEntity* p_MissionEntity);

    static std::string NormalizeMissionCodeName(const ZString& p_CodeName);
    static std::string GetMissionTitleKey(EKntMissionType p_MissionType, const ZString& p_MissionCodeName);
    static bool TryGetUIText(const std::string& p_Key, ZString& p_OutText);
    static void ExtractCampaignTitleAndLocation(const ZString& p_CombinedText, ZString& p_OutTitle, ZString& p_OutLocation);
    static std::string ToTitleCase(std::string p_Text);

    static std::string_view MissionTypeToString(EKntMissionType p_MissionType);
    static std::string GetActivityImageKey(EKntMissionType p_MissionType, const ZString& p_MissionTitle);

    DECLARE_PLUGIN_DETOUR(
        DiscordRichPresence, void, ZKntGameProgressionManager_SetCurrentCheckpoint, ZKntGameProgressionManager* th,
        const TEntityRef<ZKntCheckpointEntity>& checkpointEntity
    );

    std::unique_ptr<discord::Core> m_DiscordCore;
};

DECLARE_ZKNT_PLUGIN(DiscordRichPresence)
