#include "DiscordRichPresence.hpp"

#include <algorithm>
#include <ranges>
#include <cctype>

#include <discord.h>

#include <Glacier/ZGameLoopManager.hpp>
#include <Glacier/ZKntCampaign.hpp>
#include <Glacier/ZKntGameProgressionManager.hpp>

#include <Logging.hpp>

namespace {
    constexpr discord::ClientId k_ApplicationId = 1535686171760263258;
}

DiscordRichPresence::~DiscordRichPresence() {
    const ZMemberDelegate<DiscordRichPresence, void(const SGameUpdateEvent&)> s_Delegate(this, &DiscordRichPresence::OnFrameUpdate);
    SDK()->Globals()->GameLoopManager->UnregisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdateAlways);
}

void DiscordRichPresence::Init() {
    discord::Core* s_DiscordCore = nullptr;

    const auto s_Result = discord::Core::Create(k_ApplicationId, DiscordCreateFlags_Default, &s_DiscordCore);

    if (s_Result == discord::Result::Ok) {
        m_DiscordCore.reset(s_DiscordCore);
    }
    else {
        delete s_DiscordCore;

        Logger::Error("[DiscordRichPresence] Discord init failed with result: {}", static_cast<int>(s_Result));

        return;
    }

    SDK()->Hooks()->ZKntGameProgressionManager_SetCurrentCheckpoint->AddDetour(
        this, &DiscordRichPresence::ZKntGameProgressionManager_SetCurrentCheckpoint
    );
}

void DiscordRichPresence::OnEngineInitialized() {
    const ZMemberDelegate<DiscordRichPresence, void(const SGameUpdateEvent&)> s_Delegate(this, &DiscordRichPresence::OnFrameUpdate);
    SDK()->Globals()->GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdateAlways);

    UpdateActivity();
}

void DiscordRichPresence::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) {
    if (m_DiscordCore) {
        m_DiscordCore->RunCallbacks();
    }
}

void DiscordRichPresence::UpdateActivity() {
    auto s_KntGameProgressionManager = SDK()->Globals()->KntGameProgressionManager;

    if (!s_KntGameProgressionManager->m_rCurrentCheckpointEntity) {
        return;
    }

    ZString s_CheckpointName;

    if (s_KntGameProgressionManager->m_rCurrentCheckpointEntity) {
        s_CheckpointName = s_KntGameProgressionManager->m_rCurrentCheckpointEntity.m_pInterfaceRef->m_name;
    }

    ZString s_Location;
    ZString s_MissionTitle;
    ZString s_CheckpointTitle;
    ZString s_MissionTypeName;
    EKntMissionType s_MissionType;

    if (s_CheckpointName == "Boot") {
        s_Location = "In startup screen";
    }
    else if (s_CheckpointName == "MainMenu") {
        s_Location = "In main menu";
    }
    else {
        // Use GetCampaignMissionEntity instead of ZKntGameProgressionManager::m_rCurrentCampaignMissionEntity since it's set to Online hub mission
        // when loading TacSim mission.
        ZKntCampaignMissionEntity* s_Mission = GetCampaignMissionEntity(s_KntGameProgressionManager->m_rCurrentCheckpointEntity.m_pInterfaceRef);

        if (!s_Mission) {
            Logger::Error("[DiscordRichPresence] Could not find the campaign mission for checkpoint '{}'.", s_CheckpointName);
            return;
        }

        const auto s_OptionalMissionType = GetMissionType(s_Mission);

        if (!s_OptionalMissionType) {
            Logger::Error("[DiscordRichPresence] Mission type not found for mission ID {}.", s_Mission->m_missionId.ToString().c_str());
            return;
        }

        s_MissionType = *s_OptionalMissionType;

        s_MissionTypeName = MissionTypeToString(s_MissionType);

        if (s_MissionType == EKntMissionType::Hub) {
            s_MissionTitle = "Online hub";
            s_Location = "MI6";
        }
        else {
            const std::string s_TitleKey = GetMissionTitleKey(s_MissionType, s_Mission->m_codeName);

            if (s_MissionType == EKntMissionType::Campaign) {
                std::string s_TitleKey = GetMissionTitleKey(s_MissionType, s_Mission->m_codeName);

                ZString s_CombinedText;
                bool s_TitleFound = TryGetUIText(s_TitleKey, s_CombinedText);

                if (!s_TitleFound && s_MissionType == EKntMissionType::Campaign) {
                    constexpr std::string_view s_WithLocationSuffix = "_WITHLOCATION";

                    if (s_TitleKey.ends_with(s_WithLocationSuffix)) {
                        s_TitleKey.erase(s_TitleKey.size() - s_WithLocationSuffix.size());

                        s_TitleFound = TryGetUIText(s_TitleKey, s_CombinedText);
                    }
                }

                if (s_TitleFound) {
                    ExtractCampaignTitleAndLocation(s_CombinedText, s_MissionTitle, s_Location);

                    if (s_Location.IsEmpty()) {
                        // Fall back to the raw title, which contains the campaign location.
                        s_Location = s_Mission->m_rawTitle;
                    }
                }
                else {
                    s_MissionTitle = "ERR_UNKNOWN_MISSION";

                    Logger::Error("[DiscordRichPresence] Missing UI text for mission title key: {}.", s_TitleKey);
                }
            }
            else {
                if (!TryGetUIText(s_TitleKey, s_MissionTitle)) {
                    s_MissionTitle = "ERR_UNKNOWN_MISSION";

                    Logger::Error("[DiscordRichPresence] Missing UI text for mission title key: {}.", s_TitleKey);
                }

                const std::string s_CodeName = NormalizeMissionCodeName(s_Mission->m_codeName);

                const std::string s_LocationKey = s_CodeName + "_LOCATION";

                if (!TryGetUIText(s_LocationKey, s_Location)) {
                    const std::string s_AlternativeLocationKey = s_CodeName + "_LOCATION_NAME";

                    if (!TryGetUIText(s_AlternativeLocationKey, s_Location)) {
                        s_Location = "ERR_UNKNOWN_LOCATION";

                        Logger::Error(
                            "[DiscordRichPresence] Missing UI text for location keys '{}' and '{}'.", s_LocationKey, s_AlternativeLocationKey
                        );
                    }
                }
            }

            auto s_CheckPoint = s_KntGameProgressionManager->m_rCurrentCheckpointEntity.m_pInterfaceRef;

            if (s_CheckPoint->m_pTitle.Exists()) {
                s_CheckpointTitle = s_CheckPoint->m_pTitle.GetResource()->GetText();
            }
            else {
                s_CheckpointTitle = "ERR_UNKNOWN_CHECKPOINT";

                Logger::Error("[DiscordRichPresence] Missing UI text for checkpoint '{}'.", s_CheckpointName);
            }
        }
    }

    std::string s_Action;
    std::string s_Details;
    std::string s_ImageKey;

    if (s_Location == "In startup screen" || s_Location == "In main menu") {
        s_Action = s_Location;
        s_ImageKey = "logo";
    }
    else {
        if (s_MissionType == EKntMissionType::Hub) {
            s_Action = s_MissionTitle.c_str();
        }
        else {
            s_Action = std::format("{} - {}", s_MissionTitle.c_str(), s_CheckpointTitle.c_str());
        }

        s_Details = std::format("Playing {} in {}", s_MissionTypeName.c_str(), s_Location.c_str());
        s_ImageKey = GetActivityImageKey(s_MissionType, s_MissionTitle);
    }

    discord::Activity activity{};
    activity.SetType(discord::ActivityType::Playing);
    activity.SetState(s_Action.c_str());
    activity.SetDetails(s_Details.c_str());
    activity.GetAssets().SetLargeImage(s_ImageKey.c_str());

    m_DiscordCore->ActivityManager().UpdateActivity(activity, [](discord::Result p_Result) {
        Logger::Trace("[DiscordRichPresence] Activity manager push completed with result: {}.", static_cast<int>(p_Result));
    });
}

ZKntCampaignMissionEntity* DiscordRichPresence::GetCampaignMissionEntity(ZKntCheckpointEntity* p_CheckpointEntity) {
    auto s_KntGameProgressionManager = SDK()->Globals()->KntGameProgressionManager;

    for (const auto& s_Mission : s_KntGameProgressionManager->m_rCurrentCampaignEntity.m_pInterfaceRef->m_missions) {
        for (const auto& s_Checkpoint : s_Mission.m_pInterfaceRef->m_checkpoints) {
            if (s_Checkpoint.m_pInterfaceRef->m_name == p_CheckpointEntity->m_name) {
                return s_Mission.m_pInterfaceRef;
            }
        }
    }

    return nullptr;
}

std::optional<EKntMissionType> DiscordRichPresence::GetMissionType(ZKntCampaignMissionEntity* p_MissionEntity) {
    for (const auto& s_MissionData : *SDK()->Globals()->Missions) {
        if (s_MissionData.m_Id == p_MissionEntity->m_missionId) {
            return s_MissionData.m_Type;
        }
    }

    return std::nullopt;
}

std::string DiscordRichPresence::NormalizeMissionCodeName(const ZString& p_CodeName) {
    std::string s_CodeName = p_CodeName.c_str();

    std::ranges::transform(s_CodeName, s_CodeName.begin(), [](unsigned char p_Character) { return static_cast<char>(std::toupper(p_Character)); });

    if (const auto s_TierPosition = s_CodeName.find("_TIER"); s_TierPosition != std::string::npos) {
        s_CodeName.erase(s_TierPosition);
    }

    return s_CodeName;
}

std::string DiscordRichPresence::GetMissionTitleKey(EKntMissionType p_MissionType, const ZString& p_MissionCodeName) {
    std::string s_CodeName = p_MissionCodeName.c_str();

    std::ranges::transform(s_CodeName, s_CodeName.begin(), [](unsigned char p_Character) { return static_cast<char>(std::toupper(p_Character)); });

    if (p_MissionType == EKntMissionType::Campaign) {
        // Remove the mission-number prefix, such as "M02.1 ".
        if (const std::size_t s_PrefixEnd = s_CodeName.find(' '); s_PrefixEnd != std::string::npos) {
            s_CodeName.erase(0, s_PrefixEnd + 1);
        }

        std::erase_if(s_CodeName, [](unsigned char p_Character) { return std::isspace(p_Character) != 0; });

        return "UI_MISSIONNAMES_" + s_CodeName + "_WITHLOCATION";
    }

    if (const std::size_t s_TierPosition = s_CodeName.find("_TIER"); s_TierPosition != std::string::npos) {
        s_CodeName.erase(s_TierPosition);
    }

    return s_CodeName + "_NAME";
}

bool DiscordRichPresence::TryGetUIText(const std::string& p_Key, ZString& p_OutText) {
    const std::uint32_t s_Hash = Hash::Crc32(p_Key.data(), p_Key.size());
    int s_MarkupResult = 0;

    return SDK()->Functions()->ZUIText_TryGetTextFromNameHash->Call(SDK()->Globals()->UIText, s_Hash, p_OutText, s_MarkupResult);
}

void DiscordRichPresence::ExtractCampaignTitleAndLocation(const ZString& p_CombinedText, ZString& p_OutTitle, ZString& p_OutLocation) {
    std::string s_CombinedText = p_CombinedText.c_str();

    const std::size_t s_SeparatorPosition = s_CombinedText.rfind(',');

    if (s_SeparatorPosition == std::string::npos) {
        p_OutTitle = ToTitleCase(std::move(s_CombinedText));
        p_OutLocation = {};
        return;
    }

    std::string s_Title = s_CombinedText.substr(0, s_SeparatorPosition);

    std::string s_Location = s_CombinedText.substr(s_SeparatorPosition + 1);

    const auto s_Trim = [](std::string& p_Text) {
        const auto s_First = std::ranges::find_if(p_Text, [](unsigned char p_Character) { return !std::isspace(p_Character); });

        const auto s_Last =
            std::ranges::find_if(p_Text | std::views::reverse, [](unsigned char p_Character) { return !std::isspace(p_Character); }).base();

        if (s_First >= s_Last) {
            p_Text.clear();
        }
        else {
            p_Text = std::string(s_First, s_Last);
        }
    };

    s_Trim(s_Title);
    s_Trim(s_Location);

    p_OutTitle = ToTitleCase(std::move(s_Title));
    p_OutLocation = ToTitleCase(std::move(s_Location));
}

std::string DiscordRichPresence::ToTitleCase(std::string p_Text) {
    static constexpr std::array<std::string_view, 8> s_LowercaseWords = {"a", "an", "and", "at", "in", "of", "the", "to"};

    std::ranges::transform(p_Text, p_Text.begin(), [](unsigned char p_Character) { return static_cast<char>(std::tolower(p_Character)); });

    bool s_IsFirstWord = true;
    bool s_IsWordStart = true;
    std::size_t s_WordStart = 0;

    for (std::size_t i = 0; i <= p_Text.size(); ++i) {
        if (i < p_Text.size() && !std::isspace(static_cast<unsigned char>(p_Text[i]))) {
            if (s_IsWordStart) {
                s_WordStart = i;
                s_IsWordStart = false;
            }

            continue;
        }

        if (s_IsWordStart) {
            continue;
        }

        const std::string_view s_Word(p_Text.data() + s_WordStart, i - s_WordStart);

        if (s_IsFirstWord || !std::ranges::contains(s_LowercaseWords, s_Word)) {
            p_Text[s_WordStart] = static_cast<char>(std::toupper(static_cast<unsigned char>(p_Text[s_WordStart])));
        }

        s_IsFirstWord = false;
        s_IsWordStart = true;
    }

    return p_Text;
}

std::string_view DiscordRichPresence::MissionTypeToString(EKntMissionType p_MissionType) {
    switch (p_MissionType) {
    case EKntMissionType::Operation:
        return "Operation";
    case EKntMissionType::Escalation:
        return "Escalation";
    case EKntMissionType::Campaign:
        return "Campaign";
    case EKntMissionType::Hub:
        return "Hub";
    default:
        return "Unknown";
    }
}

std::string DiscordRichPresence::GetActivityImageKey(EKntMissionType p_MissionType, const ZString& p_MissionTitle) {
    if (p_MissionType == EKntMissionType::Hub) {
        return "tacsim";
    }

    std::string_view s_Prefix;

    switch (p_MissionType) {
    case EKntMissionType::Campaign:
        s_Prefix = "mission-";
        break;
    case EKntMissionType::Operation:
    case EKntMissionType::Escalation:
        s_Prefix = "tacsim-";
        break;
    default:
        return "logo";
    }

    std::string s_MissionTitle = p_MissionTitle.c_str();

    std::ranges::transform(s_MissionTitle, s_MissionTitle.begin(), [](unsigned char p_Character) {
        return static_cast<char>(std::tolower(p_Character));
    });

    std::ranges::replace(s_MissionTitle, ' ', '-');

    const auto s_EraseAll = [&s_MissionTitle](std::string_view p_Value) {
        std::size_t s_Position = 0;

        while ((s_Position = s_MissionTitle.find(p_Value, s_Position)) != std::string::npos) {
            s_MissionTitle.erase(s_Position, p_Value.size());
        }
    };

    std::erase(s_MissionTitle, '\'');
    s_EraseAll("\xE2\x80\x98"); // ‘
    s_EraseAll("\xE2\x80\x99"); // ’

    return std::format("{}{}", s_Prefix, s_MissionTitle);
}

DEFINE_PLUGIN_DETOUR(
    DiscordRichPresence, void, ZKntGameProgressionManager_SetCurrentCheckpoint, ZKntGameProgressionManager* th,
    const TEntityRef<ZKntCheckpointEntity>& checkpointEntity
) {
    p_Hook->CallOriginal(th, checkpointEntity);

    UpdateActivity();

    return {HookAction::Return()};
}

DEFINE_ZKNT_PLUGIN(DiscordRichPresence)
