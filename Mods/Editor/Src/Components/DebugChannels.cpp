#include "Editor.hpp"

#include <queue>

#include <directxtk12/SimpleMath.h>

#include <Glacier/ZModule.hpp>
#include <Glacier/SColorRGB.hpp>
#include <Glacier/ZColor.hpp>
#include <Glacier/ZLight.hpp>
#include <Glacier/ZSpawner.hpp>

#include <Logging.hpp>

class ZPFBoxEntity;
class ZPFObstacleEntity;
class ZPFStaticObstacleBoxEntity;
class ZPFSeedPoint;
class ZPathfinderConfigurationBase;
class ZDebugPOIEntity;
class ZDebugTextEntity;
class ZLightEntity;
class ZDarkLightEntity;
class ZBoxReflectionEntity;
class ZCubemapProbeEntity;
class ZParticleEmitterBoxEntity;
class ZParticleEmitterEmitterEntity;
class ZParticleEmitterMeshEntity;
class ZParticleEmitterPointEntity;
class ZParticleGlobalAttractorEntity;
class ZParticleKillVolumeEntity;
class ZGateEntity;
class ZOccluderEntity;
class ZDecalsSpawnEntity;
class ZStaticDecalEntity;
class ZLiquidTrailEntity;
class ZCrowdActorGroupEntity;
class ZCrowdActorGroupFocalPointEntity;
class ZCrowdEntity;
class ZManualActorEntity;
class ZSplineCrowdFlowEntity;
class ZBoxShapeAspect;
class ZCapsuleShapeAspect;
class ZSphereShapeAspect;
class ZWindEntity;
class ZAISoundConnector;
class ZPostfilterAreaBoxEntity;
class ZCheckpointEntity;
class ZKntSpawnpointEntity;
class ZHumanoidSequenceSpatial;
class ZCameraEntity;
class ZGameCameraEntityBase;
class ZBoxVolumeEntity;
class ZSphereVolumeEntity;
class ZCapsuleVolumeEntity;
class ZVolumeShapeEntity;
class ZOrientationEntity;
class ZScatterContainerEntity;
class ZTrailShapeEntity;
class ZSplineEntity;
class ZSplineControlPointEntity;
class ZAudioEmitterSpatialAspect;
class ZAudioEmitterVolumetricAspect;
class ZClothWireEntity;
class ZChildNetworkActEntity;
class ZCompositeEntity;

void Editor::InitializeDebugChannels() {
    m_DebugChannels.push_back(std::make_pair("Guides - pathfinder", DEBUGCHANNEL_GUIDES_PATHFINDER));
    m_DebugChannels.push_back(std::make_pair("Default", DEBUGCHANNEL_DEFAULT));
    m_DebugChannels.push_back(std::make_pair("Light", DEBUGCHANNEL_LIGHT));
    m_DebugChannels.push_back(std::make_pair("Particles", DEBUGCHANNEL_PARTICLES));
    m_DebugChannels.push_back(std::make_pair("Partitioning", DEBUGCHANNEL_PARTITIONING));
    m_DebugChannels.push_back(std::make_pair("Decals", DEBUGCHANNEL_DECALS));
    m_DebugChannels.push_back(std::make_pair("Crowd", DEBUGCHANNEL_CROWD));
    m_DebugChannels.push_back(std::make_pair("Terrain", DEBUGCHANNEL_TERRAIN));
    m_DebugChannels.push_back(std::make_pair("Physics", DEBUGCHANNEL_PHYSICS));
    m_DebugChannels.push_back(std::make_pair("Projectile", DEBUGCHANNEL_PROJECTILE));
    m_DebugChannels.push_back(std::make_pair("AI", DEBUGCHANNEL_AI));
    m_DebugChannels.push_back(std::make_pair("Game", DEBUGCHANNEL_GAME));
    m_DebugChannels.push_back(std::make_pair("Design", DEBUGCHANNEL_DESIGN));
    m_DebugChannels.push_back(std::make_pair("Character", DEBUGCHANNEL_CHARACTER));
    m_DebugChannels.push_back(std::make_pair("Camera", DEBUGCHANNEL_CAMERA));
    m_DebugChannels.push_back(std::make_pair("Debug Camera", DEBUGCHANNEL_DEBUGCAMERA));
    m_DebugChannels.push_back(std::make_pair("Interactions", DEBUGCHANNEL_INTERACTIONS));
    m_DebugChannels.push_back(std::make_pair("Triggers", DEBUGCHANNEL_TRIGGERS));
    m_DebugChannels.push_back(std::make_pair("Engine", DEBUGCHANNEL_ENGINE));
    m_DebugChannels.push_back(std::make_pair("Spline", DEBUGCHANNEL_SPLINE));
    m_DebugChannels.push_back(std::make_pair("Core Logic", DEBUGCHANNEL_CORELOGIC));
    m_DebugChannels.push_back(std::make_pair("Sound", DEBUGCHANNEL_SOUND));
    m_DebugChannels.push_back(std::make_pair("Animation", DEBUGCHANNEL_ANIMATION));
    m_DebugChannels.push_back(std::make_pair("Cloth", DEBUGCHANNEL_CLOTH));
    m_DebugChannels.push_back(std::make_pair("Sound partitioning", DEBUGCHANNEL_SOUND_PARTITIONING));
    m_DebugChannels.push_back(std::make_pair("UI", DEBUGCHANNEL_UI));
    m_DebugChannels.push_back(std::make_pair("Performance", DEBUGCHANNEL_PERFORMANCE));
    m_DebugChannels.push_back(std::make_pair("Vehicle", DEBUGCHANNEL_VEHICLE));

    m_DebugChannelNameToTypeNames["Guides - pathfinder"].push_back("ZPFBoxEntity");
    m_DebugChannelNameToTypeNames["Guides - pathfinder"].push_back("ZPFObstacleEntity");
    m_DebugChannelNameToTypeNames["Guides - pathfinder"].push_back("ZPFStaticObstacleBoxEntity");
    m_DebugChannelNameToTypeNames["Guides - pathfinder"].push_back("ZPFSeedPoint");
    m_DebugChannelNameToTypeNames["Guides - pathfinder"].push_back("ZPathfinderConfigurationBase");

    m_DebugChannelNameToTypeNames["Default"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Default"].push_back("ZDebugPOIEntity");
    m_DebugChannelNameToTypeNames["Default"].push_back("ZDebugTextEntity");

    m_DebugChannelNameToTypeNames["Light"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Light"].push_back("ZLightEntity");
    m_DebugChannelNameToTypeNames["Light"].push_back("ZDarkLightEntity");
    m_DebugChannelNameToTypeNames["Light"].push_back("ZBoxReflectionEntity");
    m_DebugChannelNameToTypeNames["Light"].push_back("ZCubemapProbeEntity");

    m_DebugChannelNameToTypeNames["Particles"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Particles"].push_back("ZParticleEmitterBoxEntity");
    m_DebugChannelNameToTypeNames["Particles"].push_back("ZParticleEmitterEmitterEntity");
    m_DebugChannelNameToTypeNames["Particles"].push_back("ZParticleEmitterMeshEntity");
    m_DebugChannelNameToTypeNames["Particles"].push_back("ZParticleEmitterPointEntity");
    m_DebugChannelNameToTypeNames["Particles"].push_back("ZParticleGlobalAttractorEntity");
    m_DebugChannelNameToTypeNames["Particles"].push_back("ZParticleKillVolumeEntity");

    m_DebugChannelNameToTypeNames["Partitioning"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Partitioning"].push_back("ZGateEntity");
    m_DebugChannelNameToTypeNames["Partitioning"].push_back("ZOccluderEntity");

    m_DebugChannelNameToTypeNames["Decals"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Decals"].push_back("ZDecalsSpawnEntity");
    m_DebugChannelNameToTypeNames["Decals"].push_back("ZStaticDecalEntity");

    m_DebugChannelNameToTypeNames["Crowd"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Crowd"].push_back("ZCrowdActorGroupEntity");
    m_DebugChannelNameToTypeNames["Crowd"].push_back("ZCrowdActorGroupFocalPointEntity");
    m_DebugChannelNameToTypeNames["Crowd"].push_back("ZCrowdEntity");
    m_DebugChannelNameToTypeNames["Crowd"].push_back("ZManualActorEntity");
    m_DebugChannelNameToTypeNames["Crowd"].push_back("ZSplineCrowdFlowEntity");

    m_DebugChannelNameToTypeNames["Terrain"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Terrain"].push_back("ZHeightStampEntity");

    m_DebugChannelNameToTypeNames["Physics"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Physics"].push_back("ZBoxShapeAspect");
    m_DebugChannelNameToTypeNames["Physics"].push_back("ZCapsuleShapeAspect");
    m_DebugChannelNameToTypeNames["Physics"].push_back("ZSphereShapeAspect");
    m_DebugChannelNameToTypeNames["Physics"].push_back("ZWindEntity");

    m_DebugChannelNameToTypeNames["Projectile"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["AI"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["AI"].push_back("ZAISoundConnector");

    m_DebugChannelNameToTypeNames["Game"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Game"].push_back("ZPostfilterAreaBoxEntity");
    m_DebugChannelNameToTypeNames["Game"].push_back("ZCheckpointEntity");

    m_DebugChannelNameToTypeNames["Design"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["Character"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Character"].push_back("ZKntSpawnpointEntity");
    m_DebugChannelNameToTypeNames["Character"].push_back("ZDynamicGameplaySpawnerEntryEntity");
    m_DebugChannelNameToTypeNames["Character"].push_back("ZHumanoidSequenceSpatial");

    m_DebugChannelNameToTypeNames["Camera"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Camera"].push_back("ZCameraEntity");
    m_DebugChannelNameToTypeNames["Camera"].push_back("ZGameCameraEntityBase");

    m_DebugChannelNameToTypeNames["Debug camera"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["Interactions"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["Triggers"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["Engine"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Engine"].push_back("ZBoxVolumeEntity");
    m_DebugChannelNameToTypeNames["Engine"].push_back("ZSphereVolumeEntity");
    m_DebugChannelNameToTypeNames["Engine"].push_back("ZCapsuleVolumeEntity");
    m_DebugChannelNameToTypeNames["Engine"].push_back("ZVolumeShapeEntity");
    m_DebugChannelNameToTypeNames["Engine"].push_back("ZOrientationEntity");
    m_DebugChannelNameToTypeNames["Engine"].push_back("ZScatterContainerEntity");
    m_DebugChannelNameToTypeNames["Engine"].push_back("ZTrailShapeEntity");

    m_DebugChannelNameToTypeNames["Spline"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Spline"].push_back("ZSplineEntity");
    m_DebugChannelNameToTypeNames["Spline"].push_back("ZSplineControlPointEntity");

    m_DebugChannelNameToTypeNames["Core logic"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["Sound"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Sound"].push_back("ZAudioEmitterSpatialAspect");
    m_DebugChannelNameToTypeNames["Sound"].push_back("ZAudioEmitterVolumetricAspect");

    m_DebugChannelNameToTypeNames["Animation"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["Cloth"].push_back("ZDebugGizmoEntity");
    m_DebugChannelNameToTypeNames["Cloth"].push_back("ZClothWireEntity");

    m_DebugChannelNameToTypeNames["Sound partitioning"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["UI"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["Performance"].push_back("ZDebugGizmoEntity");

    m_DebugChannelNameToTypeNames["Vehicle"].push_back("ZDebugGizmoEntity");

    for (const auto& [s_DebugChannelName, s_DebugChannel] : m_DebugChannels) {
        for (const auto& s_TypeName : m_DebugChannelNameToTypeNames[s_DebugChannelName]) {
            m_DebugChannelToTypeNameToGizmoState[s_DebugChannel][s_TypeName] = true;
            m_DebugChannelToTypeNameToShapeState[s_DebugChannel][s_TypeName] = true;
        }
    }
}

void Editor::InitializeDebugEntityTypeIDs() {
    ZTypeRegistry* s_TypeRegistry = (*SDK()->Globals()->TypeRegistry);

    m_DebugEntityTypeIds.resize(DebugEntityTypeName::Count);

    m_DebugEntityTypeIds[DebugEntityTypeName::PFBoxEntity] = s_TypeRegistry->GetTypeID("ZPFBoxEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::PFObstacleEntity] = s_TypeRegistry->GetTypeID("ZPFObstacleEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::PFStaticObstacleBoxEntity] = s_TypeRegistry->GetTypeID("ZPFStaticObstacleBoxEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::PFSeedPoint] = s_TypeRegistry->GetTypeID("ZPFSeedPoint");
    m_DebugEntityTypeIds[DebugEntityTypeName::PathfinderConfigurationBase] = s_TypeRegistry->GetTypeID("ZPathfinderConfigurationBase");
    m_DebugEntityTypeIds[DebugEntityTypeName::DebugGizmoEntity] = s_TypeRegistry->GetTypeID("ZDebugGizmoEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::DebugPOIEntity] = s_TypeRegistry->GetTypeID("ZDebugPOIEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::DebugTextEntity] = s_TypeRegistry->GetTypeID("ZDebugTextEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::LightEntity] = s_TypeRegistry->GetTypeID("ZLightEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::DarkLightEntity] = s_TypeRegistry->GetTypeID("ZDarkLightEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::BoxReflectionEntity] = s_TypeRegistry->GetTypeID("ZBoxReflectionEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::CubemapProbeEntity] = s_TypeRegistry->GetTypeID("ZCubemapProbeEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::ParticleEmitterBoxEntity] = s_TypeRegistry->GetTypeID("ZParticleEmitterBoxEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::ParticleEmitterEmitterEntity] = s_TypeRegistry->GetTypeID("ZParticleEmitterEmitterEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::ParticleEmitterMeshEntity] = s_TypeRegistry->GetTypeID("ZParticleEmitterMeshEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::ParticleEmitterPointEntity] = s_TypeRegistry->GetTypeID("ZParticleEmitterPointEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::ParticleGlobalAttractorEntity] = s_TypeRegistry->GetTypeID("ZParticleGlobalAttractorEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::ParticleKillVolumeEntity] = s_TypeRegistry->GetTypeID("ZParticleKillVolumeEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::GateEntity] = s_TypeRegistry->GetTypeID("ZGateEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::OccluderEntity] = s_TypeRegistry->GetTypeID("ZOccluderEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::DecalsSpawnEntity] = s_TypeRegistry->GetTypeID("ZDecalsSpawnEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::StaticDecalEntity] = s_TypeRegistry->GetTypeID("ZStaticDecalEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::CrowdActorGroupEntity] = s_TypeRegistry->GetTypeID("ZCrowdActorGroupEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::CrowdActorGroupFocalPointEntity] = s_TypeRegistry->GetTypeID("ZCrowdActorGroupFocalPointEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::CrowdEntity] = s_TypeRegistry->GetTypeID("ZCrowdEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::ManualActorEntity] = s_TypeRegistry->GetTypeID("ZManualActorEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::SplineCrowdFlowEntity] = s_TypeRegistry->GetTypeID("ZSplineCrowdFlowEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::HeightStampEntity] = s_TypeRegistry->GetTypeID("ZHeightStampEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::BoxShapeAspect] = s_TypeRegistry->GetTypeID("ZBoxShapeAspect");
    m_DebugEntityTypeIds[DebugEntityTypeName::CapsuleShapeAspect] = s_TypeRegistry->GetTypeID("ZCapsuleShapeAspect");
    m_DebugEntityTypeIds[DebugEntityTypeName::SphereShapeAspect] = s_TypeRegistry->GetTypeID("ZSphereShapeAspect");
    m_DebugEntityTypeIds[DebugEntityTypeName::WindEntity] = s_TypeRegistry->GetTypeID("ZWindEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::AISoundConnector] = s_TypeRegistry->GetTypeID("ZAISoundConnector");
    m_DebugEntityTypeIds[DebugEntityTypeName::PostfilterAreaBoxEntity] = s_TypeRegistry->GetTypeID("ZPostfilterAreaBoxEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::CheckpointEntity] = s_TypeRegistry->GetTypeID("ZCheckpointEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::KntSpawnpointEntity] = s_TypeRegistry->GetTypeID("ZKntSpawnpointEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::DynamicGameplaySpawnerEntryEntity] = s_TypeRegistry->GetTypeID("ZDynamicGameplaySpawnerEntryEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::HumanoidSequenceSpatial] = s_TypeRegistry->GetTypeID("ZHumanoidSequenceSpatial");
    m_DebugEntityTypeIds[DebugEntityTypeName::CameraEntity] = s_TypeRegistry->GetTypeID("ZCameraEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::GameCameraEntityBase] = s_TypeRegistry->GetTypeID("ZGameCameraEntityBase");
    m_DebugEntityTypeIds[DebugEntityTypeName::BoxVolumeEntity] = s_TypeRegistry->GetTypeID("ZBoxVolumeEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::SphereVolumeEntity] = s_TypeRegistry->GetTypeID("ZSphereVolumeEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::CapsuleVolumeEntity] = s_TypeRegistry->GetTypeID("ZCapsuleVolumeEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::VolumeShapeEntity] = s_TypeRegistry->GetTypeID("ZVolumeShapeEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::OrientationEntity] = s_TypeRegistry->GetTypeID("ZOrientationEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::ScatterContainerEntity] = s_TypeRegistry->GetTypeID("ZScatterContainerEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::TrailShapeEntity] = s_TypeRegistry->GetTypeID("ZTrailShapeEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::SplineEntity] = s_TypeRegistry->GetTypeID("ZSplineEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::SplineControlPointEntity] = s_TypeRegistry->GetTypeID("ZSplineControlPointEntity");
    m_DebugEntityTypeIds[DebugEntityTypeName::AudioEmitterSpatialAspect] = s_TypeRegistry->GetTypeID("ZAudioEmitterSpatialAspect");
    m_DebugEntityTypeIds[DebugEntityTypeName::AudioEmitterVolumetricAspect] = s_TypeRegistry->GetTypeID("ZAudioEmitterVolumetricAspect");
    m_DebugEntityTypeIds[DebugEntityTypeName::ClothWireEntity] = s_TypeRegistry->GetTypeID("ZClothWireEntity");
}

void Editor::DrawDebugChannelsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) {
    if (!p_HasFocus || !m_ShowDebugChannelsWindow) {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    const auto s_IsWindowExpanded = ImGui::Begin("Debug channels", &m_ShowDebugChannelsWindow);
    ImGui::PushFont(p_Renderer->GetRegularFont());

    if (s_IsWindowExpanded) {
        if (m_DebugChannels.empty()) {
            InitializeDebugChannels();
            InitializeDebugEntityTypeIDs();
        }

        size_t s_DebugEntityCount;

        {
            std::scoped_lock s_Lock(m_DebugEntitiesMutex);
            s_DebugEntityCount = m_EntityRefToDebugEntities.size();
        }

        if (s_DebugEntityCount) {
            ImGui::Text("Debug entity count: %zu", s_DebugEntityCount);

            ImGui::Separator();

            ImGui::Checkbox("Draw gizmos", &m_DrawGizmos);

            ImGui::Indent();
            ImGui::BeginDisabled(!m_DrawGizmos);

            if (ImGui::RadioButton("Channels/types##Gizmos", m_GizmoDrawMode == DebugDrawMode::SelectedChannelsAndTypes)) {
                m_GizmoDrawMode = DebugDrawMode::SelectedChannelsAndTypes;
            }

            if (ImGui::RadioButton("Selected entity##Gizmos", m_GizmoDrawMode == DebugDrawMode::SelectedEntity)) {
                m_GizmoDrawMode = DebugDrawMode::SelectedEntity;
            }

            if (ImGui::RadioButton("All##Gizmos", m_GizmoDrawMode == DebugDrawMode::All)) {
                m_GizmoDrawMode = DebugDrawMode::All;
            }

            ImGui::EndDisabled();
            ImGui::Unindent();

            ImGui::Separator();

            ImGui::Checkbox("Draw shapes", &m_DrawShapes);

            ImGui::Indent();
            ImGui::BeginDisabled(!m_DrawShapes);

            if (ImGui::RadioButton("Channels/types##Shapes", m_ShapeDrawMode == DebugDrawMode::SelectedChannelsAndTypes)) {
                m_ShapeDrawMode = DebugDrawMode::SelectedChannelsAndTypes;
            }

            if (ImGui::RadioButton("Selected entity##Shapes", m_ShapeDrawMode == DebugDrawMode::SelectedEntity)) {
                m_ShapeDrawMode = DebugDrawMode::SelectedEntity;
            }

            ImGui::EndDisabled();
            ImGui::Unindent();

            const bool s_UseChannelAndTypeNameFilters = (m_DrawGizmos && m_GizmoDrawMode == DebugDrawMode::SelectedChannelsAndTypes)
                                                        || (m_DrawShapes && m_ShapeDrawMode == DebugDrawMode::SelectedChannelsAndTypes);

            ImGui::BeginDisabled(!s_UseChannelAndTypeNameFilters);

            for (const auto& s_Pair : m_DebugChannels) {
                const std::string s_Header = fmt::format("{} ({})", s_Pair.first, m_DebugChannelToDebugEntityCount[s_Pair.second]);

                if (ImGui::CollapsingHeader(s_Header.c_str())) {
                    bool& s_DrawGizmos = m_DebugChannelToGizmoState[s_Pair.second];
                    bool& s_DrawShapes = m_DebugChannelToShapeState[s_Pair.second];

                    ImGui::Checkbox(fmt::format("Draw gizmos##{}", s_Pair.first).c_str(), &s_DrawGizmos);
                    ImGui::Checkbox(fmt::format("Draw shapes##{}", s_Pair.first).c_str(), &s_DrawShapes);

                    ImGui::Separator();

                    const auto& s_TypeNameToDebugEntityCount = m_DebugChannelToTypeNameToDebugEntityCount[s_Pair.second];

                    auto& s_TypeNameToGizmoState = m_DebugChannelToTypeNameToGizmoState[s_Pair.second];

                    auto& s_TypeNameToShapeState = m_DebugChannelToTypeNameToShapeState[s_Pair.second];

                    if (ImGui::BeginTable("DebugEntityTypes", 3)) {
                        const auto s_CenterItem = [](float p_ItemWidth) {
                            const float s_AvailableWidth = ImGui::GetContentRegionAvail().x;

                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (s_AvailableWidth - p_ItemWidth) * 0.5f);
                        };

                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 400.f);
                        ImGui::TableSetupColumn(
                            "Gizmo", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Gizmo").x + ImGui::GetStyle().CellPadding.x * 2.f
                        );
                        ImGui::TableSetupColumn(
                            "Shape", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Shape").x + ImGui::GetStyle().CellPadding.x * 2.f
                        );

                        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

                        ImGui::TableNextColumn();
                        s_CenterItem(ImGui::CalcTextSize("Type").x);
                        ImGui::TextUnformatted("Type");

                        ImGui::TableNextColumn();
                        s_CenterItem(ImGui::CalcTextSize("Gizmo").x);
                        ImGui::TextUnformatted("Gizmo");

                        ImGui::TableNextColumn();
                        s_CenterItem(ImGui::CalcTextSize("Shape").x);
                        ImGui::TextUnformatted("Shape");

                        for (const auto& [s_TypeName, s_Count] : s_TypeNameToDebugEntityCount) {
                            bool& s_DrawGizmo = s_TypeNameToGizmoState[s_TypeName];
                            bool& s_DrawShape = s_TypeNameToShapeState[s_TypeName];

                            ImGui::TableNextRow();

                            ImGui::TableNextColumn();
                            ImGui::AlignTextToFramePadding();
                            ImGui::Text("%s (%u)", s_TypeName.c_str(), s_Count);

                            ImGui::TableNextColumn();
                            ImGui::BeginDisabled(!s_DrawGizmos);
                            s_CenterItem(ImGui::GetFrameHeight());
                            ImGui::Checkbox(fmt::format("##Gizmo{}{}", s_Pair.first, s_TypeName).c_str(), &s_DrawGizmo);
                            ImGui::EndDisabled();

                            ImGui::TableNextColumn();
                            ImGui::BeginDisabled(!s_DrawShapes);
                            s_CenterItem(ImGui::GetFrameHeight());
                            ImGui::Checkbox(fmt::format("##Shape{}{}", s_Pair.first, s_TypeName).c_str(), &s_DrawShape);
                            ImGui::EndDisabled();
                        }

                        ImGui::EndTable();
                    }
                }
            }

            ImGui::EndDisabled();
        }
        else {
            if (ImGui::Button("Get debug entities")) {
                if (!m_CachedEntityTree || !m_CachedEntityTree->m_Entity) {
                    UpdateEntities();
                }

                GetDebugEntities(m_CachedEntityTree);
            }
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Editor::DrawDebugEntities(zknt::IDirectXTKRenderer* p_Renderer) {
    if (!m_DrawGizmos && !m_DrawShapes) {
        return;
    }

    std::scoped_lock s_Lock(m_DebugEntitiesMutex);

    const bool s_DrawGizmosForSelectedEntity = m_DrawGizmos && m_GizmoDrawMode == DebugDrawMode::SelectedEntity;
    const bool s_DrawShapesForSelectedEntity = m_DrawShapes && m_ShapeDrawMode == DebugDrawMode::SelectedEntity;

    const bool s_DrawOtherGizmos = m_DrawGizmos && m_GizmoDrawMode != DebugDrawMode::SelectedEntity;
    const bool s_DrawOtherShapes = m_DrawShapes && m_ShapeDrawMode != DebugDrawMode::SelectedEntity;

    p_Renderer->SetDistanceCullingEnabled(true);

    if (s_DrawGizmosForSelectedEntity || s_DrawShapesForSelectedEntity) {
        const auto s_Iterator = m_EntityRefToDebugEntities.find(m_SelectedEntity);

        if (s_Iterator != m_EntityRefToDebugEntities.end()) {
            for (const auto& s_DebugEntity : s_Iterator->second) {
                if (s_DebugEntity->m_HasGizmo) {
                    if (s_DrawGizmosForSelectedEntity) {
                        DrawGizmo(static_cast<GizmoEntity&>(*s_DebugEntity), p_Renderer);
                    }
                }
                else if (s_DrawShapesForSelectedEntity) {
                    DrawShapes(*s_DebugEntity, p_Renderer);
                }
            }
        }
    }

    if (s_DrawOtherGizmos || s_DrawOtherShapes) {
        for (const auto& [s_EntityRef, s_DebugEntities] : m_EntityRefToDebugEntities) {
            for (const auto& s_DebugEntity : s_DebugEntities) {
                if (s_DebugEntity->m_HasGizmo) {
                    if (s_DrawOtherGizmos) {
                        DrawGizmo(static_cast<GizmoEntity&>(*s_DebugEntity), p_Renderer);
                    }
                }
                else if (s_DrawOtherShapes) {
                    DrawShapes(*s_DebugEntity, p_Renderer);
                }
            }
        }
    }

    p_Renderer->SetDistanceCullingEnabled(false);
}

void Editor::DrawGizmo(GizmoEntity& p_GizmoEntity, zknt::IDirectXTKRenderer* p_Renderer) {
    if (!m_DrawGizmos) {
        return;
    }

    if (m_GizmoDrawMode == DebugDrawMode::SelectedChannelsAndTypes) {
        if (!m_DebugChannelToGizmoState[p_GizmoEntity.m_DebugChannel]) {
            return;
        }

        if (!m_DebugChannelToTypeNameToGizmoState[p_GizmoEntity.m_DebugChannel][p_GizmoEntity.m_TypeName]) {
            return;
        }
    }

    if (p_GizmoEntity.m_DebugChannel == EDebugChannel::DEBUGCHANNEL_PARTITIONING && p_GizmoEntity.m_TypeName == "ZGateEntity") {
        const bool s_IsOpen = p_GizmoEntity.m_EntityRef.GetProperty<bool>("m_bIsOpen").Get();

        if (s_IsOpen && p_GizmoEntity.m_RuntimeResourceID.GetID() != ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_gate_01.prim].prim">) {
            return;
        }

        if (!s_IsOpen && p_GizmoEntity.m_RuntimeResourceID.GetID() == ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_gate_01.prim].prim">) {
            return;
        }
    }

    static STypeID* s_SpatialEntityTypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID("ZSpatialEntity");
    auto s_SpatialEntity = p_GizmoEntity.m_EntityRef.QueryInterface<ZSpatialEntity>(s_SpatialEntityTypeID);

    SMatrix s_Transform = s_SpatialEntity->GetObjectToWorldMatrix() * p_GizmoEntity.m_Transform;

    ZRenderPrimitiveResource* s_pRenderPrimitiveResource = static_cast<ZRenderPrimitiveResource*>(p_GizmoEntity.m_PrimResourcePtr.GetResourceData());

    if (!s_pRenderPrimitiveResource) {
        Logger::Error("[Editor] PRIM of {:016x} gizmo isn't installed!", p_GizmoEntity.m_RuntimeResourceID.GetID());
        return;
    }

    for (const auto& s_Primitive : s_pRenderPrimitiveResource->m_Primitives) {
        ZRenderPrimitiveMesh* s_pRenderPrimitive = static_cast<ZRenderPrimitiveMesh*>(s_Primitive.m_pObject);
        SPrimDrawData* s_PrimDrawData = &(*SDK()->Globals()->PrimDrawData)[s_pRenderPrimitive->m_nPrimDrawDataIndex];

        p_Renderer->DrawMesh(
            s_pRenderPrimitiveResource, s_PrimDrawData->m_pVertexBuffers, 3, s_PrimDrawData->m_pIndexBuffer, s_Transform,
            s_PrimDrawData->vPositionScale, s_PrimDrawData->vPositionBias, s_PrimDrawData->vTextureScaleBias, p_GizmoEntity.m_Color
        );
    }
}

void Editor::DrawShapes(const DebugEntity& p_DebugEntity, zknt::IDirectXTKRenderer* p_Renderer) {
    if (!m_DrawShapes) {
        return;
    }

    if (m_ShapeDrawMode == DebugDrawMode::SelectedChannelsAndTypes) {
        if (!m_DebugChannelToShapeState[p_DebugEntity.m_DebugChannel]) {
            return;
        }

        if (!m_DebugChannelToTypeNameToShapeState[p_DebugEntity.m_DebugChannel][p_DebugEntity.m_TypeName]) {
            return;
        }
    }
}

void Editor::GetDebugEntities(const std::shared_ptr<EntityTreeNode>& p_EntityTreeNode) {
    /*
     * Entity id of queried interface has to be compared with entity id of current node
     * because if current entity is composed entity or if it has exposed interfaces
     * it will have interfaces of it's children.
     *
     * EntityIDMatches method can't be used for ZBoxShapeAspect, ZCapsuleShapeAspect, ZSphereShapeAspect,
     * ZAudioEmitterSpatialAspect, ZAudioEmitterVolumetricAspect and ZClothWireEntity since they are
     * in aspect entity and they don't have entity type.
     */

    if (!p_EntityTreeNode || p_EntityTreeNode->m_IsPendingDeletion.load(std::memory_order_acquire)) {
        return;
    }

    if (IsSpecialEntityTreeNode(p_EntityTreeNode->m_Entity)) {
        for (const auto& [_, s_Child] : p_EntityTreeNode->m_Children) {
            GetDebugEntities(s_Child);
        }

        return;
    }

    static const SVector4 s_Color = SVector4(1.f, 1.f, 1.f, 1.f);

    static STypeID* s_CompositeEntityTypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID("ZCompositeEntity");

    auto s_LightEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZLightEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::LightEntity]);
    auto s_DarkLightEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZDarkLightEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::DarkLightEntity]);

    if (auto s_PFBoxEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZPFBoxEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::PFBoxEntity])) {
        if (EntityIDMatches(s_PFBoxEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZPFBoxEntity", DEBUGCHANNEL_GUIDES_PATHFINDER, "m_pHelper");
        }
    }
    else if (
        auto s_PFObstacleEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZPFObstacleEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::PFObstacleEntity])
    ) {
        if (EntityIDMatches(s_PFObstacleEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZPFObstacleEntity", DEBUGCHANNEL_GUIDES_PATHFINDER, "m_pHelper");
        }
    }
    else if (
        auto s_PFStaticObstacleBoxEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZPFStaticObstacleBoxEntity>(
            m_DebugEntityTypeIds[DebugEntityTypeName::PFStaticObstacleBoxEntity]
        )
    ) {
        if (EntityIDMatches(s_PFStaticObstacleBoxEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZPFStaticObstacleBoxEntity", DEBUGCHANNEL_GUIDES_PATHFINDER, "m_pHelper");
        }
    }
    else if (auto s_PFSeedPoint = p_EntityTreeNode->m_Entity.QueryInterface<ZPFSeedPoint>(m_DebugEntityTypeIds[DebugEntityTypeName::PFSeedPoint])) {
        if (EntityIDMatches(s_PFSeedPoint, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZPFSeedPoint", DEBUGCHANNEL_GUIDES_PATHFINDER, "m_pHelper");
        }
    }
    else if (
        auto s_PathfinderConfigurationBase = p_EntityTreeNode->m_Entity.QueryInterface<ZPathfinderConfigurationBase>(
            m_DebugEntityTypeIds[DebugEntityTypeName::PathfinderConfigurationBase]
        )
    ) {
        if (EntityIDMatches(s_PathfinderConfigurationBase, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZPathfinderConfigurationBase", DEBUGCHANNEL_GUIDES_PATHFINDER, "m_pHelper");
        }
    }
    else if (
        auto s_DebugGizmoEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZDebugGizmoEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::DebugGizmoEntity])
    ) {
        if (EntityIDMatches(s_DebugGizmoEntity, p_EntityTreeNode->m_EntityID)) {
            const ZDebugGizmoEntity::EDrawLayer s_DrawLayer =
                p_EntityTreeNode->m_Entity.GetProperty<ZDebugGizmoEntity::EDrawLayer>("m_eDrawLayer").Get();
            const EDebugChannel s_DebugChannel = ConvertDrawLayerToDebugChannel(s_DrawLayer);

            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZDebugGizmoEntity", s_DebugChannel, "m_GizmoGeomRID");
        }
    }
    else if (
        auto s_DebugPOIEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZDebugPOIEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::DebugPOIEntity])
    ) {
        if (EntityIDMatches(s_DebugPOIEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZDebugPOIEntity", DEBUGCHANNEL_DEFAULT, "m_pDebugGizmoResource");
        }
    }
    else if (
        auto s_DebugTextEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZDebugTextEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::DebugTextEntity])
    ) {
        if (EntityIDMatches(s_DebugTextEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZDebugTextEntity", DEBUGCHANNEL_DEFAULT, "m_pDebugGizmoResource");
        }
    }
    else if (s_LightEntity || s_DarkLightEntity) {
        if (EntityIDMatches(s_LightEntity, p_EntityTreeNode->m_EntityID) || EntityIDMatches(s_DarkLightEntity, p_EntityTreeNode->m_EntityID)) {
            std::string s_TypeName;

            if (s_LightEntity) {
                s_TypeName = "ZLightEntity";
            }
            else {
                s_TypeName = "ZDarkLightEntity";
            }

            const ILightEntity::ELightType s_LightType = p_EntityTreeNode->m_Entity.GetProperty<ILightEntity::ELightType>("m_eLightType").Get();

            switch (s_LightType) {
            case ILightEntity::ELightType::LT_DIRECTIONAL:
            case ILightEntity::ELightType::LT_ENVIRONMENT:
            case ILightEntity::ELightType::LT_AREA_QUAD:
                AddGizmoEntity(
                    p_EntityTreeNode->m_Entity, s_TypeName, DEBUGCHANNEL_LIGHT,
                    ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_light_directional_01.prim].prim">
                );
                break;
            case ILightEntity::ELightType::LT_OMNI:
                AddGizmoEntity(
                    p_EntityTreeNode->m_Entity, s_TypeName, DEBUGCHANNEL_LIGHT,
                    ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_light_01.prim].prim">
                );
                break;
            case ILightEntity::ELightType::LT_SPOT:
            case ILightEntity::ELightType::LT_SQUARESPOT:
                AddGizmoEntity(
                    p_EntityTreeNode->m_Entity, s_TypeName, DEBUGCHANNEL_LIGHT,
                    ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_light_spot_01.prim].prim">
                );
                break;
            case ILightEntity::ELightType::LT_CAPSULE:
                AddGizmoEntity(
                    p_EntityTreeNode->m_Entity, s_TypeName, DEBUGCHANNEL_LIGHT,
                    ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_light_capsule_01.prim].prim">
                );
                break;
            }
        }
    }
    else if (
        auto s_BoxReflectionEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZBoxReflectionEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::BoxReflectionEntity])
    ) {
        if (EntityIDMatches(s_BoxReflectionEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZBoxReflectionEntity", DEBUGCHANNEL_LIGHT,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_cubemap_01.prim].prim">
            );
        }
    }
    else if (
        auto s_CubemapProbeEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZCubemapProbeEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::CubemapProbeEntity])
    ) {
        if (EntityIDMatches(s_CubemapProbeEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZCubemapProbeEntity", DEBUGCHANNEL_LIGHT, "m_pHelper");
        }
    }
    else if (
        auto s_ParticleEmitterBoxEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZParticleEmitterBoxEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::ParticleEmitterBoxEntity])
    ) {
        if (EntityIDMatches(s_ParticleEmitterBoxEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZParticleEmitterBoxEntity", DEBUGCHANNEL_PARTICLES,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_particles_01.prim].prim">
            );
        }
    }
    else if (
        auto s_ParticleEmitterEmitterEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZParticleEmitterEmitterEntity>(
            m_DebugEntityTypeIds[DebugEntityTypeName::ParticleEmitterEmitterEntity]
        )
    ) {
        if (EntityIDMatches(s_ParticleEmitterEmitterEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZParticleEmitterEmitterEntity", DEBUGCHANNEL_PARTICLES,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_particles_01.prim].prim">
            );
        }
    }
    else if (
        auto s_ParticleEmitterMeshEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZParticleEmitterMeshEntity>(
            m_DebugEntityTypeIds[DebugEntityTypeName::ParticleEmitterMeshEntity]
        )
    ) {
        if (EntityIDMatches(s_ParticleEmitterMeshEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZParticleEmitterMeshEntity", DEBUGCHANNEL_PARTICLES,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_particles_01.prim].prim">
            );
        }
    }
    else if (
        auto s_ParticleEmitterPointEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZParticleEmitterPointEntity>(
            m_DebugEntityTypeIds[DebugEntityTypeName::ParticleEmitterPointEntity]
        )
    ) {
        if (EntityIDMatches(s_ParticleEmitterPointEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZParticleEmitterPointEntity", DEBUGCHANNEL_PARTICLES,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_particles_01.prim].prim">
            );
        }
    }
    else if (
        auto s_ParticleGlobalAttractorEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZParticleGlobalAttractorEntity>(
            m_DebugEntityTypeIds[DebugEntityTypeName::ParticleGlobalAttractorEntity]
        )
    ) {
        if (EntityIDMatches(s_ParticleGlobalAttractorEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZParticleGlobalAttractorEntity", DEBUGCHANNEL_PARTICLES, "m_pHelper");
        }
    }
    else if (
        auto s_ParticleKillVolumeEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZParticleKillVolumeEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::ParticleKillVolumeEntity])
    ) {
        if (EntityIDMatches(s_ParticleKillVolumeEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZParticleKillVolumeEntity", DEBUGCHANNEL_PARTICLES, "m_pHelper");
        }
    }
    else if (auto s_GateEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZGateEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::GateEntity])) {
        if (EntityIDMatches(s_GateEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZGateEntity", DEBUGCHANNEL_PARTITIONING, "m_pHelper");
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZGateEntity", DEBUGCHANNEL_PARTITIONING, "m_pHelperClosed");
        }
    }
    else if (
        auto s_OccluderEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZOccluderEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::OccluderEntity])
    ) {
        if (EntityIDMatches(s_OccluderEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZOccluderEntity", DEBUGCHANNEL_PARTITIONING, "m_pHelper");
        }
    }
    else if (
        auto s_DecalsSpawnEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZDecalsSpawnEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::DecalsSpawnEntity])
    ) {
        if (EntityIDMatches(s_DecalsSpawnEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZDecalsSpawnEntity", DEBUGCHANNEL_DECALS, "m_pHelper");
        }
    }
    else if (
        auto s_StaticDecalEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZStaticDecalEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::StaticDecalEntity])
    ) {
        if (EntityIDMatches(s_StaticDecalEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZStaticDecalEntity", DEBUGCHANNEL_DECALS,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_decal_01.prim].prim">
            );
        }
    }
    else if (
        auto s_CrowdActorGroupEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZCrowdActorGroupEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::CrowdActorGroupEntity])
    ) {
        if (EntityIDMatches(s_CrowdActorGroupEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZCrowdActorGroupEntity", DEBUGCHANNEL_CROWD, "m_pHelper");
        }
    }
    else if (
        auto s_CrowdActorGroupFocalPointEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCrowdActorGroupFocalPointEntity>(
            m_DebugEntityTypeIds[DebugEntityTypeName::CrowdActorGroupFocalPointEntity]
        )
    ) {
        if (EntityIDMatches(s_CrowdActorGroupFocalPointEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZCrowdActorGroupFocalPointEntity", DEBUGCHANNEL_CROWD, "m_pHelper");
        }
    }
    else if (auto s_CrowdEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCrowdEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::CrowdEntity])) {
        if (EntityIDMatches(s_CrowdEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZCrowdEntity", DEBUGCHANNEL_CROWD, "m_pGizmo");
        }
    }
    else if (
        auto s_ManualActorEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZManualActorEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::ManualActorEntity])
    ) {
        if (EntityIDMatches(s_ManualActorEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZManualActorEntity", DEBUGCHANNEL_CROWD, "m_pGizmo");
        }
    }
    else if (
        auto s_SplineCrowdFlowEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZSplineCrowdFlowEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::SplineCrowdFlowEntity])
    ) {
        if (EntityIDMatches(s_SplineCrowdFlowEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZSplineCrowdFlowEntity", DEBUGCHANNEL_CROWD, "m_pHelper");
        }
    }
    else if (
        auto s_SplineCrowdFlowEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZSplineCrowdFlowEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::HeightStampEntity])
    ) {
        if (EntityIDMatches(s_SplineCrowdFlowEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZSplineCrowdFlowEntity", DEBUGCHANNEL_TERRAIN, "m_pHelper");
        }
    }
    else if (p_EntityTreeNode->m_Entity.QueryInterface<ZBoxShapeAspect>(m_DebugEntityTypeIds[DebugEntityTypeName::BoxShapeAspect])) {
        const bool s_IsCompositeEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCompositeEntity>(s_CompositeEntityTypeID);

        if (!s_IsCompositeEntity) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZBoxShapeAspect", DEBUGCHANNEL_PHYSICS,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_colibox.prim].prim">
            );
        }
    }
    else if (p_EntityTreeNode->m_Entity.QueryInterface<ZCapsuleShapeAspect>(m_DebugEntityTypeIds[DebugEntityTypeName::CapsuleShapeAspect])) {
        const bool s_IsCompositeEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCompositeEntity>(s_CompositeEntityTypeID);

        if (!s_IsCompositeEntity) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZCapsuleShapeAspect", DEBUGCHANNEL_PHYSICS,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_colicapsule.prim].prim">
            );
        }
    }
    else if (p_EntityTreeNode->m_Entity.QueryInterface<ZSphereShapeAspect>(m_DebugEntityTypeIds[DebugEntityTypeName::SphereShapeAspect])) {
        const bool s_IsCompositeEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCompositeEntity>(s_CompositeEntityTypeID);

        if (!s_IsCompositeEntity) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZSphereShapeAspect", DEBUGCHANNEL_PHYSICS,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_colisphere.prim].prim">
            );
        }
    }
    else if (auto s_WindEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZWindEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::WindEntity])) {
        if (EntityIDMatches(s_WindEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZWindEntity", DEBUGCHANNEL_PHYSICS, "m_pHelperGizmo");
        }
    }
    else if (
        auto s_AISoundConnector =
            p_EntityTreeNode->m_Entity.QueryInterface<ZAISoundConnector>(m_DebugEntityTypeIds[DebugEntityTypeName::AISoundConnector])
    ) {
        if (EntityIDMatches(s_AISoundConnector, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZAISoundConnector", DEBUGCHANNEL_AI,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_annotation_01.prim].prim">
            );
        }
    }
    else if (
        auto s_PostfilterAreaBoxEntit =
            p_EntityTreeNode->m_Entity.QueryInterface<ZPostfilterAreaBoxEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::PostfilterAreaBoxEntity])
    ) {
        if (EntityIDMatches(s_PostfilterAreaBoxEntit, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZPostfilterAreaBoxEntity", DEBUGCHANNEL_GAME, "m_pHelper");
        }
    }
    else if (
        auto s_CheckpointEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZCheckpointEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::CheckpointEntity])
    ) {
        if (EntityIDMatches(s_CheckpointEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZCheckpointEntity", DEBUGCHANNEL_GAME, "m_pGizmo");
        }
    }
    else if (
        auto s_KntSpawnpointEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZKntSpawnpointEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::KntSpawnpointEntity])
    ) {
        if (EntityIDMatches(s_KntSpawnpointEntity, p_EntityTreeNode->m_EntityID)) {
            const ZRuntimeResourceID s_HumanoidTemplate = p_EntityTreeNode->m_Entity.GetProperty<ZRuntimeResourceID>("m_humanoidTemplate").Get();
            /*bool s_ResourceExists = false;

            if (s_HumanoidTemplate.IsValid()) {
                for (const auto& s_ResourceInfo : (*SDK()->Globals()->ResourceContainer)->m_resources) {
                    if (s_ResourceInfo.rid == s_HumanoidTemplate) {
                        s_ResourceExists = true;
                        break;
                    }
                }
            }*/

            if (s_HumanoidTemplate.IsValid()) {
                AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZKntSpawnpointEntity", DEBUGCHANNEL_CHARACTER, "m_validCharacterGizmoResource");
            }
            else {
                AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZKntSpawnpointEntity", DEBUGCHANNEL_CHARACTER, "m_invalidCharacterGizmoResource");
            }
        }
    }
    else if (
        auto s_DynamicGameplaySpawnerEntryEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZDynamicGameplaySpawnerEntryEntity>(
            m_DebugEntityTypeIds[DebugEntityTypeName::DynamicGameplaySpawnerEntryEntity]
        )
    ) {
        if (EntityIDMatches(s_DynamicGameplaySpawnerEntryEntity, p_EntityTreeNode->m_EntityID)) {
            if (s_DynamicGameplaySpawnerEntryEntity->IsTemplateValid()) {
                AddGizmoEntity(
                    p_EntityTreeNode->m_Entity, "ZDynamicGameplaySpawnerEntryEntity", DEBUGCHANNEL_CHARACTER, "m_validCharacterGizmoResource"
                );
            }
            else {
                AddGizmoEntity(
                    p_EntityTreeNode->m_Entity, "ZDynamicGameplaySpawnerEntryEntity", DEBUGCHANNEL_CHARACTER, "m_invalidCharacterGizmoResource"
                );
            }
        }
    }
    else if (
        auto s_HumanoidSequenceSpatial =
            p_EntityTreeNode->m_Entity.QueryInterface<ZHumanoidSequenceSpatial>(m_DebugEntityTypeIds[DebugEntityTypeName::HumanoidSequenceSpatial])
    ) {
        if (EntityIDMatches(s_HumanoidSequenceSpatial, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZHumanoidSequenceSpatial", DEBUGCHANNEL_CHARACTER, "m_gizmoResource");
        }
    }
    else if (
        auto s_CameraEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCameraEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::CameraEntity])
    ) {
        if (EntityIDMatches(s_CameraEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZCameraEntity", DEBUGCHANNEL_CAMERA, "m_pHelper");
        }
    }
    else if (
        auto s_GameCameraEntityBase =
            p_EntityTreeNode->m_Entity.QueryInterface<ZGameCameraEntityBase>(m_DebugEntityTypeIds[DebugEntityTypeName::GameCameraEntityBase])
    ) {
        if (EntityIDMatches(s_GameCameraEntityBase, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZGameCameraEntityBase", DEBUGCHANNEL_CAMERA, "m_pDebugGizmoResource");
        }
    }
    else if (
        auto s_BoxVolumeEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZBoxVolumeEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::BoxVolumeEntity])
    ) {
        if (EntityIDMatches(s_BoxVolumeEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZBoxVolumeEntity", DEBUGCHANNEL_ENGINE, "m_pHelper");
        }
    }
    else if (
        auto s_SphereVolumeEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZSphereVolumeEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::SphereVolumeEntity])
    ) {
        if (EntityIDMatches(s_SphereVolumeEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZSphereVolumeEntity", DEBUGCHANNEL_ENGINE, "m_pHelper");
        }
    }
    else if (
        auto s_CapsuleVolumeEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZCapsuleVolumeEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::CapsuleVolumeEntity])
    ) {
        if (EntityIDMatches(s_CapsuleVolumeEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZCapsuleVolumeEntity", DEBUGCHANNEL_ENGINE, "m_pHelper");
        }
    }
    else if (
        auto s_VolumeShapeEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZVolumeShapeEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::VolumeShapeEntity])
    ) {
        if (EntityIDMatches(s_VolumeShapeEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZVolumeShapeEntity", DEBUGCHANNEL_ENGINE, "m_pHelper");
        }
    }
    else if (
        auto s_OrientationEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZOrientationEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::OrientationEntity])
    ) {
        if (EntityIDMatches(s_OrientationEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZOrientationEntity", DEBUGCHANNEL_ENGINE, "m_pHelper");
        }
    }
    else if (
        auto s_ScatterContainerEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZScatterContainerEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::ScatterContainerEntity])
    ) {
        if (EntityIDMatches(s_ScatterContainerEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZScatterContainerEntity", DEBUGCHANNEL_ENGINE, "m_pHelperGizmo");
        }
    }
    else if (
        auto s_TrailShapeEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZTrailShapeEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::TrailShapeEntity])
    ) {
        if (EntityIDMatches(s_TrailShapeEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZTrailShapeEntity", DEBUGCHANNEL_ENGINE, "m_pHelper");
        }
    }
    else if (
        auto s_SplineEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZSplineEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::SplineEntity])
    ) {
        if (EntityIDMatches(s_SplineEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZSplineEntity", DEBUGCHANNEL_SPLINE, "m_pSplineGizmo");
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZSplineEntity", DEBUGCHANNEL_SPLINE, "m_pMarkerGizmo");
        }
    }
    else if (
        auto s_SplineControlPointEntity =
            p_EntityTreeNode->m_Entity.QueryInterface<ZSplineControlPointEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::SplineControlPointEntity])
    ) {
        if (EntityIDMatches(s_SplineControlPointEntity, p_EntityTreeNode->m_EntityID)) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZSplineControlPointEntity", DEBUGCHANNEL_SPLINE, "m_pControlPointGizmo");
        }
    }
    else if (
        p_EntityTreeNode->m_Entity.QueryInterface<ZAudioEmitterSpatialAspect>(m_DebugEntityTypeIds[DebugEntityTypeName::AudioEmitterSpatialAspect])
    ) {
        const bool s_IsCompositeEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCompositeEntity>(s_CompositeEntityTypeID);

        if (!s_IsCompositeEntity) {
            AddGizmoEntity(
                p_EntityTreeNode->m_Entity, "ZAudioEmitterSpatialAspect", DEBUGCHANNEL_SOUND,
                ResId<"[assembly:/_glacier/geometry/gizmos.wl2?/gizmo_sound_01.prim].prim">
            );
        }
    }
    else if (
        p_EntityTreeNode->m_Entity.QueryInterface<ZAudioEmitterVolumetricAspect>(
            m_DebugEntityTypeIds[DebugEntityTypeName::AudioEmitterVolumetricAspect]
        )
    ) {
        const bool s_IsCompositeEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCompositeEntity>(s_CompositeEntityTypeID);

        if (!s_IsCompositeEntity) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZAudioEmitterVolumetricAspect", DEBUGCHANNEL_SOUND, "m_pHelper");
        }
    }
    else if (p_EntityTreeNode->m_Entity.QueryInterface<ZClothWireEntity>(m_DebugEntityTypeIds[DebugEntityTypeName::ClothWireEntity])) {
        const bool s_IsCompositeEntity = p_EntityTreeNode->m_Entity.QueryInterface<ZCompositeEntity>(s_CompositeEntityTypeID);

        if (!s_IsCompositeEntity) {
            AddGizmoEntity(p_EntityTreeNode->m_Entity, "ZClothWireEntity", DEBUGCHANNEL_CLOTH, "m_pHelper");
        }
    }

    for (const auto& [_, s_Child] : p_EntityTreeNode->m_Children) {
        GetDebugEntities(s_Child);
    }
}

void Editor::AddDebugEntity(const ZEntityRef p_EntityRef, const std::string& p_TypeName, const EDebugChannel p_DebugChannel) {
    auto s_DebugEntity = std::make_unique<DebugEntity>();

    s_DebugEntity->m_TypeName = p_TypeName;
    s_DebugEntity->m_EntityRef = p_EntityRef;
    s_DebugEntity->m_DebugChannel = p_DebugChannel;
    s_DebugEntity->m_HasGizmo = false;

    std::scoped_lock s_Lock(m_DebugEntitiesMutex);
    m_EntityRefToDebugEntities[p_EntityRef].push_back(std::move(s_DebugEntity));

    ++m_DebugChannelToDebugEntityCount[p_DebugChannel];
    ++m_DebugChannelToTypeNameToDebugEntityCount[p_DebugChannel][p_TypeName];
}

void Editor::AddGizmoEntity(
    const ZEntityRef p_EntityRef, const std::string& p_TypeName, const EDebugChannel p_DebugChannel, const ZRuntimeResourceID p_RuntimeResourceID,
    const SVector4& p_Color, const SMatrix& p_Transform
) {
    ZResourcePtr s_ResourcePtr;

    SDK()->Globals()->ResourceManager->GetResourcePtr(s_ResourcePtr, p_RuntimeResourceID, 0);

    auto s_GizmoEntity = std::make_unique<GizmoEntity>();

    s_GizmoEntity->m_TypeName = p_TypeName;
    s_GizmoEntity->m_EntityRef = p_EntityRef;
    s_GizmoEntity->m_DebugChannel = p_DebugChannel;
    s_GizmoEntity->m_HasGizmo = true;
    s_GizmoEntity->m_RuntimeResourceID = p_RuntimeResourceID;
    s_GizmoEntity->m_PrimResourcePtr = s_ResourcePtr;
    s_GizmoEntity->m_Color = p_Color;
    s_GizmoEntity->m_Transform = p_Transform;

    std::scoped_lock s_Lock(m_DebugEntitiesMutex);
    m_EntityRefToDebugEntities[p_EntityRef].push_back(std::move(s_GizmoEntity));

    ++m_DebugChannelToDebugEntityCount[p_DebugChannel];
    ++m_DebugChannelToTypeNameToDebugEntityCount[p_DebugChannel][p_TypeName];
}

void Editor::AddGizmoEntity(
    const ZEntityRef p_EntityRef, const std::string& p_TypeName, const EDebugChannel p_DebugChannel, const std::string& p_PropertyName,
    const SVector4& p_Color, const SMatrix& p_Transform
) {
    const ZResourcePtr s_ResourcePtr = p_EntityRef.GetProperty<ZResourcePtr>(p_PropertyName).Get();
    const ZRuntimeResourceID s_RuntimeResourceID;

    if (s_ResourcePtr.m_ResourceIndex.val != -1) {
        const ZRuntimeResourceID s_RuntimeResourceID = (*SDK()->Globals()->ResourceContainer)->m_resources[s_ResourcePtr.m_ResourceIndex.val].rid;

        auto s_GizmoEntity = std::make_unique<GizmoEntity>();

        s_GizmoEntity->m_TypeName = p_TypeName;
        s_GizmoEntity->m_EntityRef = p_EntityRef;
        s_GizmoEntity->m_DebugChannel = p_DebugChannel;
        s_GizmoEntity->m_HasGizmo = true;
        s_GizmoEntity->m_RuntimeResourceID = s_RuntimeResourceID;
        s_GizmoEntity->m_PrimResourcePtr = s_ResourcePtr;
        s_GizmoEntity->m_Color = p_Color;
        s_GizmoEntity->m_Transform = p_Transform;

        std::scoped_lock s_Lock(m_DebugEntitiesMutex);
        m_EntityRefToDebugEntities[p_EntityRef].push_back(std::move(s_GizmoEntity));

        ++m_DebugChannelToDebugEntityCount[p_DebugChannel];
        ++m_DebugChannelToTypeNameToDebugEntityCount[p_DebugChannel][p_TypeName];
    }
    else {
        const uint64_t s_EntityId = p_EntityRef.GetEntity()->GetType()->m_nEntityID;

        Logger::Error("[Editor] Hash of gizmo is missing for entity with {:016x} id and {} type!", s_EntityId, p_TypeName);
    }
}

void Editor::DeleteDebugEntities(const std::shared_ptr<EntityTreeNode>& p_RootNode) {
    std::scoped_lock s_Lock(m_DebugEntitiesMutex);

    std::queue<std::shared_ptr<EntityTreeNode>> s_Nodes;
    s_Nodes.push(p_RootNode);

    while (!s_Nodes.empty()) {
        const auto s_Node = s_Nodes.front();
        s_Nodes.pop();

        const auto s_Iterator = m_EntityRefToDebugEntities.find(s_Node->m_Entity);

        if (s_Iterator != m_EntityRefToDebugEntities.end()) {
            for (const auto& s_DebugEntity : s_Iterator->second) {
                const EDebugChannel s_DebugChannel = s_DebugEntity->m_DebugChannel;
                const std::string& s_TypeName = s_DebugEntity->m_TypeName;

                auto& s_ChannelCount = m_DebugChannelToDebugEntityCount[s_DebugChannel];
                auto& s_TypeCount = m_DebugChannelToTypeNameToDebugEntityCount[s_DebugChannel][s_TypeName];

                if (s_ChannelCount > 0) {
                    --s_ChannelCount;
                }

                if (s_TypeCount > 0) {
                    --s_TypeCount;
                }
            }

            m_EntityRefToDebugEntities.erase(s_Iterator);
        }

        for (const auto& [_, s_Child] : s_Node->m_Children) {
            s_Nodes.push(s_Child);
        }
    }
}

EDebugChannel Editor::ConvertDrawLayerToDebugChannel(ZDebugGizmoEntity::EDrawLayer p_DrawLayer) {
    switch (p_DrawLayer) {
    case ZDebugGizmoEntity::EDrawLayer::DL_DEFAULT:
        return DEBUGCHANNEL_DEFAULT;
    case ZDebugGizmoEntity::EDrawLayer::DL_LIGHT:
        return DEBUGCHANNEL_LIGHT;
    case ZDebugGizmoEntity::EDrawLayer::DL_PARTICLES:
        return DEBUGCHANNEL_PARTICLES;
    case ZDebugGizmoEntity::EDrawLayer::DL_PARTITIONING:
        return DEBUGCHANNEL_PARTITIONING;
    case ZDebugGizmoEntity::EDrawLayer::DL_DECALS:
        return DEBUGCHANNEL_DECALS;
    case ZDebugGizmoEntity::EDrawLayer::DL_CROWD:
        return DEBUGCHANNEL_CROWD;
    case ZDebugGizmoEntity::EDrawLayer::DL_TERRAIN:
        return DEBUGCHANNEL_TERRAIN;
    case ZDebugGizmoEntity::EDrawLayer::DL_PHYSICS:
        return DEBUGCHANNEL_PHYSICS;
    case ZDebugGizmoEntity::EDrawLayer::DL_PROJECTILE:
        return DEBUGCHANNEL_PROJECTILE;
    case ZDebugGizmoEntity::EDrawLayer::DL_AI:
        return DEBUGCHANNEL_AI;
    case ZDebugGizmoEntity::EDrawLayer::DL_GAME:
        return DEBUGCHANNEL_GAME;
    case ZDebugGizmoEntity::EDrawLayer::DL_DESIGN:
        return DEBUGCHANNEL_DESIGN;
    case ZDebugGizmoEntity::EDrawLayer::DL_CHARACTER:
        return DEBUGCHANNEL_CHARACTER;
    case ZDebugGizmoEntity::EDrawLayer::DL_CAMERA:
        return DEBUGCHANNEL_CAMERA;
    case ZDebugGizmoEntity::EDrawLayer::DL_DEBUGCAMERA:
        return DEBUGCHANNEL_DEBUGCAMERA;
    case ZDebugGizmoEntity::EDrawLayer::DL_INTERACTIONS:
        return DEBUGCHANNEL_INTERACTIONS;
    case ZDebugGizmoEntity::EDrawLayer::DL_TRIGGERS:
        return DEBUGCHANNEL_TRIGGERS;
    case ZDebugGizmoEntity::EDrawLayer::DL_ENGINE:
        return DEBUGCHANNEL_ENGINE;
    case ZDebugGizmoEntity::EDrawLayer::DL_SPLINE:
        return DEBUGCHANNEL_SPLINE;
    case ZDebugGizmoEntity::EDrawLayer::DL_CORELOGIC:
        return DEBUGCHANNEL_CORELOGIC;
    case ZDebugGizmoEntity::EDrawLayer::DL_SOUND:
        return DEBUGCHANNEL_SOUND;
    case ZDebugGizmoEntity::EDrawLayer::DL_ANIMATION:
        return DEBUGCHANNEL_ANIMATION;
    case ZDebugGizmoEntity::EDrawLayer::DL_CLOTH:
        return DEBUGCHANNEL_CLOTH;
    case ZDebugGizmoEntity::EDrawLayer::DL_SOUND_PARTITIONING:
        return DEBUGCHANNEL_SOUND_PARTITIONING;
    case ZDebugGizmoEntity::EDrawLayer::DL_UI:
        return DEBUGCHANNEL_UI;
    case ZDebugGizmoEntity::EDrawLayer::DL_PERFORMANCE:
        return DEBUGCHANNEL_PERFORMANCE;
    case ZDebugGizmoEntity::EDrawLayer::DL_VEHICLE:
        return DEBUGCHANNEL_VEHICLE;
    default:
        return DEBUGCHANNEL_NONE;
    }
}

bool Editor::EntityIDMatches(void* p_Interface, const uint64 p_EntityID) {
    auto s_EntityType = reinterpret_cast<ZEntityType**>(reinterpret_cast<uintptr_t>(p_Interface) + 8);

    if (s_EntityType && *s_EntityType && (*s_EntityType)->m_nEntityID == p_EntityID) {
        return true;
    }

    return false;
}

bool Editor::RayCastDebugEntities(const SVector3& p_WorldPosition, const SVector3& p_Direction) {
    RayCastResult s_Result;

    RayCastGizmos(p_WorldPosition, p_Direction, s_Result);
    RayCastShapes(p_WorldPosition, p_Direction, s_Result);

    if (!s_Result.m_Entity) {
        if (m_RaycastLogging) {
            Logger::Debug("[Editor] RayCastDebugEntities found no hits.");
        }

        m_SelectedDebugEntity = {};

        return false;
    }

    m_SelectedDebugEntity = s_Result.m_Entity;

    if (m_RaycastLogging) {
        Logger::Debug(
            "[Editor] RayCastDebugEntities hit '{}' (channel {}) at distance {}", s_Result.m_TypeName, static_cast<int32_t>(s_Result.m_DebugChannel),
            s_Result.m_Distance
        );
    }

    const auto s_EntitySceneContext = SDK()->Globals()->GameSceneflowModule->m_pEntitySceneContext;
    bool s_IsParentFound = false;

    for (const auto& s_Brick : s_EntitySceneContext->m_SceneConfig->m_aMainBricks) {
        if (m_SelectedDebugEntity.IsAnyParent(s_Brick.m_EntityType)) {
            s_IsParentFound = true;
            Logger::Debug("[Editor] Found entity in brick {}.", s_Brick.m_RuntimeResourceID);
            break;
        }
    }

    if (!s_IsParentFound) {
        for (const auto& [s_RuntimeResourceID, s_EntityType] : s_EntitySceneContext->m_aDynamicBrickEntities) {
            if (m_SelectedDebugEntity.IsAnyParent(s_EntityType)) {
                Logger::Debug("[Editor] Found entity in brick {}.", s_RuntimeResourceID);
                break;
            }
        }
    }

    if (m_SelectedDebugEntity.GetEntity() && m_SelectedDebugEntity.GetEntity()->GetType()) {
        const auto& s_Type = *m_SelectedDebugEntity.GetEntity()->GetType();
        const auto& s_Interfaces = *s_Type.m_pInterfaceData;

        Logger::Trace("[Editor] Hit entity of type '{}' with id '{:x}'.", s_Interfaces[0].m_Type->GetTypeInfo()->pszTypeName, s_Type.m_nEntityID);
    }

    OnSelectEntity(m_SelectedDebugEntity, true, std::nullopt);

    return true;
}

void Editor::RayCastGizmos(const SVector3& p_WorldPosition, const SVector3& p_Direction, RayCastResult& p_Result) {
    if (!m_DrawGizmos) {
        return;
    }

    DirectX::SimpleMath::Ray s_Ray(
        DirectX::SimpleMath::Vector3(p_WorldPosition.x, p_WorldPosition.y, p_WorldPosition.z),
        DirectX::SimpleMath::Vector3(p_Direction.x, p_Direction.y, p_Direction.z)
    );
    float s_ClosestDistance = FLT_MAX;
    ZEntityRef s_HitEntity;
    std::string s_HitEntityTypeName;
    EDebugChannel s_HitEntityDebugChannel{};

    static STypeID* s_SpatialEntityTypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID("ZSpatialEntity");

    {
        std::scoped_lock s_Lock(m_DebugEntitiesMutex);

        for (const auto& [s_EntityRef, s_DebugEntities] : m_EntityRefToDebugEntities) {
            for (const auto& s_DebugEntity : s_DebugEntities) {
                if (!s_DebugEntity->m_HasGizmo) {
                    continue;
                }

                GizmoEntity* s_GizmoEntity = static_cast<GizmoEntity*>(s_DebugEntity.get());

                switch (m_GizmoDrawMode) {
                case DebugDrawMode::SelectedChannelsAndTypes: {
                    if (!m_DebugChannelToGizmoState[s_GizmoEntity->m_DebugChannel]
                        || !m_DebugChannelToTypeNameToGizmoState[s_GizmoEntity->m_DebugChannel][s_GizmoEntity->m_TypeName]) {
                        continue;
                    }

                    break;
                }
                case DebugDrawMode::SelectedEntity: {
                    if (s_GizmoEntity->m_EntityRef != m_SelectedEntity) {
                        continue;
                    }

                    break;
                }
                case DebugDrawMode::All:
                    break;
                }

                ZRenderPrimitiveResource* s_RenderPrimitiveResource =
                    static_cast<ZRenderPrimitiveResource*>(s_GizmoEntity->m_PrimResourcePtr.GetResourceData());

                if (!s_RenderPrimitiveResource) {
                    continue;
                }

                SVector3 s_Center = (s_RenderPrimitiveResource->m_vMin + s_RenderPrimitiveResource->m_vMax) * 0.5f;
                SVector3 s_Extents = (s_RenderPrimitiveResource->m_vMax - s_RenderPrimitiveResource->m_vMin) * 0.5f;

                DirectX::BoundingBox s_Box(
                    DirectX::SimpleMath::Vector3(s_Center.x, s_Center.y, s_Center.z),
                    DirectX::SimpleMath::Vector3(s_Extents.x, s_Extents.y, s_Extents.z)
                );

                SMatrix s_Transform;

                if (s_GizmoEntity->m_TypeName == "ZActBehaviorEntity") {
                    const TEntityRef<ZSpatialEntity> s_MoveToTransform =
                        s_GizmoEntity->m_EntityRef.GetProperty<TEntityRef<ZSpatialEntity>>("m_rMoveToTransform").Get();

                    s_Transform = s_MoveToTransform.m_pInterfaceRef->GetObjectToWorldMatrix() * s_GizmoEntity->m_Transform;
                }
                else {
                    auto s_SpatialEntity = s_GizmoEntity->m_EntityRef.QueryInterface<ZSpatialEntity>(s_SpatialEntityTypeID);

                    s_Transform = s_SpatialEntity->GetObjectToWorldMatrix();
                }

                DirectX::XMMATRIX s_Transform2 = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&s_Transform));

                s_Box.Transform(s_Box, s_Transform2);

                float s_Distance = 0.f;

                if (s_Ray.Intersects(s_Box, s_Distance) && s_Distance < p_Result.m_Distance && s_Distance <= 200.f) {
                    p_Result.m_Distance = s_Distance;
                    p_Result.m_Entity = s_GizmoEntity->m_EntityRef;
                    p_Result.m_TypeName = s_GizmoEntity->m_TypeName;
                    p_Result.m_DebugChannel = s_GizmoEntity->m_DebugChannel;
                }
            }
        }
    }
}

void Editor::RayCastShapes(const SVector3& p_WorldPosition, const SVector3& p_Direction, RayCastResult& p_Result) {
    if (!m_DrawShapes) {
        return;
    }

    DirectX::SimpleMath::Ray s_Ray(
        DirectX::SimpleMath::Vector3(p_WorldPosition.x, p_WorldPosition.y, p_WorldPosition.z),
        DirectX::SimpleMath::Vector3(p_Direction.x, p_Direction.y, p_Direction.z)
    );

    std::scoped_lock s_Lock(m_DebugEntitiesMutex);

    for (const auto& [s_EntityRef, s_DebugEntities] : m_EntityRefToDebugEntities) {
        for (const auto& s_DebugEntity : s_DebugEntities) {
            if (s_DebugEntity->m_HasGizmo) {
                continue;
            }

            switch (m_ShapeDrawMode) {
            case DebugDrawMode::SelectedChannelsAndTypes: {
                if (!m_DebugChannelToShapeState[s_DebugEntity->m_DebugChannel]
                    || !m_DebugChannelToTypeNameToShapeState[s_DebugEntity->m_DebugChannel][s_DebugEntity->m_TypeName]) {
                    continue;
                }

                break;
            }
            case DebugDrawMode::SelectedEntity: {
                if (s_DebugEntity->m_EntityRef != m_SelectedEntity) {
                    continue;
                }

                break;
            }
            case DebugDrawMode::All:
                break;
            }

            float s_Distance = FLT_MAX;
            bool s_Hit = false;

            if (s_Hit && s_Distance < p_Result.m_Distance && s_Distance <= 200.f) {
                p_Result.m_Distance = s_Distance;
                p_Result.m_Entity = s_DebugEntity->m_EntityRef;
                p_Result.m_TypeName = s_DebugEntity->m_TypeName;
                p_Result.m_DebugChannel = s_DebugEntity->m_DebugChannel;
            }
        }
    }
}
