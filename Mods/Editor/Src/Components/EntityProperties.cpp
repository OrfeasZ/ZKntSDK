#include <Editor.hpp>

#include "IconsMaterialDesign.h"

#include <ResourceLib_KNT.h>

#include <Glacier/ZModule.hpp>
#include <Glacier/ZRender.hpp>
#include <Glacier/ZPlayer.hpp>
#include <Glacier/SColorRGB.hpp>
#include <Glacier/SColorRGBA.hpp>
#include <Glacier/ZGameTime.hpp>
#include <Glacier/ZKeyword.hpp>

#include "Util/ImGuiUtils.hpp"

void Editor::DrawEntityPropertiesWindow(zknt::IImGuiRenderer* p_Renderer) {
    auto s_ImgGuiIO = ImGui::GetIO();

    ImGui::SetNextWindowPos({s_ImgGuiIO.DisplaySize.x - 500, 110}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({500, s_ImgGuiIO.DisplaySize.y - 110}, ImGuiCond_FirstUseEver);
    ImGui::Begin(ICON_MD_TUNE " Entity properties", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    if (IsSpecialEntityTreeNode(m_SelectedEntity)) {
        ImGui::End();
        return;
    }

    const auto s_EntitySceneContext = SDK()->Globals()->GameSceneflowModule->m_pEntitySceneContext;

    const auto s_SelectedEntity = m_SelectedEntity;

    if (!s_EntitySceneContext || !s_EntitySceneContext->m_pScene || !s_SelectedEntity) {
        ImGui::End();
        return;
    }

    if (ImGui::Button(ICON_MD_SUPERVISOR_ACCOUNT)) {
        OnSelectEntity(s_SelectedEntity.GetLogicalParent(), true, std::nullopt);
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Select logical parent");
    }

    ImGui::SameLine(0, 5);

    if (ImGui::Button(ICON_MD_BRANDING_WATERMARK)) {
        OnSelectEntity(s_SelectedEntity.GetOwningEntity(), true, std::nullopt);
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Select owning entity (brick)");
    }

    ImGui::SameLine(0, 5);

    if (ImGui::Button(ICON_MD_DESELECT)) {
        OnSelectEntity({}, false, std::nullopt);
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Deselect");
    }

    // Only show destroy button for custom entities.
    if (m_EntityNames.contains(s_SelectedEntity)) {
        ImGui::SameLine(0, 5);

        if (ImGui::Button(ICON_MD_DELETE)) {
            OnDestroyEntity(s_SelectedEntity, std::nullopt);

            // Prevent crash on destroy.
            ImGui::End();
            return;
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Destroy entity");
        }
    }

    static bool s_LocalTransform = false;
    ImGui::Checkbox("Local transforms", &s_LocalTransform);

    ImGui::SameLine(0, 20);

    if (ImGui::Checkbox("QNE transforms", &m_UseQneTransforms)) {
        SetSettingBool("general", "qne_transforms", m_UseQneTransforms);
    }

    if (ImGui::Checkbox("Round copied matrix values", &m_RoundCopiedMatrixValues)) {
        SetSettingBool("general", "round_copied_matrix_values", m_RoundCopiedMatrixValues);
    }

    ImGui::BeginDisabled(!m_RoundCopiedMatrixValues);

    ImGui::AlignTextToFramePadding();

    ImGui::TextUnformatted("Decimal places");

    ImGui::SameLine();

    ImGui::SetNextItemWidth(150.0f);

    if (ImGui::SliderInt("##DecimalPlaces", &m_CopyDecimalPlaces, 0, 6)) {
        SetSettingInt("general", "copy_decimal_places", m_CopyDecimalPlaces);
    }

    ImGui::EndDisabled();

    ImGui::Separator();

    if (ImGui::Checkbox("##UseSnap", &m_UseSnap)) {
        SetSettingBool("general", "snap", m_UseSnap);
    }

    ImGui::SameLine();

    if (ImGui::InputDouble("Snap", &m_SnapValue)) {
        SetSettingDouble("general", "snap_value", m_SnapValue);
    }

    if (ImGui::Checkbox("##UseAngleSnap", &m_UseAngleSnap)) {
        SetSettingBool("general", "angle_snap", m_UseAngleSnap);
    }

    ImGui::SameLine();

    if (ImGui::InputDouble("Angle snap", &m_AngleSnapValue)) {
        SetSettingDouble("general", "angle_snap_value", m_AngleSnapValue);
    }

    if (ImGui::Checkbox("##UseScaleSnap", &m_UseScaleSnap)) {
        SetSettingBool("general", "scale_snap", m_UseScaleSnap);
    }

    ImGui::SameLine();

    if (ImGui::InputDouble("Scale snap", &m_ScaleSnapValue)) {
        SetSettingDouble("general", "scale_snap_value", m_ScaleSnapValue);
    }

    ImGui::Separator();

    std::string s_ReferencedTemplateFactoryType;
    ZRuntimeResourceID s_ReferencedTemplateFactory;

    {
        std::shared_lock s_TreeLock(m_CachedEntityTreeMutex);
        auto s_Iterator = m_CachedEntityTreeMap.find(s_SelectedEntity);

        if (s_Iterator != m_CachedEntityTreeMap.end()) {
            const std::shared_ptr<EntityTreeNode> s_EntityTreeNode = s_Iterator->second;
            const std::string_view s_EntityName = s_EntityTreeNode->GetEntityName();

            ImGui::TextUnformatted(fmt::format("Entity name: {}", s_EntityName).c_str());

            if (ImGuiCopyWidget("EntityName")) {
                CopyToClipboard(s_EntityName);
            }

            ImGui::TextUnformatted(fmt::format("Entity ID: {:016x}", s_EntityTreeNode->m_EntityID).c_str());

            if (ImGuiCopyWidget("EntityID")) {
                CopyToClipboard(fmt::format("{:016x}", s_EntityTreeNode->m_EntityID));
            }

            ImGui::TextUnformatted(fmt::format("Entity type: {}", s_EntityTreeNode->m_EntityType).c_str());

            if (ImGuiCopyWidget("EntityType")) {
                CopyToClipboard(s_EntityTreeNode->m_EntityType);
            }

            {
                std::shared_lock s_FactoryLock(m_EntityRefToFactoryRuntimeResourceIDsMutex);
                auto s_Iterator2 = m_EntityRefToFactoryRuntimeResourceIDs.find(s_SelectedEntity);

                if (s_Iterator2 != m_EntityRefToFactoryRuntimeResourceIDs.end()) {
                    const auto [s_TemplateFactoryRuntimeResourceID, s_ParentTemplateFactoryRuntimeResourceID] = s_Iterator2->second;

                    if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "TBLU") {
                        s_ReferencedTemplateFactoryType = "TEMP";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "ASEB") {
                        s_ReferencedTemplateFactoryType = "ASET";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "CBLU") {
                        s_ReferencedTemplateFactoryType = "CPPT";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "ECPB") {
                        s_ReferencedTemplateFactoryType = "ECPT";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "UICB") {
                        s_ReferencedTemplateFactoryType = "UICT";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "MATB") {
                        s_ReferencedTemplateFactoryType = "MATT";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "WSWB") {
                        s_ReferencedTemplateFactoryType = "WSWT";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "WSGB") {
                        s_ReferencedTemplateFactoryType = "WSGT";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "PEBL") {
                        s_ReferencedTemplateFactoryType = "PETY";
                    }
                    else if (s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "SNEB") {
                        s_ReferencedTemplateFactoryType = "SNET";
                    }

                    s_ReferencedTemplateFactory = s_TemplateFactoryRuntimeResourceID;

                    ImGui::TextUnformatted(
                        fmt::format("{}: {:016X}", s_ReferencedTemplateFactoryType, s_TemplateFactoryRuntimeResourceID.GetID()).c_str()
                    );

                    if (ImGui::IsItemHovered() && s_ReferencedTemplateFactoryType == "TEMP") {
                        TResourcePtr<ZTemplateEntityFactory> s_ResourcePtr;
                        SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_TemplateFactoryRuntimeResourceID);

                        ZTemplateEntityFactory* s_TemplateEntityFactory = s_ResourcePtr.GetResource();

                        if (s_TemplateEntityFactory) {
                            ImGui::SetTooltip(s_TemplateEntityFactory->m_SourceResourceID.c_str());
                        }
                    }

                    if (ImGuiCopyWidget("ReferencedTemplateFactory")) {
                        CopyToClipboard(fmt::format("{:016X}", s_TemplateFactoryRuntimeResourceID.GetID()));
                    }

                    ImGui::TextUnformatted(
                        fmt::format(
                            "{}: {:016X}", s_EntityTreeNode->m_ReferencedBlueprintFactoryType, s_EntityTreeNode->m_ReferencedBlueprintFactory.GetID()
                        )
                            .c_str()
                    );

                    if (ImGui::IsItemHovered() && s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "TBLU") {
                        TResourcePtr<ZTemplateEntityBlueprintFactory> s_ResourcePtr;
                        SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_EntityTreeNode->m_ReferencedBlueprintFactory);

                        ZTemplateEntityBlueprintFactory* s_TemplateEntityBlueprintFactory = s_ResourcePtr.GetResource();

                        if (s_TemplateEntityBlueprintFactory) {
                            ImGui::SetTooltip(s_TemplateEntityBlueprintFactory->m_SourceResourceID.c_str());
                        }
                    }

                    if (ImGuiCopyWidget("ReferencedBlueprintFactory")) {
                        CopyToClipboard(fmt::format("{:016X}", s_EntityTreeNode->m_ReferencedBlueprintFactory.GetID()));
                    }

                    ImGui::TextUnformatted(
                        fmt::format(
                            "Contained in {}: {:016X}", s_EntityTreeNode->m_BlueprintFactoryType == "TBLU" ? "TEMP" : "ASET",
                            s_ParentTemplateFactoryRuntimeResourceID.GetID()
                        )
                            .c_str()
                    );

                    if (ImGui::IsItemHovered() && s_EntityTreeNode->m_BlueprintFactoryType == "TBLU") {
                        TResourcePtr<ZTemplateEntityFactory> s_ResourcePtr;
                        SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_ParentTemplateFactoryRuntimeResourceID);

                        ZTemplateEntityFactory* s_TemplateEntityFactory = s_ResourcePtr.GetResource();

                        if (s_TemplateEntityFactory) {
                            ImGui::SetTooltip(s_TemplateEntityFactory->m_SourceResourceID.c_str());
                        }
                    }

                    if (ImGuiCopyWidget("ParentTemplateFactory")) {
                        CopyToClipboard(fmt::format("{:016X}", s_ParentTemplateFactoryRuntimeResourceID.GetID()));
                    }

                    ImGui::TextUnformatted(
                        fmt::format(
                            "Contained in {}: {:016X}", s_EntityTreeNode->m_BlueprintFactoryType, s_EntityTreeNode->m_BlueprintFactory.GetID()
                        )
                            .c_str()
                    );

                    if (ImGui::IsItemHovered() && s_EntityTreeNode->m_BlueprintFactoryType == "TBLU") {
                        TResourcePtr<ZTemplateEntityBlueprintFactory> s_ResourcePtr;
                        SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_EntityTreeNode->m_BlueprintFactory);

                        ZTemplateEntityBlueprintFactory* s_TemplateEntityBlueprintFactory = s_ResourcePtr.GetResource();

                        if (s_TemplateEntityBlueprintFactory) {
                            ImGui::SetTooltip(s_TemplateEntityBlueprintFactory->m_SourceResourceID.c_str());
                        }
                    }

                    if (ImGuiCopyWidget("ParentBlueprintFactory")) {
                        CopyToClipboard(fmt::format("{:016X}", s_EntityTreeNode->m_BlueprintFactory.GetID()));
                    }
                }
                else {
                    if (!s_EntityTreeNode->m_ReferencedBlueprintFactoryType.empty()) {
                        ImGui::TextUnformatted(
                            fmt::format(
                                "{}: {:016X}", s_EntityTreeNode->m_ReferencedBlueprintFactoryType,
                                s_EntityTreeNode->m_ReferencedBlueprintFactory.GetID()
                            )
                                .c_str()
                        );

                        if (ImGui::IsItemHovered() && s_EntityTreeNode->m_ReferencedBlueprintFactoryType == "TBLU") {
                            TResourcePtr<ZTemplateEntityBlueprintFactory> s_ResourcePtr;
                            SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_EntityTreeNode->m_ReferencedBlueprintFactory);

                            ZTemplateEntityBlueprintFactory* s_TemplateEntityBlueprintFactory = s_ResourcePtr.GetResource();

                            if (s_TemplateEntityBlueprintFactory) {
                                ImGui::SetTooltip(s_TemplateEntityBlueprintFactory->m_SourceResourceID.c_str());
                            }
                        }

                        if (ImGuiCopyWidget("ReferencedBlueprintFactory")) {
                            CopyToClipboard(fmt::format("{:016X}", s_EntityTreeNode->m_ReferencedBlueprintFactory.GetID()));
                        }
                    }

                    ImGui::TextUnformatted(
                        fmt::format(
                            "Contained in {}: {:016X}", s_EntityTreeNode->m_BlueprintFactoryType, s_EntityTreeNode->m_BlueprintFactory.GetID()
                        )
                            .c_str()
                    );

                    if (ImGui::IsItemHovered() && s_EntityTreeNode->m_BlueprintFactoryType == "TBLU") {
                        TResourcePtr<ZTemplateEntityBlueprintFactory> s_ResourcePtr;
                        SDK()->Globals()->ResourceManager->LoadResource(s_ResourcePtr, s_EntityTreeNode->m_BlueprintFactory);

                        ZTemplateEntityBlueprintFactory* s_TemplateEntityBlueprintFactory = s_ResourcePtr.GetResource();

                        if (s_TemplateEntityBlueprintFactory) {
                            ImGui::SetTooltip(s_TemplateEntityBlueprintFactory->m_SourceResourceID.c_str());
                        }
                    }

                    if (ImGuiCopyWidget("ParentBlueprintFactory")) {
                        CopyToClipboard(fmt::format("{:016X}", s_EntityTreeNode->m_BlueprintFactory.GetID()));
                    }
                }
            }
        };
    }

    if (const ZGeomEntity* s_GeomEntity = s_SelectedEntity.QueryInterface<ZGeomEntity>()) {
        if (s_GeomEntity->m_ResourceID.m_ResourceIndex.val != -1) {
            const auto s_PrimResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[s_GeomEntity->m_ResourceID.m_ResourceIndex.val];
            const auto s_PrimHash = s_PrimResourceInfo.rid.GetID();

            ImGui::TextUnformatted(fmt::format("Associated PRIM: {:016X}", s_PrimHash).c_str());

            const auto s_ContainedRpkg = (*SDK()->Globals()->ResourceContainer)->m_MountedPackages[s_PrimResourceInfo.packageId];

            if (ImGuiCopyWidget("EntPrim")) {
                CopyToClipboard(fmt::format("{:016X}", s_PrimHash));
            }
        }
    }

    if (const auto s_Spatial = s_SelectedEntity.QueryInterface<ZSpatialEntity>()) {
        ImGui::TextUnformatted("Entity transform");

        auto s_Trans = s_Spatial->GetObjectToWorldMatrix();

        if (s_LocalTransform) {
            SMatrix s_ParentTrans;

            // Get parent entity.
            if (s_Spatial->m_eidParent.m_pInterfaceRef) {
                s_ParentTrans = s_Spatial->m_eidParent.m_pInterfaceRef->GetObjectToWorldMatrix();
            }
            else if (s_SelectedEntity.GetLogicalParent() && s_SelectedEntity.GetLogicalParent().QueryInterface<ZSpatialEntity>()) {
                s_ParentTrans = s_SelectedEntity.GetLogicalParent().QueryInterface<ZSpatialEntity>()->GetObjectToWorldMatrix();
            }
            else if (s_SelectedEntity.GetOwningEntity() && s_SelectedEntity.GetOwningEntity().QueryInterface<ZSpatialEntity>()) {
                s_ParentTrans = s_SelectedEntity.GetOwningEntity().QueryInterface<ZSpatialEntity>()->GetObjectToWorldMatrix();
            }

            const auto s_ParentTransInv = s_ParentTrans.Inverse();

            auto s_LocalTrans = s_ParentTransInv * s_Trans;
            s_LocalTrans.Trans = s_Trans.Trans - s_ParentTrans.Trans;
            s_LocalTrans.Trans.w = 1.f;

            s_Trans = s_LocalTrans;
        }

        if (ImGuiCopyWidget("EntTrans")) {
            CopyToClipboard(fmt::format("{}", s_Trans));
        }

        ImGui::Separator();

        if (ImGui::BeginTable("EntityTransform", 4)) {
            for (int i = 0; i < 4; ++i) {
                ImGui::TableNextRow();

                for (int j = 0; j < 4; ++j) {
                    ImGui::TableSetColumnIndex(j);
                    ImGui::Text("%f", s_Trans.flt[(i * 4) + j]);
                }
            }

            ImGui::EndTable();
        }

        ImGui::Separator();

        if (ImGui::Button(ICON_MD_CONTENT_COPY " RT JSON##EntRT")) {
            CopyToClipboard(
                fmt::format(
                    "{{"
                    "\"XAxis\":{{\"x\":{},\"y\":{},\"z\":{}}},"
                    "\"YAxis\":{{\"x\":{},\"y\":{},\"z\":{}}},"
                    "\"ZAxis\":{{\"x\":{},\"y\":{},\"z\":{}}},"
                    "\"Trans\":{{\"x\":{},\"y\":{},\"z\":{}}}"
                    "}}",
                    s_Trans.XAxis.x, s_Trans.XAxis.y, s_Trans.XAxis.z, s_Trans.YAxis.x, s_Trans.YAxis.y, s_Trans.YAxis.z, s_Trans.ZAxis.x,
                    s_Trans.ZAxis.y, s_Trans.ZAxis.z, s_Trans.Trans.x, s_Trans.Trans.y, s_Trans.Trans.z
                )
            );
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_MD_CONTENT_COPY " QN JSON##EntQN")) {
            const auto s_QneTrans = MatrixToQneTransform(s_Trans);

            CopyToClipboard(
                fmt::format(
                    "{{\"rotation\":{{\"x\":{},\"y\":{},\"z\":{}}},"
                    "\"position\":{{\"x\":{},\"y\":{},\"z\":{}}}}}",
                    s_QneTrans.m_Rotation.x, s_QneTrans.m_Rotation.y, s_QneTrans.m_Rotation.z, s_QneTrans.m_Position.x, s_QneTrans.m_Position.y,
                    s_QneTrans.m_Position.z
                )
            );
        }

        if (ImGui::Button(ICON_MD_PERSON_PIN " Move to player")) {
            if (auto s_LocalPlayer = SDK()->Globals()->LocalPlayerData->m_pCharacterImpl->m_pCharacter) {
                auto s_PlayerSpatial = s_LocalPlayer.m_entityRef.QueryInterface<ZSpatialEntity>();

                OnEntityTransformChange(s_SelectedEntity, s_PlayerSpatial->GetObjectToWorldMatrix(), false, std::nullopt);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(ICON_MD_PERSON_PIN_CIRCLE " Move player to")) {
            if (auto s_LocalPlayer = SDK()->Globals()->LocalPlayerData->m_pCharacterImpl->m_pCharacter) {
                auto s_PlayerSpatial = s_LocalPlayer.m_entityRef.QueryInterface<ZSpatialEntity>();

                s_PlayerSpatial->SetObjectToWorldMatrixFromEditor(s_Spatial->GetObjectToWorldMatrix());

                OnEntityTransformChange(s_LocalPlayer.m_entityRef, s_Spatial->GetObjectToWorldMatrix(), false, std::nullopt);
            }
        }
    }

    if (const auto s_CameraEntity = s_SelectedEntity.QueryInterface<ZCameraEntity>()) {
        if (ImGui::Button(ICON_MD_CAMERA "Toggle camera")) {
            ZEntityRef s_EntRef;
            s_CameraEntity->GetID(s_EntRef);

            if (m_CameraActive) {
                m_CameraActive = false;
                DeactivateCamera();
            }
            else {
                m_CameraActive = true;
                ActivateCamera(s_EntRef);
            }
        }
    }

    ImGui::Separator();

    static char s_InputPinName[1024] = {};
    static char s_InputPinTypeName[2048] = {};

    if (ImGui::Button(ICON_MD_BOLT "##FireInputPin")) {
        OnSignalEntityPin(s_SelectedEntity, s_InputPinName, false, m_InputPinValue);

        s_InputPinName[0] = '\0';
        s_InputPinTypeName[0] = '\0';

        m_InputPinValue.Clear();
    }

    auto s_InputPins = GetPins(s_SelectedEntity, false);

    ImGui::SameLine(0, 5);

    knt::util::InputWithAutocomplete(
        "Input pin##InputPinsPopup", s_InputPinName, sizeof(s_InputPinName), s_InputPins,
        [](const PinInfo& s_PinInfo) -> std::string { return s_PinInfo.m_Name; },
        [](const PinInfo& s_PinInfo) -> std::string {
            if (s_PinInfo.m_Description.empty()) {
                return s_PinInfo.m_Name;
            }

            return s_PinInfo.m_Name + " - " + s_PinInfo.m_Description;
        },
        [](const std::string&, const std::string&, const std::string& p_Value) { strcpy_s(s_InputPinName, p_Value.c_str()); },
        [](const PinInfo& s_PinInfo) -> std::string { return s_PinInfo.m_Name; }
    );

    if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        OnSignalEntityPin(s_SelectedEntity, s_InputPinName, false, m_InputPinValue);

        s_InputPinName[0] = '\0';
        s_InputPinTypeName[0] = '\0';

        m_InputPinValue.Clear();
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Type ");
    ImGui::SameLine();

    knt::util::InputWithAutocomplete(
        "##InputPinTypeNamesPopup", s_InputPinTypeName, sizeof(s_InputPinTypeName), m_PinDataTypes,
        [](auto& p_Pair) -> const std::string& { return p_Pair.first; }, [](auto& p_Pair) -> const std::string& { return p_Pair.first; },
        [&](const std::string&, const std::string& p_TypeName, STypeID* p_TypeID) {
            m_InputPinValue.Clear();
            m_InputPinValue.UNSAFE_SetType(p_TypeID);

            void* s_Data = m_InputPinValue.AllocateMemory();

            if (p_TypeName == "SMatrix43") {
                *static_cast<SMatrix43*>(s_Data) = SMatrix43{};
            }
            else {
                memset(s_Data, 0, p_TypeID->GetTypeInfo()->m_nTypeSize);
            }
        },
        [](auto& p_Pair) -> STypeID* { return p_Pair.second; }
    );

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Value");

    if (std::string(s_InputPinTypeName) != "SMatrix43") {
        ImGui::SameLine();
    }

    if (s_InputPinTypeName[0] == '\0') {
        static char s_EmptyBuffer[1] = {};

        ImGui::BeginDisabled();

        ImGui::InputText("##InputPinValue", s_EmptyBuffer, sizeof(s_EmptyBuffer));

        ImGui::EndDisabled();
    }
    else {
        DrawEntityPinValue("##InputPinValue", s_InputPinTypeName, m_InputPinValue.GetData());
    }

    static char s_OutputPinName[1024] = {};
    static char s_OutputPinTypeName[2048] = {};

    if (ImGui::Button(ICON_MD_BOLT "##FireOutputPin")) {
        OnSignalEntityPin(s_SelectedEntity, s_OutputPinName, true, m_OutputPinValue);

        s_OutputPinName[0] = '\0';
        s_OutputPinTypeName[0] = '\0';

        m_OutputPinValue.Clear();
    }

    auto s_OutputPins = GetPins(s_SelectedEntity, true);

    ImGui::SameLine(0, 5);

    knt::util::InputWithAutocomplete(
        "Output pin##OutputPinsPopup", s_OutputPinName, sizeof(s_OutputPinName), s_OutputPins,
        [](const PinInfo& s_PinInfo) -> std::string { return s_PinInfo.m_Name; },
        [](const PinInfo& s_PinInfo) -> std::string {
            if (s_PinInfo.m_Description.empty()) {
                return s_PinInfo.m_Name;
            }

            return s_PinInfo.m_Name + " - " + s_PinInfo.m_Description;
        },
        [](const std::string&, const std::string&, const std::string& value) { strcpy_s(s_OutputPinName, value.c_str()); },
        [](const PinInfo& s_PinInfo) -> std::string { return s_PinInfo.m_Name; }
    );

    if (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        OnSignalEntityPin(s_SelectedEntity, s_OutputPinName, true, m_OutputPinValue);

        s_OutputPinName[0] = '\0';
        s_OutputPinTypeName[0] = '\0';

        m_OutputPinValue.Clear();
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Type ");
    ImGui::SameLine();

    knt::util::InputWithAutocomplete(
        "##OutputPinTypeNamesPopup", s_OutputPinTypeName, sizeof(s_OutputPinTypeName), m_PinDataTypes,
        [](auto& p_Pair) -> const std::string& { return p_Pair.first; }, [](auto& p_Pair) -> const std::string& { return p_Pair.first; },
        [&](const std::string&, const std::string& p_TypeName, STypeID* p_TypeID) {
            m_OutputPinValue.Clear();
            m_OutputPinValue.UNSAFE_SetType(p_TypeID);

            void* s_Data = m_OutputPinValue.AllocateMemory();

            if (p_TypeName == "SMatrix43") {
                *static_cast<SMatrix43*>(s_Data) = SMatrix43{};
            }
            else {
                memset(s_Data, 0, p_TypeID->GetTypeInfo()->m_nTypeSize);
            }
        },
        [](auto& p_Pair) -> STypeID* { return p_Pair.second; }
    );

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Value");

    if (std::string(s_OutputPinTypeName) != "SMatrix43") {
        ImGui::SameLine();
    }

    if (s_OutputPinTypeName[0] == '\0') {
        static char s_EmptyBuffer[1] = {};

        ImGui::BeginDisabled();

        ImGui::InputText("##OuputPinValue", s_EmptyBuffer, sizeof(s_EmptyBuffer));

        ImGui::EndDisabled();
    }
    else {
        DrawEntityPinValue("##OuputPinValue", s_OutputPinTypeName, m_OutputPinValue.GetData());
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Keywords")) {
        static char s_KeywordString[2048] = {};

        const float buttonWidth = ImGui::CalcTextSize(ICON_MD_ADD " Add keyword").x + ImGui::GetStyle().FramePadding.x * 2.0f;

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonWidth - ImGui::GetStyle().ItemSpacing.x);

        ImGui::InputText("##Keyword", s_KeywordString, sizeof(s_KeywordString));

        ImGui::SameLine();

        if (ImGui::Button(ICON_MD_ADD " Add keyword")) {
            if (strlen(s_KeywordString) > 0) {
                std::vector<std::string> s_Keywords = knt::util::Split(s_KeywordString, "+");
                SKeyword s_Keyword;

                s_Keyword.m_level_0 = Hash::Fnv1a(s_Keywords[0].c_str(), s_Keywords[0].length());

                if (!s_Keywords.empty()) {
                    s_Keyword.m_level_0 = Hash::Fnv1a(s_Keywords[0].c_str(), s_Keywords[0].length());
                }

                if (s_Keywords.size() >= 2) {
                    s_Keyword.m_level_1 = Hash::Fnv1a(s_Keywords[1].c_str(), s_Keywords[1].length());
                }

                if (s_Keywords.size() >= 3) {
                    s_Keyword.m_level_2 = Hash::Fnv1a(s_Keywords[2].c_str(), s_Keywords[2].length());
                }

                if (s_Keywords.size() >= 4) {
                    s_Keyword.m_level_3 = Hash::Fnv1a(s_Keywords[3].c_str(), s_Keywords[3].length());
                }

                SDK()->Functions()->ZGameKeywordManager_AddKeyword->Call(SDK()->Globals()->GameKeywordManager, s_SelectedEntity, s_Keyword);

                s_KeywordString[0] = '\0';
            }
        }

        ImGui::Separator();

        TArray<SKeyword> s_Keywords;

        SDK()->Functions()->ZGameKeywordManager_GetKeywords->Call(SDK()->Globals()->GameKeywordManager, s_SelectedEntity, s_Keywords);

        for (const auto s_Keyword : s_Keywords) {
            ZString s_KeywordString = SDK()->Globals()->GameKeywordManager->GetKeywordString(s_Keyword);

            ImGui::Text(s_KeywordString.c_str());

            ImGui::SameLine();

            ImGui::PushID(s_KeywordString.c_str());

            if (ImGui::SmallButton(ICON_MD_CONTENT_COPY)) {
                CopyToClipboard(s_KeywordString.c_str());
            }

            ImGui::SameLine();

            if (ImGui::SmallButton(ICON_MD_DELETE)) {
                SDK()->Functions()->ZGameKeywordManager_RemoveKeyword->Call(SDK()->Globals()->GameKeywordManager, s_SelectedEntity, s_Keyword);

                ImGui::PopID();

                break;
            }

            ImGui::PopID();

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Remove");
            }
        }
    }

    ImGui::Separator();

    ZRenderMaterialInstance* s_RenderMaterialInstance = nullptr;

    if (s_ReferencedTemplateFactoryType == "MATT") {
        TResourcePtr<ZRenderMaterialEntityFactory> s_MaterialEntityResourcePtr;

        SDK()->Globals()->ResourceManager->GetResourcePtr(s_MaterialEntityResourcePtr, s_ReferencedTemplateFactory, 0);

        ZRenderMaterialEntityFactory* s_RenderMaterialEntityFactory = s_MaterialEntityResourcePtr.GetResource();

        TResourcePtr<ZRenderMaterialInstance> s_RenderMaterialInstanceResourcePtr;

        SDK()->Globals()->ResourceManager->GetResourcePtr(
            s_RenderMaterialInstanceResourcePtr, s_RenderMaterialEntityFactory->m_ridMaterialInstance, 0
        );

        s_RenderMaterialInstance = s_RenderMaterialInstanceResourcePtr.GetResource();
    }

    const auto s_EntityType = s_SelectedEntity->GetType();

    if (s_EntityType && s_EntityType->m_pPropertyData) {
        for (uint32_t i = 0; i < s_EntityType->m_pPropertyData->size(); ++i) {
            SPropertyData* s_Property = &(*s_EntityType->m_pPropertyData)[i];
            const auto* s_PropertyInfo = s_Property->GetPropertyInfo();

            if (!s_PropertyInfo || !s_PropertyInfo->m_propertyInfo.m_Type) {
                continue;
            }

            const auto s_PropertyAddress = reinterpret_cast<uintptr_t>(s_SelectedEntity.m_pObj) + s_Property->m_nPropertyOffset;
            const uint16_t s_TypeSize = s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->m_nTypeSize;
            const uint16_t s_TypeAlignment = s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->m_nTypeAlignment;
            const std::string s_TypeName = s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->pszTypeName;

            // Get the value of the property.
            auto* s_Data = (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->AllocateAligned(s_TypeSize, s_TypeAlignment);

            if (s_PropertyInfo->m_propertyInfo.m_Flags & EPropertyInfoFlags::E_HAS_GETTER_SETTER) {
                s_PropertyInfo->m_propertyInfo.m_PropertyGetter(
                    reinterpret_cast<void*>(s_PropertyAddress), s_Data, s_PropertyInfo->m_propertyInfo.m_nExtraData
                );
            }
            else {
                // SelfAs*Ref properties are stored at the interface offset rather than as an actual TEntityRef.
                // Construct the TEntityRef manually from the entity and interface pointer.
                if (s_Property->m_nPropertyOffset == (*s_SelectedEntity.GetEntity()->GetType()->m_pInterfaceData)[0].m_nInterfaceOffset
                    && s_TypeName.starts_with("TEntityRef")) {
                    TEntityRef<void> s_EntityRef;

                    s_EntityRef.m_entityRef = s_SelectedEntity;
                    s_EntityRef.m_pInterfaceRef = reinterpret_cast<void*>(
                        reinterpret_cast<uintptr_t>(s_SelectedEntity.m_pObj)
                        + (*s_SelectedEntity.GetEntity()->GetType()->m_pInterfaceData)[0].m_nInterfaceOffset
                    );

                    std::memcpy(s_Data, &s_EntityRef, sizeof(s_EntityRef));
                }
                else {
                    s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->m_pTypeFunctions->placementCopyConstruct(
                        s_Data, reinterpret_cast<void*>(s_PropertyAddress)
                    );
                }
            }

            // Render the name of the property.
            ImGui::PushFont(p_Renderer->GetBoldFont());

            ImGui::AlignTextToFramePadding();

            std::string s_PropertyName;

            if (s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->IsResource() || s_PropertyInfo->m_nPropertyID != s_Property->m_nPropertyID) {
                // Some properties don't have a name for some reason. Try to find using RL.
                const auto s_PropertyNameData = KNT_GetPropertyName(s_Property->m_nPropertyID);

                if (s_PropertyNameData.Size > 0) {
                    s_PropertyName.assign(s_PropertyNameData.Data, s_PropertyNameData.Size);
                }
                else {
                    s_PropertyName = fmt::format("~{:08x}", s_Property->m_nPropertyID);
                }
            }
            else {
                s_PropertyName = s_PropertyInfo->m_pszPropertyName;
            }

            if (!s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->IsArray()
                && !s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->IsFixedArray() && s_TypeName != "ZCurve") {
                ImGui::Text("%s", s_PropertyName.c_str());
            }

            if (ImGui::IsItemHovered()) {
                const char* s_TypeName = s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->pszTypeName;

                const char* s_ShaderParameterName = nullptr;

                if (s_RenderMaterialInstance && s_RenderMaterialInstance->m_pEffect) {
                    for (const auto& s_SemanticStringPair : s_RenderMaterialInstance->m_pEffect->m_SemanticStringPairs) {
                        if (s_PropertyName == s_SemanticStringPair.m_MaterialPropertyName) {
                            s_ShaderParameterName = s_SemanticStringPair.m_ShaderParameterName.c_str();
                            break;
                        }
                    }
                }

                if (s_ShaderParameterName) {
                    ImGui::SetTooltip("%s\n%s", s_TypeName, s_ShaderParameterName);
                }
                else {
                    ImGui::SetTooltip("%s", s_TypeName);
                }
            }

            ImGui::PopFont();

            if (!s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->IsArray()
                && !s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->IsFixedArray() && s_TypeName != "ZCurve") {
                ImGui::SameLine();

                // Make the next item fill the rest of the width.
                ImGui::PushItemWidth(-1);
            }

            const std::string s_InputId = std::format("##Property{}", i);

            // Render the value of the property.
            bool s_IsChanged = DrawEntityPropertyValue(
                p_Renderer, s_InputId, s_PropertyName, s_TypeName, s_PropertyInfo->m_propertyInfo.m_Type, s_SelectedEntity, s_Property, s_Data
            );

            if (s_IsChanged) {
                ZObjectRef s_ObjectRef;

                s_ObjectRef.Assign(s_PropertyInfo->m_propertyInfo.m_Type, s_Data);

                OnSetPropertyValue(s_SelectedEntity, s_Property->m_nPropertyID, s_ObjectRef, std::nullopt);
            }

            ImGui::Separator();

            s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->m_pTypeFunctions->destruct(s_Data);
            (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->Free(s_Data);
        }
    }

    ImGui::End();
}

bool Editor::DrawEntityPropertyValue(
    zknt::IImGuiRenderer* p_Renderer, const std::string& p_Id, const std::string& p_PropertyName, const std::string& p_TypeName,
    const STypeID* p_TypeID, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data
) {
    bool s_IsChanged = false;

    if (p_TypeName == "ZString") {
        s_IsChanged = StringProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "bool") {
        s_IsChanged = BoolProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "uint8") {
        s_IsChanged = Uint8Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "int8") {
        s_IsChanged = Int8Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "uint16") {
        s_IsChanged = Uint16Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "int16") {
        s_IsChanged = Int16Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "uint32") {
        s_IsChanged = Uint32Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "int32") {
        s_IsChanged = Int32Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "uint64") {
        s_IsChanged = Uint64Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "int64") {
        s_IsChanged = Int64Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "float32") {
        s_IsChanged = Float32Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "float64") {
        s_IsChanged = Float64Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "SVector2") {
        s_IsChanged = SVector2Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "SVector3") {
        s_IsChanged = SVector3Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "SVector4") {
        s_IsChanged = SVector4Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "SMatrix43") {
        s_IsChanged = SMatrix43Property(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "SColorRGB") {
        s_IsChanged = SColorRGBProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "SColorRGBA") {
        s_IsChanged = SColorRGBAProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeID->GetTypeInfo()->IsEnum()) {
        s_IsChanged = EnumProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeID->GetTypeInfo()->IsResource()) {
        ResourcePtrProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "ZRuntimeResourceID") {
        ZRuntimeResourceIDProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName.starts_with("ZEntityRef")) {
        ZEntityRefProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName.starts_with("TEntityRef")) {
        TEntityRefProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "ZRepositoryID") {
        ZRepositoryIDProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeName == "ZGuid") {
        ZGuidProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else if (p_TypeID->GetTypeInfo()->IsArray() || p_TypeID->GetTypeInfo()->IsFixedArray()) {
        s_IsChanged = ArrayProperty(p_Renderer, p_Id, p_Entity, p_Property, p_Data, p_PropertyName, p_TypeID);
    }
    else if (p_TypeName == "ZCurve") {
        s_IsChanged = ZCurveProperty(p_Renderer, p_Id, p_Entity, p_Property, p_Data, p_PropertyName, p_TypeID);
    }
    else if (p_TypeName == "ZGameTime") {
        s_IsChanged = ZGameTimeProperty(p_Id, p_Entity, p_Property, p_Data);
    }
    else {
        UnsupportedProperty(p_Id, p_Entity, p_Property, p_Data);
    }

    return s_IsChanged;
}

void Editor::DrawEntityPinValue(const std::string& p_Id, const std::string& p_TypeName, void* p_Data) {
    if (p_TypeName == "bool") {
        auto s_Value = static_cast<bool*>(p_Data);

        ImGui::Checkbox(p_Id.c_str(), s_Value);
    }
    else if (p_TypeName == "uint8") {
        auto s_Value = static_cast<uint8*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_U8, s_Value);
    }
    else if (p_TypeName == "int8") {
        auto s_Value = static_cast<int8*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_S8, s_Value);
    }
    else if (p_TypeName == "uint16") {
        auto s_Value = static_cast<uint16*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_U16, s_Value);
    }
    else if (p_TypeName == "int16") {
        auto s_Value = static_cast<int16*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_S16, s_Value);
    }
    else if (p_TypeName == "uint32") {
        auto s_Value = static_cast<uint32*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_U32, s_Value);
    }
    else if (p_TypeName == "int32") {
        auto s_Value = static_cast<int32*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_S32, s_Value);
    }
    else if (p_TypeName == "uint64") {
        auto s_Value = static_cast<uint64*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_U64, s_Value);
    }
    else if (p_TypeName == "int64") {
        auto s_Value = static_cast<int64*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_S64, s_Value);
    }
    else if (p_TypeName == "float32") {
        auto s_Value = static_cast<float32*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_Float, s_Value, 0.1f);
    }
    else if (p_TypeName == "float64") {
        auto s_Value = static_cast<float64*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_Double, s_Value, 0.1f);
    }
    else if (p_TypeName == "SVector2") {
        auto s_Value = static_cast<SVector2*>(p_Data);

        ImGui::DragFloat2(p_Id.c_str(), &s_Value->x, 0.1f);
    }
    else if (p_TypeName == "SVector3") {
        auto s_Value = static_cast<SVector3*>(p_Data);

        ImGui::DragFloat3(p_Id.c_str(), &s_Value->x, 0.1f);
    }
    else if (p_TypeName == "SVector4") {
        auto s_Value = static_cast<SVector4*>(p_Data);

        ImGui::DragFloat4(p_Id.c_str(), &s_Value->x, 0.1f);
    }
    else if (p_TypeName == "SMatrix43") {
        auto s_Value = static_cast<SMatrix43*>(p_Data);

        if (m_UseQneTransforms) {
            auto s_QneTransform = MatrixToQneTransform(*s_Value);

            if (ImGui::DragFloat3((p_Id + "p").c_str(), &s_QneTransform.m_Position.x, 0.1f)) {
                const auto s_Matrix = QneTransformToMatrix(s_QneTransform);
                *s_Value = s_Matrix.ToMatrix43();
            }

            if (ImGui::DragFloat3((p_Id + "r").c_str(), &s_QneTransform.m_Rotation.x, 0.1f)) {
                const auto s_Matrix = QneTransformToMatrix(s_QneTransform);
                *s_Value = s_Matrix.ToMatrix43();
            }
        }
        else {
            ImGui::DragFloat3((p_Id + "x").c_str(), &s_Value->XAxis.x, 0.1f);
            ImGui::DragFloat3((p_Id + "y").c_str(), &s_Value->YAxis.x, 0.1f);
            ImGui::DragFloat3((p_Id + "z").c_str(), &s_Value->ZAxis.x, 0.1f);
            ImGui::DragFloat3((p_Id + "t").c_str(), &s_Value->Trans.x, 0.1f);
        }
    }
    else if (p_TypeName == "SColorRGB") {
        auto s_Value = static_cast<SColorRGB*>(p_Data);

        ImGui::ColorEdit3(p_Id.c_str(), &s_Value->r);
    }
    else if (p_TypeName == "SColorRGBA") {
        auto s_Value = static_cast<SColorRGBA*>(p_Data);

        ImGui::ColorEdit4(p_Id.c_str(), &s_Value->r);
    }
    if (p_TypeName == "ZString") {
        auto* s_String = static_cast<ZString*>(p_Data);

        static char s_StringBuffer[65536] = {};

        if (ImGui::InputText(p_Id.c_str(), s_StringBuffer, sizeof(s_StringBuffer))) {
            *s_String = ZString(s_StringBuffer);
        }
    }
    else if (p_TypeName == "ZRuntimeResourceID") {
        auto* s_RuntimeResourceID = static_cast<ZRuntimeResourceID*>(p_Data);

        static char s_StringBuffer[65536] = {};

        if (ImGui::InputText(p_Id.c_str(), s_StringBuffer, sizeof(s_StringBuffer))) {
            *s_RuntimeResourceID = ZRuntimeResourceID::FromString(s_StringBuffer);
        }
    }
    else if (p_TypeName.starts_with("ZEntityRef")) {
        auto s_EntityRef = reinterpret_cast<ZEntityRef*>(p_Data);

        static char s_StringBuffer[65536] = {};

        if (ImGui::InputText(p_Id.c_str(), s_StringBuffer, sizeof(s_StringBuffer))) {
            uint64 s_EntityId = std::strtoull(s_StringBuffer, nullptr, 16);

            std::shared_lock s_TreeLock(m_CachedEntityTreeMutex);

            for (const auto& s_Pair : m_CachedEntityTreeMap) {
                if (s_Pair.first.GetEntity()->GetType()->m_nEntityID == s_EntityId) {
                    *s_EntityRef = s_Pair.first;
                    break;
                }
            }
        }
    }
    else if (p_TypeName.starts_with("TEntityRef")) {
        auto s_EntityRef = reinterpret_cast<TEntityRef<void>*>(p_Data);

        static char s_StringBuffer[65536] = {};

        if (ImGui::InputText(p_Id.c_str(), s_StringBuffer, sizeof(s_StringBuffer))) {
            uint64 s_EntityId = std::strtoull(s_StringBuffer, nullptr, 16);

            std::shared_lock s_TreeLock(m_CachedEntityTreeMutex);

            for (const auto& s_Pair : m_CachedEntityTreeMap) {
                if (s_Pair.first.GetEntity()->GetType()->m_nEntityID == s_EntityId) {
                    s_EntityRef->m_entityRef = s_Pair.first;

                    std::string s_TypeName = p_TypeName.substr(11, p_TypeName.find(">") - 11);
                    STypeID* s_TypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID(s_TypeName);

                    s_EntityRef->m_pInterfaceRef = s_EntityRef->m_entityRef.QueryInterface(s_TypeID);
                    break;
                }
            }
        }
    }
    else if (p_TypeName == "ZRepositoryID") {
        auto s_RepositoryId = reinterpret_cast<ZRepositoryID*>(p_Data);

        static char s_StringBuffer[65536] = {};

        if (ImGui::InputText(p_Id.c_str(), s_StringBuffer, sizeof(s_StringBuffer))) {
            *s_RepositoryId = ZString(s_StringBuffer);
        }
    }
    else if (p_TypeName == "ZGuid") {
        auto s_Guid = reinterpret_cast<ZGuid*>(p_Data);

        static char s_StringBuffer[65536] = {};

        if (ImGui::InputText(p_Id.c_str(), s_StringBuffer, sizeof(s_StringBuffer))) {
            *s_Guid = ZString(s_StringBuffer);
        }
    }
    else if (p_TypeName == "ZGameTime") {
        auto s_Value = static_cast<ZGameTime*>(p_Data);

        ImGui::DragScalar(p_Id.c_str(), ImGuiDataType_S64, &s_Value->m_nTicks);
    }
}
