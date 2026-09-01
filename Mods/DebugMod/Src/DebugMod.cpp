#include "DebugMod.hpp"

#include <imgui_internal.h>

#include "IconsMaterialDesign.h"

#include <Glacier/ZCamera.hpp>
#include <Glacier/ZGameLoopManager.hpp>

#include <Logging.hpp>

void DebugMod::Init() {
    SDK()->Hooks()->ZEntitySceneContext_ClearScene->AddDetour(this, &DebugMod::OnClearScene);
    SDK()->Hooks()->ZPFObstacleEntity_UpdateObstacle->AddDetour(this, &DebugMod::ZPFObstacleEntity_UpdateObstacle);
    SDK()->Hooks()->ZPathfinder_AddLoadedNavMesh->AddDetour(this, &DebugMod::ZPathfinder_AddLoadedNavMesh);
    SDK()->Hooks()->ZPathfinder_RemoveLoadedNavMesh->AddDetour(this, &DebugMod::ZPathfinder_RemoveLoadedNavMesh);
}

void DebugMod::OnDrawMenu(zknt::IImGuiRenderer* p_Renderer) {
    if (ImGui::Button(ICON_MD_BUILD " DEBUG")) {
        m_ShowWindow = !m_ShowWindow;
    }
}

void DebugMod::OnDrawUI(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) {
    if (!m_ShowWindow || !p_HasFocus) {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    const auto s_IsWindowExpanded = ImGui::Begin(ICON_MD_BUILD " Debug", &m_ShowWindow);
    ImGui::PushFont(p_Renderer->GetRegularFont());

    if (s_IsWindowExpanded) {
        if (ImGui::CollapsingHeader("Nav meshes", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Draw nav meshes", &m_DrawNavMeshes)) {
                if (m_NavMeshRuntimeResourceIDToNavMeshData.empty()) {
                    BuildAllNavMeshRenderData();
                }
            }

            ImGui::Indent();

            {
                std::shared_lock lock(m_NavMeshMutex);

                for (auto& [s_NavPowerResourceID, s_NavMeshData] : m_NavMeshRuntimeResourceIDToNavMeshData) {
                    ImGui::Checkbox(
                        fmt::format("{:08X}{:08X}", s_NavPowerResourceID.m_IDHigh, s_NavPowerResourceID.m_IDLow).c_str(), &s_NavMeshData.m_Draw
                    );

                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip(fmt::format("{} ({:016x})", s_NavMeshData.m_EntityName, s_NavMeshData.m_EntityID).c_str());
                    }

                    if (ImGuiCopyWidget("RuntimeResourceID")) {
                        CopyToClipboard(fmt::format("{:016x}", s_NavPowerResourceID.GetID()));
                    }
                }
            }

            ImGui::Unindent();

            ImGui::Spacing();

            ImGui::Checkbox("Draw planner areas", &m_DrawPlannerAreas);
            ImGui::Checkbox("Draw planner areas solid", &m_DrawPlannerAreasSolid);
            ImGui::Checkbox("Colorize area usage flags", &m_ColorizeAreaUsageFlags);
            ImGui::Checkbox("Draw obstacles", &m_DrawObstacles);
            ImGui::Checkbox("Draw planner connectivity", &m_DrawDrawPlannerConnectivity);
            ImGui::Checkbox("Draw area penalty multipliers", &m_DrawAreaPenaltyMults);
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void DebugMod::OnDepthDraw3D(zknt::IDirectXTKRenderer* p_Renderer) {
    if (m_DrawNavMeshes) {
        DrawNavMeshes(p_Renderer);
    }

    if (m_DrawObstacles) {
        DrawObstacles(p_Renderer);
    }
}

void DebugMod::DrawNavMeshes(zknt::IDirectXTKRenderer* p_Renderer) {
    static const SVector4 s_GreenTriangleColor = SVector4(0.19608f, 0.80392f, 0.19608f, 0.49804f);
    static const SVector4 s_YellowTriangleColor = SVector4(1.f, 1.f, 0.f, 0.49804f);

    std::shared_lock lock(m_NavMeshMutex);

    if (m_DrawPlannerAreasSolid) {
        for (const auto& [s_NavPowerResourceID, s_NavMeshData] : m_NavMeshRuntimeResourceIDToNavMeshData) {
            if (!s_NavMeshData.m_Draw) {
                continue;
            }

            for (size_t i = 0; i < s_NavMeshData.m_Areas.size(); ++i) {
                if (m_ColorizeAreaUsageFlags && s_NavMeshData.m_Areas[i]->m_area->m_usageFlags == NavPower::AreaUsageFlags::AREA_STEPS) {
                    p_Renderer->DrawMesh(s_NavMeshData.m_Vertices[i], s_NavMeshData.m_Indices[i], s_YellowTriangleColor);
                }
                else {
                    p_Renderer->DrawMesh(s_NavMeshData.m_Vertices[i], s_NavMeshData.m_Indices[i], s_GreenTriangleColor);
                }
            }
        }
    }

    if (m_DrawPlannerAreas) {
        for (const auto& [s_NavPowerResourceID, s_NavMeshData] : m_NavMeshRuntimeResourceIDToNavMeshData) {
            if (!s_NavMeshData.m_Draw) {
                continue;
            }

            for (size_t i = 0; i < s_NavMeshData.m_NavMeshLines.size(); ++i) {
                p_Renderer->DrawLine3D(
                    s_NavMeshData.m_NavMeshLines[i].m_Start, s_NavMeshData.m_NavMeshLines[i].m_StartColor, s_NavMeshData.m_NavMeshLines[i].m_End,
                    s_NavMeshData.m_NavMeshLines[i].m_EndColor
                );
            }
        }
    }

    if (m_DrawDrawPlannerConnectivity) {
        for (const auto& [s_NavPowerResourceID, s_NavMeshData] : m_NavMeshRuntimeResourceIDToNavMeshData) {
            if (!s_NavMeshData.m_Draw) {
                continue;
            }

            for (size_t i = 0; i < s_NavMeshData.m_NavMeshConnectivityLines.size(); ++i) {
                p_Renderer->DrawLine3D(
                    s_NavMeshData.m_NavMeshConnectivityLines[i].m_Start, s_NavMeshData.m_NavMeshConnectivityLines[i].m_StartColor,
                    s_NavMeshData.m_NavMeshConnectivityLines[i].m_End, s_NavMeshData.m_NavMeshConnectivityLines[i].m_EndColor
                );
            }
        }
    }

    if (m_DrawAreaPenaltyMults) {
        const auto s_CurrentCamera = SDK()->Functions()->GetCurrentCamera->Call();

        if (!s_CurrentCamera) {
            return;
        }

        SMatrix s_WorldMatrix = s_CurrentCamera->GetObjectToWorldMatrix();

        static const SVector4 s_Color = SVector4(1.f, 1.f, 1.f, 1.f);
        static const float s_Scale = 0.2f;

        for (const auto& [s_NavPowerResourceID, s_NavMeshData] : m_NavMeshRuntimeResourceIDToNavMeshData) {
            if (!s_NavMeshData.m_Draw) {
                continue;
            }

            for (size_t i = 0; i < s_NavMeshData.m_Areas.size(); ++i) {
                SVector3 s_WorldPosition = s_NavMeshData.m_Transform.WVectorTransform(s_NavMeshData.m_Areas[i]->m_area->m_pos);

                const DirectX::XMVECTOR s_WorldPosition2 = DirectX::XMVectorSet(s_WorldPosition.x, s_WorldPosition.y, s_WorldPosition.z, 1.0f);

                s_WorldPosition.z += 2.f;
                s_WorldMatrix.Trans = float4(s_WorldPosition.x, s_WorldPosition.y, s_WorldPosition.z, 1.0f);

                std::string s_Text;

                if (!s_NavMeshData.m_Areas[i]->m_area->m_flags.IsImpassable()
                    || s_NavMeshData.m_Areas[i]->m_area->m_flags.ApplyObCostWhenFlagsDontMatch()) {
                    const uint32_t obCostMult = s_NavMeshData.m_Areas[i]->m_area->m_flags.GetObCostMult();
                    const uint32_t staticCostMult = s_NavMeshData.m_Areas[i]->m_area->m_flags.GetStaticCostMult();
                    const uint32_t costMult = obCostMult > staticCostMult ? obCostMult : staticCostMult;

                    s_Text = std::to_string(costMult);
                }
                else {
                    s_Text = "---";
                }

                p_Renderer->DrawText3D(s_Text.c_str(), s_WorldMatrix, s_Color, s_Scale);
            }
        }
    }
}

void DebugMod::DrawObstacles(zknt::IDirectXTKRenderer* p_Renderer) {
    ZPFObstacleManager* s_ObstacleManager = static_cast<ZPFObstacleManager*>(SDK()->Globals()->Pathfinder->m_obstacleManager);

    for (const auto& obstacleHandle : s_ObstacleManager->m_obstacles) {
        const auto s_PFObstacleInternal = static_cast<const ZPFObstacleInternal*>(obstacleHandle.m_internal.GetTarget());

        const SVector4 s_Color = SVector4(1.f, 1.f, 0.f, 0.29804f);
        const SMatrix s_Transform = s_PFObstacleInternal->m_transform;
        const float4 s_HalfSize = s_PFObstacleInternal->m_halfSize;
        const SVector3 s_MinBound = -s_HalfSize;
        const SVector3 s_MaxBound = s_HalfSize;

        p_Renderer->DrawBoundingQuads3D(s_MinBound, s_MaxBound, s_Transform, s_Color);
    }

    for (const auto& obstacleHandle : s_ObstacleManager->m_obstacles) {
        const auto s_PFObstacleInternal = static_cast<const ZPFObstacleInternal*>(obstacleHandle.m_internal.GetTarget());

        const SVector4 s_Color = SVector4(1.f, 1.f, 0.f, 1.f);
        const SMatrix s_Transform = s_PFObstacleInternal->m_transform;
        const float4 s_HalfSize = s_PFObstacleInternal->m_halfSize;

        const SVector3 s_MinBound = SVector3(-s_HalfSize.x, -s_HalfSize.y, -s_HalfSize.z);
        const SVector3 s_MaxBound = SVector3(s_HalfSize.x, s_HalfSize.y, s_HalfSize.z);

        p_Renderer->DrawOBB3D(s_MinBound, s_MaxBound, s_Transform, s_Color);
    }

    const auto s_CurrentCamera = SDK()->Functions()->GetCurrentCamera->Call();

    if (!s_CurrentCamera) {
        return;
    }

    SMatrix s_WorldMatrix = s_CurrentCamera->GetObjectToWorldMatrix();

    static const SVector4 s_Color = SVector4(1.f, 1.f, 1.f, 1.f);
    static const float s_Scale = 0.1f;

    for (auto& obstacleHandle : s_ObstacleManager->m_obstacles) {
        auto s_PFObstacleInternal = static_cast<ZPFObstacleInternal*>(obstacleHandle.m_internal.GetTarget());

        const SMatrix s_Transform = s_PFObstacleInternal->m_transform;
        const float4 s_HalfSize = s_PFObstacleInternal->m_halfSize;

        float4 s_TopCenter = s_Transform.Trans + s_Transform.ZAxis * (s_HalfSize.z + 0.5f);
        s_TopCenter.z += 2.0f;

        s_WorldMatrix.Trans = s_TopCenter;

        std::string s_Text;

        if (const auto it = m_ObstacleToEntityID.find(s_PFObstacleInternal); it != m_ObstacleToEntityID.end()) {
            s_Text += fmt::format("Entity ID: {:016X}\n", it->second);
        }

        s_Text += fmt::format(
            "Behavior: {}\nPenalty multiplier: {}\nPenalty addend: {}\nBlocker flags: 0x{:04X}\nAffected nav layer flags: "
            "0x{:04X}",
            ObstacleBehaviorToString(s_PFObstacleInternal->m_Behavior), s_PFObstacleInternal->m_fPenaltyMultiplier,
            s_PFObstacleInternal->m_fPenaltyAddend, s_PFObstacleInternal->m_ObstacleBlockerFlags, s_PFObstacleInternal->m_affectedNavLayerFlags
        );

        p_Renderer->DrawText3D(s_Text.c_str(), s_WorldMatrix, s_Color, s_Scale);
    }
}

void DebugMod::BuildAllNavMeshRenderData() {
    for (const auto& s_LoadedNavMesh : SDK()->Globals()->Pathfinder->m_aLoadedNavMeshes) {
        if (!s_LoadedNavMesh.m_pPathfinderConfiguration) {
            continue;
        }

        const ZRuntimeResourceID s_NavPowerResourceID = s_LoadedNavMesh.m_pPathfinderConfiguration->m_NavpowerResourceID.GetResourceInfo().rid;

        BuildNavMeshRenderData(
            s_NavPowerResourceID, s_LoadedNavMesh.m_Transform, s_LoadedNavMesh.m_pPathfinderConfiguration, s_LoadedNavMesh.m_pNavpowerResource,
            s_LoadedNavMesh.m_nNavpowerResourceSize
        );
    }
}

void DebugMod::BuildNavMeshRenderData(
    const ZRuntimeResourceID& p_NavPowerResourceID, const SMatrix& p_Transform, ZPathfinderConfiguration* p_PathfinderConfiguration,
    void* p_NavpowerResource, uint64_t p_NavpowerResourceSize
) {
    static const SVector4 s_LineColor = SVector4(0.f, 1.f, 0.f, 1.f);
    static const SVector4 s_AdjacentLineColor = SVector4(1.f, 1.f, 1.f, 1.f);

    NavMeshData s_NavMeshData;

    s_NavMeshData.m_Transform = p_Transform;

    ZEntityRef s_EntityRef;
    p_PathfinderConfiguration->GetID(s_EntityRef);

    s_NavMeshData.m_EntityID = s_EntityRef->GetType()->m_nEntityID;

    ZString s_EntityName;
    SDK()->GetEntityName(s_EntityRef, s_EntityName);

    s_NavMeshData.m_EntityName = s_EntityName.c_str();

    s_NavMeshData.m_NavpData.resize(p_NavpowerResourceSize);

    std::memcpy(s_NavMeshData.m_NavpData.data(), reinterpret_cast<void*>(p_NavpowerResource), p_NavpowerResourceSize);

    s_NavMeshData.m_NavMesh.read(reinterpret_cast<uintptr_t>(s_NavMeshData.m_NavpData.data()), p_NavpowerResourceSize);

    for (auto& section : s_NavMeshData.m_NavMesh.m_aSections) {
        for (auto& graph : section.m_aNavGraphs) {
            for (auto& area : graph.m_areas) {
                s_NavMeshData.m_Areas.push_back(&area);
            }
        }
    }

    s_NavMeshData.m_Vertices.resize(s_NavMeshData.m_Areas.size());
    s_NavMeshData.m_Indices.resize(s_NavMeshData.m_Areas.size());
    s_NavMeshData.m_NavMeshLines.reserve(s_NavMeshData.m_Areas.size() * 3);
    s_NavMeshData.m_NavMeshConnectivityLines.reserve(s_NavMeshData.m_Areas.size() * 3);

    std::map<NavPower::Binary::Area*, uint32_t> s_AreaPointerToIndexMap = GetAreaPointerToIndexMap(s_NavMeshData);

    for (size_t i = 0; i < s_NavMeshData.m_Areas.size(); ++i) {
        const size_t s_VertexCount = s_NavMeshData.m_Areas[i]->m_edges.size();

        s_NavMeshData.m_Vertices[i].reserve(s_VertexCount);

        const SVector3 s_Centroid = s_NavMeshData.m_Areas[i]->CalculateCentroid();

        for (size_t j = 0; j < s_VertexCount; ++j) {
            s_NavMeshData.m_Vertices[i].push_back(s_NavMeshData.m_Transform.WVectorTransform(s_NavMeshData.m_Areas[i]->m_edges[j]->m_pos));

            const size_t s_NextIndex = (j + 1) % s_VertexCount;
            zknt::Line& s_Line = s_NavMeshData.m_NavMeshLines.emplace_back();

            s_Line.m_Start = s_NavMeshData.m_Transform.WVectorTransform(s_NavMeshData.m_Areas[i]->m_edges[j]->m_pos);
            s_Line.m_StartColor = s_LineColor;

            s_Line.m_End = s_NavMeshData.m_Transform.WVectorTransform(s_NavMeshData.m_Areas[i]->m_edges[s_NextIndex]->m_pos);
            s_Line.m_EndColor = s_LineColor;

            NavPower::Binary::Area* s_AdjArea = s_NavMeshData.m_Areas[i]->m_edges[j]->m_pAdjArea;

            if (s_AdjArea) {
                const uint32_t s_AdjacentAreaIndex = s_AreaPointerToIndexMap[s_AdjArea];
                NavPower::Area& s_AdjacentArea = *s_NavMeshData.m_Areas[s_AdjacentAreaIndex - 1];
                const SVector3 s_AdjacentCentroid = s_AdjacentArea.CalculateCentroid();

                zknt::Line& s_ConnLine = s_NavMeshData.m_NavMeshConnectivityLines.emplace_back();
                s_ConnLine.m_Start = s_Centroid;
                s_ConnLine.m_StartColor = s_AdjacentLineColor;
                s_ConnLine.m_End = s_AdjacentCentroid;
                s_ConnLine.m_EndColor = s_AdjacentLineColor;
            }
        }

        VertexTriangluation(s_NavMeshData.m_Vertices[i], s_NavMeshData.m_Indices[i]);
    }

    std::unique_lock lock(m_NavMeshMutex);

    m_NavMeshRuntimeResourceIDToNavMeshData[p_NavPowerResourceID] = std::move(s_NavMeshData);
}

std::map<NavPower::Binary::Area*, uint32_t> DebugMod::GetAreaPointerToIndexMap(NavMeshData& p_NavMeshData) {
    std::map<NavPower::Binary::Area*, uint32_t> s_AreaPointerToIndexMap;
    uint32_t s_AreaIndex = 1;

    for (NavPower::Area* area : p_NavMeshData.m_Areas) {
        s_AreaPointerToIndexMap.emplace(area->m_area, s_AreaIndex);

        s_AreaIndex++;
    }

    return s_AreaPointerToIndexMap;
}

float DebugMod::AngleBetween(const SVector3& a, const SVector3& b) {
    float angle = SVector3::DotProduct(a, b);
    angle /= (a.Length() * b.Length());
    return angle = acosf(angle);
}

SVector3 DebugMod::ProjectionOnto(const SVector3& a, const SVector3& b) {
    const SVector3 bn = b / b.Length();
    return bn * SVector3::DotProduct(a, bn);
}

bool DebugMod::AreOnSameSide(const SVector3& p1, const SVector3& p2, const SVector3& a, const SVector3& b) {
    const SVector3 cp1 = SVector3::CrossProduct(b - a, p1 - a);
    const SVector3 cp2 = SVector3::CrossProduct(b - a, p2 - a);

    if (SVector3::DotProduct(cp1, cp2) >= 0) {
        return true;
    }

    return false;
}

SVector3 DebugMod::ComputeTriangleNormal(const SVector3& t1, const SVector3& t2, const SVector3& t3) {
    const SVector3 u = t2 - t1;
    const SVector3 v = t3 - t1;
    const SVector3 normal = SVector3::CrossProduct(u, v);

    return normal;
}

bool DebugMod::IsInTriangle(const SVector3& point, const SVector3& triangle1, const SVector3& triangle2, const SVector3& triangle3) {
    // Test to see if it is within an infinite prism that the triangle outlines.
    const bool within_tri_prisim = AreOnSameSide(point, triangle1, triangle2, triangle3) && AreOnSameSide(point, triangle2, triangle1, triangle3)
                                   && AreOnSameSide(point, triangle3, triangle1, triangle2);

    // If it isn't it will never be on the triangle
    if (!within_tri_prisim) {
        return false;
    }

    // Calulate Triangle's Normal
    const SVector3 n = ComputeTriangleNormal(triangle1, triangle2, triangle3);

    // Project the point onto this normal
    const SVector3 proj = ProjectionOnto(point, n);

    // If the distance from the triangle to the point is 0
    //	it lies on the triangle
    if (proj.Length() == 0) {
        return true;
    }

    return false;
}

const char* DebugMod::ObstacleBehaviorToString(EPFObstacleBehavior p_Type) {
    switch (p_Type) {
    case EPFObstacleBehavior::ePFOB_Legacy:
        return "Legacy";
    case EPFObstacleBehavior::ePFOB_Penalty:
        return "Penalty";
    case EPFObstacleBehavior::ePFOB_Impassible:
        return "Impassible";
    case EPFObstacleBehavior::ePFOB_ImpassibleOrPenalty:
        return "Impassible or penalty";
    default:
        return "";
    }
}

void DebugMod::VertexTriangluation(const std::vector<SVector3>& vertices, std::vector<unsigned short>& indices) {
    // If there are 2 or less verts,
    // no triangle can be created,
    // so exit
    if (vertices.size() < 3) {
        return;
    }
    // If it is a triangle no need to calculate it
    if (vertices.size() == 3) {
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        return;
    }

    // Create a list of vertices
    std::vector<SVector3> tVerts = vertices;

    while (true) {
        // For every vertex
        for (int i = 0; i < int(tVerts.size()); i++) {
            // pPrev = the previous vertex in the list
            SVector3 pPrev;
            if (i == 0) {
                pPrev = tVerts[tVerts.size() - 1];
            }
            else {
                pPrev = tVerts[i - 1];
            }

            // pCur = the current vertex;
            SVector3 pCur = tVerts[i];

            // pNext = the next vertex in the list
            SVector3 pNext;
            if (i == tVerts.size() - 1) {
                pNext = tVerts[0];
            }
            else {
                pNext = tVerts[i + 1];
            }

            // Check to see if there are only 3 verts left
            // if so this is the last triangle
            if (tVerts.size() == 3) {
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(tVerts.size()); j++) {
                    if (vertices[j] == pCur) {
                        indices.push_back(j);
                    }
                    if (vertices[j] == pPrev) {
                        indices.push_back(j);
                    }
                    if (vertices[j] == pNext) {
                        indices.push_back(j);
                    }
                }

                tVerts.clear();
                break;
            }
            if (tVerts.size() == 4) {
                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(vertices.size()); j++) {
                    if (vertices[j] == pCur) {
                        indices.push_back(j);
                    }
                    if (vertices[j] == pPrev) {
                        indices.push_back(j);
                    }
                    if (vertices[j] == pNext) {
                        indices.push_back(j);
                    }
                }

                SVector3 tempVec;
                for (int j = 0; j < int(tVerts.size()); j++) {
                    if (tVerts[j] != pCur && tVerts[j] != pPrev && tVerts[j] != pNext) {
                        tempVec = tVerts[j];
                        break;
                    }
                }

                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(vertices.size()); j++) {
                    if (vertices[j] == pPrev) {
                        indices.push_back(j);
                    }
                    if (vertices[j] == pNext) {
                        indices.push_back(j);
                    }
                    if (vertices[j] == tempVec) {
                        indices.push_back(j);
                    }
                }

                tVerts.clear();
                break;
            }

            // If Vertex is not an interior vertex
            float angle = AngleBetween(pPrev - pCur, pNext - pCur) * (180 / 3.14159265359);
            if (angle <= 0 && angle >= 180) {
                continue;
            }

            // If any vertices are within this triangle
            bool inTri = false;
            for (int j = 0; j < int(vertices.size()); j++) {
                if (IsInTriangle(vertices[j], pPrev, pCur, pNext) && vertices[j] != pPrev && vertices[j] != pCur && vertices[j] != pNext) {
                    inTri = true;
                    break;
                }
            }
            if (inTri) {
                continue;
            }

            // Create a triangle from pCur, pPrev, pNext
            for (int j = 0; j < int(vertices.size()); j++) {
                if (vertices[j] == pCur) {
                    indices.push_back(j);
                }
                if (vertices[j] == pPrev) {
                    indices.push_back(j);
                }
                if (vertices[j] == pNext) {
                    indices.push_back(j);
                }
            }

            // Delete pCur from the list
            for (int j = 0; j < int(tVerts.size()); j++) {
                if (tVerts[j] == pCur) {
                    tVerts.erase(tVerts.begin() + j);
                    break;
                }
            }

            // reset i to the start
            // -1 since loop will add 1 to it
            i = -1;
        }

        // if no triangles were created
        if (indices.size() == 0) {
            break;
        }

        // if no more vertices
        if (tVerts.size() == 0) {
            break;
        }
    }
}

void DebugMod::CopyToClipboard(const std::string& p_String) {
    if (!OpenClipboard(nullptr)) {
        return;
    }

    EmptyClipboard();

    const auto s_GlobalData = GlobalAlloc(GMEM_MOVEABLE, p_String.size() + 1);

    if (!s_GlobalData) {
        CloseClipboard();
        return;
    }

    const auto s_GlobalDataPtr = GlobalLock(s_GlobalData);

    if (!s_GlobalDataPtr) {
        CloseClipboard();
        GlobalFree(s_GlobalData);
        return;
    }

    memset(s_GlobalDataPtr, 0, p_String.size() + 1);
    memcpy(s_GlobalDataPtr, p_String.c_str(), p_String.size());

    GlobalUnlock(s_GlobalData);

    SetClipboardData(CF_TEXT, s_GlobalData);
    CloseClipboard();
}

bool DebugMod::ImGuiCopyWidget(const std::string& p_Id) {
    ImGui::SameLine(0, 10.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.5, 0.5});
    ImGui::SetWindowFontScale(0.6);

    const auto s_Result = ImGui::ButtonEx(
        (std::string(ICON_MD_CONTENT_COPY) + "##" + p_Id).c_str(), {m_CopyWidgetButtonSize, m_CopyWidgetButtonSize},
        ImGuiButtonFlags_AlignTextBaseLine
    );

    ImGui::SetWindowFontScale(1.0);
    ImGui::PopStyleVar(2);

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Copy to clipboard");
    }

    return s_Result;
}

DEFINE_PLUGIN_DETOUR(DebugMod, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene) {
    m_DrawNavMeshes = false;
    m_DrawObstacles = false;
    m_NavMeshRuntimeResourceIDToNavMeshData.clear();
    m_ObstacleToEntityID.clear();

    return HookResult<void>(HookAction::Continue());
}

DEFINE_PLUGIN_DETOUR(DebugMod, void, ZPFObstacleEntity_UpdateObstacle, ZPFObstacleEntity* th, bool forceUpdate) {
    p_Hook->CallOriginal(th, forceUpdate);

    auto s_ObstacleIt = SDK()->Globals()->Pathfinder->m_ObstacleIDToObstacleInternal.find(th->m_ObstacleID);

    if (s_ObstacleIt != SDK()->Globals()->Pathfinder->m_ObstacleIDToObstacleInternal.end()) {
        m_ObstacleToEntityID[s_ObstacleIt->second] = th->GetType()->m_nEntityID;
    }

    return {HookAction::Return()};
}

DEFINE_PLUGIN_DETOUR(DebugMod, void, ZPathfinder_AddLoadedNavMesh, ZPathfinder* th, const SPendingLoadedNavMesh& pendingLoadedNavMesh) {
    if (!m_DrawNavMeshes && m_NavMeshRuntimeResourceIDToNavMeshData.empty()) {
        return {HookAction::Continue()};
    }

    const ZRuntimeResourceID s_NavPowerResourceID = pendingLoadedNavMesh.m_pPathfinderConfiguration->m_NavpowerResourceID.GetResourceInfo().rid;

    BuildNavMeshRenderData(
        s_NavPowerResourceID, pendingLoadedNavMesh.m_Transform, pendingLoadedNavMesh.m_pPathfinderConfiguration,
        pendingLoadedNavMesh.m_pNavpowerResource, pendingLoadedNavMesh.m_nNavpowerResourceSize
    );

    return {HookAction::Continue()};
}

DEFINE_PLUGIN_DETOUR(DebugMod, bool, ZPathfinder_RemoveLoadedNavMesh, ZPathfinder* th, ZPathfinderConfiguration* pathfinderConfiguration) {
    const ZRuntimeResourceID s_NavPowerResourceID = pathfinderConfiguration->m_NavpowerResourceID.GetResourceInfo().rid;

    std::unique_lock lock(m_NavMeshMutex);

    m_NavMeshRuntimeResourceIDToNavMeshData.erase(s_NavPowerResourceID);

    return {HookAction::Continue()};
}

DEFINE_ZKNT_PLUGIN(DebugMod)
