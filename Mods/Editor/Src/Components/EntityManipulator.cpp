#include "Editor.hpp"

#include "Functions.hpp"
#include "Glacier/ZSpatialEntity.hpp"
#include "Glacier/ZPhysics.hpp"
#include "Glacier/ZCamera.hpp"

class ZGeomEntity;

void Editor::DrawEntityAABB(zknt::IDirectXTKRenderer* p_Renderer) {
    if (IsSpecialEntityTreeNode(m_SelectedEntity)) {
        return;
    }

    if (const auto s_SelectedEntity = m_SelectedEntity) {
        if (auto* s_SpatialEntity = s_SelectedEntity.QueryInterface<ZSpatialEntity>()) {
            auto s_Transform = s_SpatialEntity->GetObjectToWorldMatrix();

            float4 s_Min, s_Max;

            // s_SpatialEntity->CalculateBounds(s_Min, s_Max);
            SDK()->Functions()->ZSpatialEntity_CalculateBounds->Call(s_SpatialEntity, s_Min, s_Max);

            p_Renderer->SetFrustumCullingEnabled(false);

            p_Renderer->DrawOBB3D(s_Min, s_Max, s_Transform, SVector4(0.f, 1.f, 1.f, 1.f));

            if (m_EntityHighlightMode == EntityHighlightMode::LinesAndTriangles) {
                p_Renderer->DrawBoundingQuads3D(s_Min, s_Max, s_Transform, SVector4(0.f, 1.f, 1.f, 0.1f));
            }

            p_Renderer->SetFrustumCullingEnabled(true);
        }
    }
}

void Editor::DrawEntityManipulator(bool p_HasFocus) {
    auto s_ImgGuiIO = ImGui::GetIO();

    ImGuizmo::BeginFrame();

    if (p_HasFocus) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !s_ImgGuiIO.WantCaptureMouse) {
            const auto s_MousePos = ImGui::GetMousePos();
            OnMouseDown(SVector2(s_MousePos.x, s_MousePos.y), !m_HoldingMouse);
            m_HoldingMouse = true;
        }
        else {
            m_HoldingMouse = false;
        }

        if (!s_ImgGuiIO.WantTextInput) {
            if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
                if (m_GizmoMode == ImGuizmo::TRANSLATE) {
                    m_GizmoMode = ImGuizmo::ROTATE;
                }
                else if (m_GizmoMode == ImGuizmo::ROTATE) {
                    m_GizmoMode = ImGuizmo::SCALE;
                }
                else if (m_GizmoMode == ImGuizmo::SCALE) {
                    m_GizmoMode = ImGuizmo::TRANSLATE;
                }
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
                m_GizmoSpace = m_GizmoSpace == ImGuizmo::WORLD ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
                OnSelectEntity({}, false, std::nullopt);
            }
        }
    }

    ImGuizmo::Enable(p_HasFocus);

    const auto s_SelectedEntity = m_SelectedEntity;

    if (IsSpecialEntityTreeNode(m_SelectedEntity)) {
        return;
    }

    if (!s_SelectedEntity) {
        return;
    }

    const auto s_CurrentCamera = SDK()->Functions()->GetCurrentCamera->Call();

    if (!s_CurrentCamera) {
        return;
    }

    const auto s_SpatialEntity = s_SelectedEntity.QueryInterface<ZSpatialEntity>();

    if (!s_SpatialEntity) {
        return;
    }

    auto s_ModelMatrix = s_SpatialEntity->GetObjectToWorldMatrix();
    const auto s_ViewMatrix = SDK()->GetViewMatrix();
    const auto s_ProjectionMatrix = SDK()->GetProjectionMatrix();

    ImGuizmo::SetRect(0, 0, s_ImgGuiIO.DisplaySize.x, s_ImgGuiIO.DisplaySize.y);

    if (m_GizmoMode == ImGuizmo::SCALE) {
        const auto s_GeomEntity = s_SelectedEntity.QueryInterface<ZGeomEntity>();

        if (s_GeomEntity) {
            ZVariant<SVector3> s_Scale = m_SelectedEntity.GetProperty<SVector3>("m_PrimitiveScale");

            s_ModelMatrix.ScaleTransform(s_Scale.Get());

            float s_ScaleSnapValue = static_cast<float>(m_ScaleSnapValue);
            if (ImGuizmo::Manipulate(
                    &s_ViewMatrix.XAxis.x, &s_ProjectionMatrix.XAxis.x, m_GizmoMode, m_GizmoSpace, &s_ModelMatrix.XAxis.x, NULL,
                    m_UseScaleSnap ? &s_ScaleSnapValue : NULL
                )) {
                m_SelectedEntity.SetProperty<SVector3>("m_PrimitiveScale", s_ModelMatrix.GetScale());

                const bool s_bRemovePhysics = m_SelectedEntity.GetProperty<bool>("m_bRemovePhysics").Get();

                if (!s_bRemovePhysics) {
                    m_SelectedEntity.SetProperty<bool>("m_bRemovePhysics", true);
                    m_SelectedEntity.SetProperty<bool>("m_bRemovePhysics", false);
                }
            }
        }
    }
    else if (m_GizmoMode == ImGuizmo::TRANSLATE) {
        float m_CombinedSnapValue[3] = {static_cast<float>(m_SnapValue), static_cast<float>(m_SnapValue), static_cast<float>(m_SnapValue)};

        if (ImGuizmo::Manipulate(
                &s_ViewMatrix.XAxis.x, &s_ProjectionMatrix.XAxis.x, m_GizmoMode, m_GizmoSpace, &s_ModelMatrix.XAxis.x, NULL,
                m_UseSnap ? m_CombinedSnapValue : NULL
            )) {
            OnEntityTransformChange(s_SelectedEntity, s_ModelMatrix, false, std::nullopt);
        }
    }
    else if (m_GizmoMode == ImGuizmo::ROTATE) {
        float s_AngleSnapValue = static_cast<float>(m_AngleSnapValue);

        if (ImGuizmo::Manipulate(
                &s_ViewMatrix.XAxis.x, &s_ProjectionMatrix.XAxis.x, m_GizmoMode, m_GizmoSpace, &s_ModelMatrix.XAxis.x, NULL,
                m_UseAngleSnap ? &s_AngleSnapValue : NULL
            )) {
            OnEntityTransformChange(s_SelectedEntity, s_ModelMatrix, false, std::nullopt);
        }
    }
}

void Editor::OnEntityTransformChange(ZEntityRef p_Entity, SMatrix p_Transform, bool p_Relative, std::optional<std::string> p_ClientId) {
    if (auto* s_SpatialEntity = p_Entity.QueryInterface<ZSpatialEntity>()) {
        if (!p_Relative) {
            s_SpatialEntity->SetObjectToWorldMatrixFromEditor(p_Transform);
        }
        else {
            SMatrix s_ParentTrans;

            // Get parent entity transform.
            if (s_SpatialEntity->m_eidParent.m_pInterfaceRef) {
                s_ParentTrans = s_SpatialEntity->m_eidParent.m_pInterfaceRef->GetObjectToWorldMatrix();
            }
            else if (p_Entity.GetLogicalParent() && p_Entity.GetLogicalParent().QueryInterface<ZSpatialEntity>()) {
                s_ParentTrans = p_Entity.GetLogicalParent().QueryInterface<ZSpatialEntity>()->GetObjectToWorldMatrix();
            }
            else if (p_Entity.GetOwningEntity() && p_Entity.GetOwningEntity().QueryInterface<ZSpatialEntity>()) {
                s_ParentTrans = p_Entity.GetOwningEntity().QueryInterface<ZSpatialEntity>()->GetObjectToWorldMatrix();
            }

            // Calculate world transform based on the provided relative transform.
            auto s_WorldTrans = p_Transform * s_ParentTrans;
            s_WorldTrans.Trans = s_ParentTrans.Trans + p_Transform.Trans;
            s_WorldTrans.Trans.w = 1.f;

            s_SpatialEntity->SetObjectToWorldMatrixFromEditor(s_WorldTrans);
        }

        if (const auto s_PhysicsAspect = p_Entity.QueryInterface<ZStaticPhysicsAspect>()) {
            if (s_PhysicsAspect->m_pPhysicsObject) {
                s_PhysicsAspect->m_pPhysicsObject->SetTransform(s_SpatialEntity->GetObjectToWorldMatrix());
            }
        }

        m_Server.OnEntityTransformChanged(p_Entity, std::move(p_ClientId));
    }
}
