#pragma once

#include <shared_mutex>

#include <IPluginInterface.hpp>

#include <NavPower.hpp>

#include <Glacier/ZResourceID.hpp>
#include <Glacier/ZPathfinder.hpp>

class DebugMod : public zknt::IPluginInterface {
  public:
    void Init() override;
    void OnDrawMenu(zknt::IImGuiRenderer* p_Renderer) override;
    void OnDrawUI(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) override;
    void OnDepthDraw3D(zknt::IDirectXTKRenderer* p_Renderer) override;

  private:
    struct NavMeshData {
        NavPower::NavMesh m_NavMesh;
        std::vector<uint8_t> m_NavpData;
        std::vector<NavPower::Area*> m_Areas;
        std::vector<std::vector<SVector3>> m_Vertices;
        std::vector<std::vector<uint16_t>> m_Indices;
        std::vector<zknt::Line> m_NavMeshLines;
        std::vector<zknt::Line> m_NavMeshConnectivityLines;
        SMatrix m_Transform;
        uint64_t m_EntityID;
        std::string m_EntityName;
        bool m_Draw = true;
    };

    void DrawNavMeshes(zknt::IDirectXTKRenderer* p_Renderer);
    void DrawObstacles(zknt::IDirectXTKRenderer* p_Renderer);

    void BuildAllNavMeshRenderData();
    void BuildNavMeshRenderData(
        const ZRuntimeResourceID& p_NavPowerResourceID, const SMatrix& p_Transform, ZPathfinderConfiguration* p_PathfinderConfiguration,
        void* p_NavpowerResource, uint64_t p_NavpowerResourceSize
    );

    std::map<NavPower::Binary::Area*, uint32_t> GetAreaPointerToIndexMap(NavMeshData& p_NavMeshData);

    // Functions are adapted from OBJ Loader plugin: https://github.com/Bly7/OBJ-Loader/blob/master/Source/OBJ_Loader.h
    static void VertexTriangluation(const std::vector<SVector3>& vertices, std::vector<unsigned short>& indices);
    static float AngleBetween(const SVector3& a, const SVector3& b);
    static SVector3 ProjectionOnto(const SVector3& a, const SVector3& b);
    static bool AreOnSameSide(const SVector3& p1, const SVector3& p2, const SVector3& a, const SVector3& b);
    static SVector3 ComputeTriangleNormal(const SVector3& t1, const SVector3& t2, const SVector3& t3);
    static bool IsInTriangle(const SVector3& point, const SVector3& triangle1, const SVector3& triangle2, const SVector3& triangle3);

    static const char* ObstacleBehaviorToString(EPFObstacleBehavior p_Type);

    static void CopyToClipboard(const std::string& p_String);
    static bool ImGuiCopyWidget(const std::string& p_Id);

    DECLARE_PLUGIN_DETOUR(DebugMod, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene);

    DECLARE_PLUGIN_DETOUR(DebugMod, void, ZPFObstacleEntity_UpdateObstacle, ZPFObstacleEntity* th, bool forceUpdate);

    DECLARE_PLUGIN_DETOUR(DebugMod, void, ZPathfinder_AddLoadedNavMesh, ZPathfinder* th, const SPendingLoadedNavMesh& pendingLoadedNavMesh);
    DECLARE_PLUGIN_DETOUR(DebugMod, bool, ZPathfinder_RemoveLoadedNavMesh, ZPathfinder* th, ZPathfinderConfiguration* pathfinderConfiguration);

    bool m_ShowWindow = false;

    bool m_DrawNavMeshes = false;
    bool m_DrawPlannerAreas = true;
    bool m_DrawPlannerAreasSolid = true;
    bool m_ColorizeAreaUsageFlags = true;
    bool m_DrawObstacles = false;
    bool m_DrawDrawPlannerConnectivity = false;
    bool m_DrawAreaPenaltyMults = false;

    mutable std::shared_mutex m_NavMeshMutex;
    std::unordered_map<ZRuntimeResourceID, NavMeshData> m_NavMeshRuntimeResourceIDToNavMeshData;
    std::unordered_map<IPFObstacleInternal*, uint64_t> m_ObstacleToEntityID;

    static constexpr float m_CopyWidgetButtonSize = 20.f;
    static constexpr float m_CopyWidgetSpacing = 10.f;
    static constexpr float m_CopyWidgetWidth = m_CopyWidgetButtonSize + m_CopyWidgetSpacing;
};

DECLARE_ZKNT_PLUGIN(DebugMod)
