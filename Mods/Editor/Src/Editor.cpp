#include <queue>
#include <numbers>
#include <future>

#include "Editor.hpp"

#include "IconsMaterialDesign.h"

#include <simdjson.h>

#include <Glacier/ZModule.hpp>
#include <Glacier/ZCollision.hpp>
#include <Glacier/ZHumanoid.hpp>
#include <Glacier/ZGameLoopManager.hpp>

#include <Logging.hpp>
#include <Util/ImGuiUtils.hpp>
#include <Util/HttpUtils.hpp>

#undef min
#undef max

Editor::Editor() {}

Editor::~Editor() {
    const auto s_DeleteEntity = [](auto* p_Ref) {
        if (*p_Ref) {
            SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, p_Ref->m_entityRef);
        }

        *p_Ref = {};
    };

    if (m_SelectionForFreeCameraEditorStyleEntity) {
        SDK()->Globals()->Selections->clear();
    }

    s_DeleteEntity(&m_SelectionForFreeCameraEditorStyleEntity);
    s_DeleteEntity(&m_OutfitRefGetter);
    s_DeleteEntity(&m_HumanoidGetter);
    s_DeleteEntity(&m_HumanoidTeleporter);
    s_DeleteEntity(&m_TeleportTarget);
    s_DeleteEntity(&m_TargetEntityRefValue);
    s_DeleteEntity(&m_HumanoidRefFromEntityRefGetter);
    s_DeleteEntity(&m_HumanoidOutfitSetter);
    s_DeleteEntity(&m_HumanoidHealthGetter);
    s_DeleteEntity(&m_HumanoidMaxHealthGetter);
    s_DeleteEntity(&m_HumanoidHealthSetter);
    s_DeleteEntity(&m_HealthAmmountFloatValue);
    s_DeleteEntity(&m_InterruptPassiveRegenerationBoolValue);
    s_DeleteEntity(&m_HumanoidImmuneToDamageSetter);
    s_DeleteEntity(&m_InvulnerableBoolValue);
    s_DeleteEntity(&m_HumanoidDisarmer);
    s_DeleteEntity(&m_IsHumanoidAliveGetter);
}

void Editor::Init() {
    SDK()->Hooks()->ZEntitySceneContext_CreateScene->AddDetour(this, &Editor::OnCreateScene);
    SDK()->Hooks()->ZEntitySceneContext_ClearScene->AddDetour(this, &Editor::OnClearScene);

    SDK()->Hooks()->ZEntitySceneContext_LoadDynamicBrick->AddDetour(this, &Editor::ZEntitySceneContext_LoadDynamicBrick);
    SDK()->Hooks()->ZEntitySceneContext_UnloadDynamicBrick->AddDetour(this, &Editor::ZEntitySceneContext_UnloadDynamicBrick);

    SDK()->Hooks()->ZTemplateEntityFactory_ConfigureEntity->AddDetour(this, &Editor::ZTemplateEntityFactory_ConfigureEntity);

    SDK()->Hooks()->ZEntityManager_NewUninitializedEntity->AddDetour(this, &Editor::ZEntityManager_NewUninitializedEntity);
    SDK()->Hooks()->GenerateEntityIdentifier->AddDetour(this, &Editor::GenerateEntityIdentifier);
    SDK()->Hooks()->ZEntityManager_DeleteEntity->AddDetour(this, &Editor::ZEntityManager_DeleteEntity);
}

void Editor::OnEngineInitialized() {
    const ZMemberDelegate<Editor, void(const SGameUpdateEvent&)> s_Delegate(this, &Editor::OnFrameUpdate);
    SDK()->Globals()->GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdateAlways);

    ZTypeRegistry* s_TypeRegistry = *SDK()->Globals()->TypeRegistry;

    m_PinDataTypes.push_back(std::make_pair("bool", s_TypeRegistry->GetTypeID("bool")));
    m_PinDataTypes.push_back(std::make_pair("uint8", s_TypeRegistry->GetTypeID("uint8")));
    m_PinDataTypes.push_back(std::make_pair("int8", s_TypeRegistry->GetTypeID("int8")));
    m_PinDataTypes.push_back(std::make_pair("uint16", s_TypeRegistry->GetTypeID("uint16")));
    m_PinDataTypes.push_back(std::make_pair("int16", s_TypeRegistry->GetTypeID("int16")));
    m_PinDataTypes.push_back(std::make_pair("uint32", s_TypeRegistry->GetTypeID("uint32")));
    m_PinDataTypes.push_back(std::make_pair("int32", s_TypeRegistry->GetTypeID("int32")));
    m_PinDataTypes.push_back(std::make_pair("uint64", s_TypeRegistry->GetTypeID("uint64")));
    m_PinDataTypes.push_back(std::make_pair("int64", s_TypeRegistry->GetTypeID("int64")));
    m_PinDataTypes.push_back(std::make_pair("float32", s_TypeRegistry->GetTypeID("float32")));
    m_PinDataTypes.push_back(std::make_pair("float64", s_TypeRegistry->GetTypeID("float64")));
    m_PinDataTypes.push_back(std::make_pair("SVector2", s_TypeRegistry->GetTypeID("SVector2")));
    m_PinDataTypes.push_back(std::make_pair("SVector3", s_TypeRegistry->GetTypeID("SVector3")));
    m_PinDataTypes.push_back(std::make_pair("SVector4", s_TypeRegistry->GetTypeID("SVector4")));
    m_PinDataTypes.push_back(std::make_pair("SMatrix43", s_TypeRegistry->GetTypeID("SMatrix43")));
    m_PinDataTypes.push_back(std::make_pair("SColorRGB", s_TypeRegistry->GetTypeID("SColorRGB")));
    m_PinDataTypes.push_back(std::make_pair("SColorRGBA", s_TypeRegistry->GetTypeID("SColorRGBA")));
    m_PinDataTypes.push_back(std::make_pair("ZString", s_TypeRegistry->GetTypeID("ZString")));
    m_PinDataTypes.push_back(std::make_pair("ZRuntimeResourceID", s_TypeRegistry->GetTypeID("ZRuntimeResourceID")));
    m_PinDataTypes.push_back(std::make_pair("ZEntityRef", s_TypeRegistry->GetTypeID("ZEntityRef")));
    m_PinDataTypes.push_back(std::make_pair("ZRepositoryID", s_TypeRegistry->GetTypeID("ZRepositoryID")));
    m_PinDataTypes.push_back(std::make_pair("ZGuid", s_TypeRegistry->GetTypeID("ZGuid")));
    m_PinDataTypes.push_back(std::make_pair("ZGameTime", s_TypeRegistry->GetTypeID("ZGameTime")));

    for (const auto& [s_TypeName, s_TypeID] : s_TypeRegistry->m_types) {
        if (s_TypeID->GetTypeInfo()->IsClass()) {
            if (s_TypeName.StartsWith("TEntityRef<") || s_TypeName.StartsWith("TResourcePtr<")) {
                m_PinDataTypes.push_back(std::make_pair(s_TypeName.c_str(), s_TypeRegistry->GetTypeID(s_TypeName)));
            }
        }
    }

    for (const auto& [s_TypeName, s_TypeID] : s_TypeRegistry->m_types) {
        if (s_TypeID->GetTypeInfo()->IsClass()) {
            m_ClassNames.push_back(std::string(s_TypeName.c_str(), s_TypeName.size()));
        }
    }

    std::sort(m_ClassNames.begin(), m_ClassNames.end(), [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), [](char ac, char bc) {
            return std::tolower(ac) < std::tolower(bc);
        });
    });
}

void Editor::OnDrawMenu(zknt::IImGuiRenderer* p_Renderer) {
    if (ImGui::Button(ICON_MD_VIDEO_SETTINGS "  EDITOR SETTINGS")) {
        m_ShowSettingsWindow = !m_ShowSettingsWindow;
    }

    if (ImGui::Button(ICON_MD_PEOPLE " HUMANOIDS")) {
        m_ShowHumanoidsWindow = !m_ShowHumanoidsWindow;
    }

    if (ImGui::Button(ICON_MD_TUNE " ITEMS")) {
        m_ShowItemsWindow = !m_ShowItemsWindow;
    }

    if (ImGui::Button(ICON_MD_MEETING_ROOM " ROOMS")) {
        m_ShowRoomsWindow = !m_ShowRoomsWindow;
    }

    if (ImGui::Button(ICON_MD_VIEW_IN_AR " BOX REFLECTIONS")) {
        m_ShowBoxReflectionsWindow = !m_ShowBoxReflectionsWindow;
    }

    if (ImGui::Button(ICON_MD_CATEGORY " DEBUG CHANNELS")) {
        m_ShowDebugChannelsWindow = !m_ShowDebugChannelsWindow;
    }
}

void Editor::OnDrawUI(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) {
    auto s_ImgGuiIO = ImGui::GetIO();

    DrawSettingsWindow(p_Renderer, p_HasFocus);

    if (m_ShowEntityEditor) {
        DrawEntityTreeWindow();
        DrawEntityPropertiesWindow(p_Renderer);
        DrawEntityManipulator(p_HasFocus);
    }

    DrawHumanoidsWindow(p_Renderer, p_HasFocus);
    DrawItemsWindow(p_Renderer, p_HasFocus);
    DrawRoomsWindow(p_Renderer, p_HasFocus);
    DrawBoxReflectionsWindow(p_Renderer, p_HasFocus);
    DrawDebugChannelsWindow(p_Renderer, p_HasFocus);
}

void Editor::OnDraw3D(zknt::IDirectXTKRenderer* p_Renderer) {
    DrawEntityAABB(p_Renderer);
}

void Editor::OnDepthDraw3D(zknt::IDirectXTKRenderer* p_Renderer) {
    DrawDebugEntities(p_Renderer);
}

bool Editor::SpawnEntities() {
    if (m_SelectionForFreeCameraEditorStyleEntity) {
        return true;
    }

    CreateSelectionForFreeCameraEditorStyleEntity();

    if (!m_SelectionForFreeCameraEditorStyleEntity) {
        Logger::Error("[Editor] Failed to create selection for free camera editor style entity.");
        return false;
    }

    m_OutfitRefGetter = TEntityRef<ZCLGetOutfitRefFromOutfit>::SpawnEntity(ResId<"[modules:/zclgetoutfitreffromoutfit.class].entitytype">);

    if (!m_OutfitRefGetter) {
        Logger::Error("[Editor] Failed to create get outfit ref from outfit entity.");
        return false;
    }

    m_HumanoidGetter = TEntityRef<ZCLGetHumanoidFromOutfitEntity>::SpawnEntity(ResId<"[modules:/zclgethumanoidfromoutfitentity.class].entitytype">);

    if (!m_HumanoidGetter) {
        Logger::Error("[Editor] Failed to create get humanoid from outfit entity.");
        return false;
    }

    m_IsHumanoidAliveGetter = TEntityRef<ZCLIsHumanoidAlive>::SpawnEntity(ResId<"[modules:/zclishumanoidalive.class].entitytype">);

    if (!m_IsHumanoidAliveGetter) {
        Logger::Error("[Editor] Failed to create is humanoid alive entity.");
        return false;
    }

    m_HumanoidHealthGetter = TEntityRef<ZCLGetHumanoidHealth>::SpawnEntity(ResId<"[modules:/zclgethumanoidhealth.class].entitytype">);

    if (!m_HumanoidHealthGetter) {
        Logger::Error("[Editor] Failed to create get humanoid health entity.");
        return false;
    }

    m_HumanoidMaxHealthGetter = TEntityRef<ZCLGetHumanoidMaxHealth>::SpawnEntity(ResId<"[modules:/zclgethumanoidmaxhealth.class].entitytype">);

    if (!m_HumanoidMaxHealthGetter) {
        Logger::Error("[Editor] Failed to create get humanoid max health entity.");
        return false;
    }

    m_TargetEntityRefValue = TEntityRef<ZCLValueEntityRefEntity>::SpawnEntity(ResId<"[modules:/zclvalueentityrefentity.class].entitytype">);

    if (!m_TargetEntityRefValue) {
        Logger::Error("[Editor] Failed to create value entity ref entity.");
        return false;
    }

    m_HumanoidRefFromEntityRefGetter =
        TEntityRef<ZCLGetHumanoidRefFromEntityRef>::SpawnEntity(ResId<"[modules:/zclgethumanoidreffromentityref.class].entitytype">);

    if (!m_HumanoidRefFromEntityRefGetter) {
        Logger::Error("[Editor] Failed to create get character ref from entity ref entity.");
        return false;
    }

    m_HumanoidOutfitSetter = TEntityRef<ZCLSetHumanoidOutfitEntity>::SpawnEntity(ResId<"[modules:/zclsethumanoidoutfitentity.class].entitytype">);

    if (!m_HumanoidOutfitSetter) {
        Logger::Error("[Editor] Failed to create set humanoid outfit entity.");
        return false;
    }

    m_HealthAmmountFloatValue = TEntityRef<ZCLValueFloatEntity>::SpawnEntity(ResId<"[modules:/zclvaluefloatentity.class].entitytype">);

    if (!m_HealthAmmountFloatValue) {
        Logger::Error("[Editor] Failed to create float value entity for health ammount.");
        return false;
    }

    m_InterruptPassiveRegenerationBoolValue = TEntityRef<ZCLValueBoolEntity>::SpawnEntity(ResId<"[modules:/zclvalueboolentity.class].entitytype">);

    if (!m_InterruptPassiveRegenerationBoolValue) {
        Logger::Error("[Editor] Failed to create bool value entity for interrupt passive regeneration.");
        return false;
    }

    m_HumanoidHealthSetter = TEntityRef<ZCLSetHumanoidHealth>::SpawnEntity(ResId<"[modules:/zclsethumanoidhealth.class].entitytype">);

    if (!m_HumanoidHealthSetter) {
        Logger::Error("[Editor] Failed to create set humanoid health entity.");
        return false;
    }

    m_InvulnerableBoolValue = TEntityRef<ZCLValueBoolEntity>::SpawnEntity(ResId<"[modules:/zclvalueboolentity.class].entitytype">);

    if (!m_InvulnerableBoolValue) {
        Logger::Error("[Editor] Failed to create bool value entity for invulnerable.");
        return false;
    }

    m_HumanoidImmuneToDamageSetter =
        TEntityRef<ZCLSetHumanoidImmuneToDamage>::SpawnEntity(ResId<"[modules:/zclsethumanoidimmunetodamage.class].entitytype">);

    if (!m_HumanoidImmuneToDamageSetter) {
        Logger::Error("[Editor] Failed to create set humanoid immune to damage entity.");
        return false;
    }

    m_HumanoidTeleporter = TEntityRef<ZCLTeleportHumanoidEntity>::SpawnEntity(ResId<"[modules:/zclteleporthumanoidentity.class].entitytype">);

    if (!m_HumanoidTeleporter) {
        Logger::Error("[Editor] Failed to create teleport humanoid entity.");
        return false;
    }

    m_TeleportTarget = TEntityRef<ZSpatialEntity>::SpawnEntity(ResId<"[modules:/zspatialentity.class].entitytype">);

    if (!m_TeleportTarget) {
        Logger::Error("[Editor] Failed to create spatial entity for teleport target.");
        return false;
    }

    m_HumanoidDisarmer = TEntityRef<ZCLDisarmNpcHumanoid>::SpawnEntity(ResId<"[modules:/zcldisarmnpchumanoid.class].entitytype">);

    if (!m_HumanoidDisarmer) {
        Logger::Error("[Editor] Failed to create disarm npc humanoid entity.");
        return false;
    }

    return true;
}

void Editor::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent) {
    ProcessTasks();

    std::lock_guard s_Lock(m_EntityDestructionMutex);

    for (const auto& [s_Entity, s_ClientId] : m_EntitiesToDestroy) {
        DestroyEntityInternal(s_Entity, s_ClientId);
    }

    m_EntitiesToDestroy.clear();

    if (m_CachedEntityTree && !m_IsBuildingEntityTree.load()) {
        std::vector<std::pair<ZEntityRef, IEntityBlueprintFactory*>> s_DynamicEntitiesToAdd;

        {
            std::scoped_lock s_ScopedLock(m_PendingDynamicEntitiesMutex);

            if (!m_PendingDynamicEntities.empty()) {
                s_DynamicEntitiesToAdd.reserve(m_PendingDynamicEntities.size());

                for (const auto& s_Pair : m_PendingDynamicEntities) {
                    s_DynamicEntitiesToAdd.push_back(s_Pair);
                }

                m_PendingDynamicEntities.clear();
            }
        }

        if (!s_DynamicEntitiesToAdd.empty()) {
            std::scoped_lock s_ScopedLock(m_CachedEntityTreeMutex);

            UpdateEntityTree(m_CachedEntityTreeMap, s_DynamicEntitiesToAdd, false, true);

            bool s_HasDebugEntities;

            {
                std::scoped_lock s_Lock(m_DebugEntitiesMutex);
                s_HasDebugEntities = !m_EntityRefToDebugEntities.empty();
            }

            if (s_HasDebugEntities) {
                for (const auto& [s_DynamicEntity, _] : s_DynamicEntitiesToAdd) {
                    auto s_Iterator = m_CachedEntityTreeMap.find(s_DynamicEntity);

                    if (s_Iterator != m_CachedEntityTreeMap.end()) {
                        GetDebugEntities(s_Iterator->second);
                    }
                }
            }
        }

        std::vector<std::pair<ZEntityRef, IEntityBlueprintFactory*>> s_DynamicBricksToAdd;

        {
            std::scoped_lock s_ScopedLock(m_PendingDynamicBricksMutex);

            if (!m_PendingDynamicBricks.empty()) {
                s_DynamicBricksToAdd.reserve(m_PendingDynamicBricks.size());

                for (const auto& s_Pair : m_PendingDynamicBricks) {
                    s_DynamicBricksToAdd.push_back(s_Pair);
                }

                m_PendingDynamicBricks.clear();
            }
        }

        if (!s_DynamicBricksToAdd.empty()) {
            std::scoped_lock s_ScopedLock(m_CachedEntityTreeMutex);

            UpdateEntityTree(m_CachedEntityTreeMap, s_DynamicBricksToAdd, true, false);
        }
    }

    static std::future<std::map<std::string, PinLists>> s_DownloadFuture;
    static bool s_DownloadStarted = false;
    static bool s_DownloadCompleted = false;

    if (m_ClassToInputAndOutputPins.empty() && !s_DownloadStarted) {
        const std::string s_PinsUrl = "https://github.com/glacier-modding/Bond-Hashes/releases/latest/download/pins.json";

        s_DownloadFuture = std::async(std::launch::async, [this, s_PinsUrl]() {
            std::string jsonContent = knt::util::DownloadFromUrl(s_PinsUrl);

            if (!jsonContent.empty()) {
                return ParsePinsJson(jsonContent);
            }

            return std::map<std::string, PinLists>();
        });

        s_DownloadStarted = true;
    }

    if (s_DownloadStarted && !s_DownloadCompleted) {
        if (s_DownloadFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            m_ClassToInputAndOutputPins = s_DownloadFuture.get();
            s_DownloadCompleted = true;
            Logger::Debug("[Editor] Pin list download complete! Loaded {} classes.", m_ClassToInputAndOutputPins.size());
        }
    }
}

void Editor::CreateSelectionForFreeCameraEditorStyleEntity() {
    TResourcePtr<ZCppEntityBlueprintFactory> s_BlueprintFactory;
    TResourcePtr<ZCppEntityFactory> s_Factory;

    SDK()->LoadCppEntity(
        R"({
                "typeName": "zselectionforfreecameraeditorstyleentity",
                "typeFlags": 0
        })",
        R"({
                "id": "[modules:/zselectionforfreecameraeditorstyleentity.class].entityblueprint",
                "type": "CBLU",
                "references": []
        })",
        R"({
                "blueprintIndexInResourceHeader": 0,
                "propertyValues": [
                    {
                        "nPropertyID": 3363090287,
                        "value": {
                            "$type": "TArray<ZEntityReference>",
                            "$val": []
                        }
                    }
                ]
        })",
        R"({
                "id": "[modules:/zselectionforfreecameraeditorstyleentity.class].entitytype",
                "type": "CPPT",
                "references": [
                    "[modules:/zselectionforfreecameraeditorstyleentity.class].entityblueprint"
                ]
        })",
        s_BlueprintFactory, s_Factory
    );

    m_SelectionForFreeCameraEditorStyleEntity = TEntityRef<ZSelectionForFreeCameraEditorStyleEntity>::SpawnEntity(
        ResId<"[modules:/zselectionforfreecameraeditorstyleentity.class].entitytype">
    );

    SDK()->Globals()->Selections->push_back(m_SelectionForFreeCameraEditorStyleEntity);
}

void Editor::OnMouseDown(const SVector2& p_Position, bool p_IsFirstClick) {
    SVector3 s_World;
    SVector3 s_Direction;
    SDK()->ScreenToWorld(p_Position, s_World, s_Direction);

    if (RayCastDebugEntities(s_World, s_Direction)) {
        return;
    }

    float4 s_DirectionVec(s_Direction.x, s_Direction.y, s_Direction.z, 1.f);

    float4 s_From = float4(s_World.x, s_World.y, s_World.z, 1.f);
    float4 s_To = s_From + s_DirectionVec * 200.f;

    if (!SDK()->Globals()->CollisionManager) {
        Logger::Error("[Editor] Collision manager not found.");
        return;
    }

    if (m_RaycastLogging) {
        Logger::Debug("[Editor] RayCasting from {} to {}.", s_From, s_To);
    }

    TResourcePtr<ZEntityRef> s_EntityFactory;
    SDK()->Globals()->ResourceManager->LoadResource(
        s_EntityFactory,
        ResId<"[assembly:/_knt/design/utility/collision_query_presets.template?/collisionquery_blockall.entitytemplate].entityresource">
    );

    SEntityResource* s_EntityResource = static_cast<SEntityResource*>(s_EntityFactory.GetResourceData());
    ZCollisionQueryPreset* s_CollisionQueryPreset = s_EntityResource->entityRef.QueryInterface<ZCollisionQueryPreset>();

    ZRayQueryInput s_RayQueryInput = {};
    s_RayQueryInput.m_BlockingChannelMask = s_CollisionQueryPreset->m_BlockingChannelMask;
    s_RayQueryInput.m_OverlapChannelMask = s_CollisionQueryPreset->m_OverlapChannelMask;
    s_RayQueryInput.m_RequiredAttributeMask = s_CollisionQueryPreset->m_RequiredAttributeMask;
    s_RayQueryInput.m_ForbiddenAttributeMask = s_CollisionQueryPreset->m_ForbiddenAttributeMask;
    s_RayQueryInput.m_eRayCollidables = s_CollisionQueryPreset->m_eCollidableTypes;
    s_RayQueryInput.m_vFrom = s_From;
    s_RayQueryInput.m_vTo = s_To;
    s_RayQueryInput.m_TypedQueryMask = s_CollisionQueryPreset->m_TypedQueryMask;
    s_RayQueryInput.m_eRayDetailLevel = ERayDetailLevel::RAYDETAILS_MESH;

    ZRayQueryOutput s_RayQueryOutput = {};

    if (!SDK()->Globals()->CollisionManager->RayCastClosestHit(&s_RayQueryOutput, s_RayQueryInput)) {
        Logger::Error("[Editor] Raycast failed.");
        return;
    }

    if (m_RaycastLogging) {
        Logger::Debug("[Editor] Raycast output: {}", s_RayQueryOutput.m_vPosition);
    }

    m_From = s_From;
    m_To = s_To;
    m_Hit = s_RayQueryOutput.m_vPosition;
    m_Normal = s_RayQueryOutput.m_vNormal;

    if (p_IsFirstClick) {
        if (s_RayQueryOutput.m_pBlockingSpatialEntity.m_pInterfaceRef) {
            const auto& s_Interfaces = *s_RayQueryOutput.m_pBlockingSpatialEntity.m_pInterfaceRef->GetType()->m_pInterfaceData;

            if (m_RaycastLogging) {
                Logger::Info(
                    "[Editor] Hit entity of type '{}' with id '{:x}'.", s_Interfaces[0].m_Type->GetTypeInfo()->pszTypeName,
                    s_RayQueryOutput.m_pBlockingSpatialEntity.m_entityRef->GetType()->m_nEntityID
                );
            }

            const auto s_EntitySceneContext = SDK()->Globals()->GameSceneflowModule->m_pEntitySceneContext;
            ZEntityRef s_SelectedEntity = s_RayQueryOutput.m_pBlockingSpatialEntity.m_entityRef;
            bool s_IsParentFound = false;

            for (const auto& s_Brick : s_EntitySceneContext->m_SceneConfig->m_aMainBricks) {
                if (s_SelectedEntity.IsAnyParent(s_Brick.m_EntityType)) {
                    s_IsParentFound = true;

                    if (m_RaycastLogging) {
                        Logger::Info("[Editor] Found entity in brick {}.", s_Brick.m_RuntimeResourceID);
                    }

                    break;
                }
            }

            if (!s_IsParentFound) {
                for (const auto& [s_RuntimeResourceID, s_EntityType] : s_EntitySceneContext->m_aDynamicBrickEntities) {
                    if (s_SelectedEntity.IsAnyParent(s_EntityType)) {
                        if (m_RaycastLogging) {
                            Logger::Info("[Editor] Found entity in brick {}.", s_RuntimeResourceID);
                        }

                        break;
                    }
                }
            }

            if (s_SelectedEntity.QueryInterface<ZHumanoidOutfitEntity>()) {
                if (!SpawnEntities()) {
                    OnSelectEntity(s_SelectedEntity, true, std::nullopt);
                    return;
                }

                m_OutfitRefGetter.m_entityRef.SetProperty("m_outfit", TInterfaceRef<ZHumanoidOutfitEntity>::FromEntityRef(s_SelectedEntity));

                m_HumanoidGetter.m_entityRef.SetProperty(
                    "m_outfit", TInterfaceRef<ITEntityRefValue<ZHumanoidOutfitEntity>>::FromEntityRef(m_OutfitRefGetter.m_entityRef)
                );

                ZEntityRef s_HumanoidEntity;
                m_HumanoidGetter.m_pInterfaceRef->GetValue(s_HumanoidEntity);

                OnSelectEntity(s_HumanoidEntity, true, std::nullopt);

                if (m_SelectHumanoidOnMouseClick) {
                    m_SelectedHumanoid = TInterfaceRef<ZHumanoidCharacterEntity>::FromEntityRef(s_HumanoidEntity);

                    if (!m_SelectedHumanoid) {
                        Logger::Error("[Editor] Failed to get ZHumanoidCharacterEntity for selected humanoid.");
                        return;
                    }

                    m_ScrollToHumanoid = true;
                }
            }
            else if (auto s_LinkedEntity = s_SelectedEntity.QueryInterface<ZLinkedEntity>()) {
                ZEntityRef s_EntityRef;
                s_LinkedEntity->GetID(s_EntityRef);

                ZEntityRef s_LogicalParent = s_EntityRef.GetLogicalParent();

                if (s_LogicalParent && s_LogicalParent.QueryInterface<ZItemCharacterEntityBase>()) {
                    OnSelectEntity(s_LogicalParent, true, std::nullopt);
                }
                else {
                    OnSelectEntity(s_SelectedEntity, true, std::nullopt);
                }

                if (m_SelectItemOnMouseClick) {
                    m_SelectedItem = TInterfaceRef<ZItemCharacterEntityBase>::FromEntityRef(s_LogicalParent);

                    if (!m_SelectedItem) {
                        Logger::Error("[Editor] Failed to get ZItemCharacterEntityBase for selected item.");
                        return;
                    }

                    m_ScrollToItem = true;
                }
            }
            else {
                OnSelectEntity(s_SelectedEntity, true, std::nullopt);
            }
        }
    }
}

std::vector<Editor::PinInfo> Editor::GetPins(ZEntityRef p_EntityRef, bool p_OutputPins) {
    std::vector<PinInfo> s_Result;

    if (!p_EntityRef || !p_EntityRef->GetType()->m_pInterfaceData) {
        return s_Result;
    }

    TArray<SInterfaceData>* s_Interfaces = p_EntityRef->GetType()->m_pInterfaceData;

    std::unordered_set<std::string> s_PinNames;

    for (const SInterfaceData& s_InterfaceData : *s_Interfaces) {
        const IType* s_TypeInfo = s_InterfaceData.m_Type->GetTypeInfo();

        if (!s_TypeInfo) {
            continue;
        }

        const std::string s_ClassName = knt::util::ToLowerCase(s_TypeInfo->pszTypeName);

        auto s_Iterator = m_ClassToInputAndOutputPins.find(s_ClassName);

        if (s_Iterator == m_ClassToInputAndOutputPins.end()) {
            continue;
        }

        const auto& s_Pins = p_OutputPins ? s_Iterator->second.m_OutputPins : s_Iterator->second.m_InputPins;

        for (const auto& s_Pin : s_Pins) {
            if (s_PinNames.insert(s_Pin.m_Name).second) {
                s_Result.push_back(s_Pin);
            }
        }
    }

    std::sort(s_Result.begin(), s_Result.end(), [](const PinInfo& p_A, const PinInfo& p_B) { return p_A.m_Name < p_B.m_Name; });

    return s_Result;
}

std::map<std::string, Editor::PinLists> Editor::ParsePinsJson(const std::string& p_PinsJson) {
    std::map<std::string, PinLists> s_Result;

    simdjson::ondemand::parser s_Parser;
    simdjson::padded_string s_Json = simdjson::padded_string(p_PinsJson);

    simdjson::ondemand::document s_Document = s_Parser.iterate(s_Json);
    simdjson::ondemand::array s_Entries = s_Document.get_array();

    for (auto s_Entry : s_Entries) {
        std::string_view s_Path = s_Entry["path"].get_string();

        std::string s_ClassName;
        {
            size_t s_Start = s_Path.find('/');
            size_t s_End = s_Path.find(".class");

            if (s_Start != std::string_view::npos && s_End != std::string_view::npos && s_End > s_Start) {
                s_ClassName = std::string(s_Path.substr(s_Start + 1, s_End - s_Start - 1));
            }
        }

        if (s_ClassName.empty()) {
            continue;
        }

        auto& s_Pins = s_Result[s_ClassName];

        simdjson::ondemand::array s_InputPins = s_Entry["in"].get_array();

        for (auto s_PinEntry : s_InputPins) {
            std::string_view s_PinName = s_PinEntry["pin"].get_string();
            std::string_view s_Description = s_PinEntry["description"].get_string();

            s_Pins.m_InputPins.push_back({std::string(s_PinName), std::string(s_Description)});
        }

        simdjson::ondemand::array s_OutputPins = s_Entry["out"].get_array();

        for (auto s_PinEntry : s_OutputPins) {
            std::string_view s_PinName = s_PinEntry["pin"].get_string();
            std::string_view s_Description = s_PinEntry["description"].get_string();

            s_Pins.m_OutputPins.push_back({std::string(s_PinName), std::string(s_Description)});
        }
    }

    for (auto& [s_ClassName, s_Pins] : s_Result) {
        std::sort(s_Pins.m_InputPins.begin(), s_Pins.m_InputPins.end(), [](const PinInfo& a, const PinInfo& b) { return a.m_Name < b.m_Name; });
        std::sort(s_Pins.m_OutputPins.begin(), s_Pins.m_OutputPins.end(), [](const PinInfo& a, const PinInfo& b) { return a.m_Name < b.m_Name; });
    }

    return s_Result;
}

Editor::QneTransform Editor::MatrixToQneTransform(const SMatrix& p_Matrix) {
    // This is adapted from QN: https://github.com/atampy25/quickentity-rs/blob/240ffba9d23dedc864bd39f1f029646837d3916d/src/lib.rs#L2528
    auto s_Trans = p_Matrix;

    constexpr float c_RAD2DEG = 180.0f / std::numbers::pi;

    const auto n11 = s_Trans.XAxis.x;
    const auto n12 = s_Trans.XAxis.y;
    const auto n13 = s_Trans.XAxis.z;
    const auto n14 = 0.0f;
    const auto n21 = s_Trans.YAxis.x;
    const auto n22 = s_Trans.YAxis.y;
    const auto n23 = s_Trans.YAxis.z;
    const auto n24 = 0.0f;
    const auto n31 = s_Trans.ZAxis.x;
    const auto n32 = s_Trans.ZAxis.y;
    const auto n33 = s_Trans.ZAxis.z;
    const auto n34 = 0.0f;
    const auto n41 = s_Trans.Trans.x;
    const auto n42 = s_Trans.Trans.y;
    const auto n43 = s_Trans.Trans.z;
    const auto n44 = 1.0f;

    const auto det = n41 * (n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34)
                     + n42 * (n11 * n23 * n34 - n11 * n24 * n33 + n14 * n21 * n33 - n13 * n21 * n34 + n13 * n24 * n31 - n14 * n23 * n31)
                     + n43 * (n11 * n24 * n32 - n11 * n22 * n34 - n14 * n21 * n32 + n12 * n21 * n34 + n14 * n22 * n31 - n12 * n24 * n31)
                     + n44 * (-n13 * n22 * n31 - n11 * n23 * n32 + n11 * n22 * n33 + n13 * n21 * n32 - n12 * n21 * n33 + n12 * n23 * n31);

    auto sx = n11 * n11 + n21 * n21 + n31 * n31;
    const auto sy = n12 * n12 + n22 * n22 + n32 * n32;
    const auto sz = n13 * n13 + n23 * n23 + n33 * n33;

    if (det < 0.0f) {
        sx = -sx;
    }

    const auto inv_sx = 1.0f / sx;
    const auto inv_sy = 1.0f / sy;
    const auto inv_sz = 1.0f / sz;

    s_Trans.XAxis.x *= inv_sx;
    s_Trans.YAxis.x *= inv_sx;
    s_Trans.ZAxis.x *= inv_sx;
    s_Trans.XAxis.y *= inv_sy;
    s_Trans.YAxis.y *= inv_sy;
    s_Trans.ZAxis.y *= inv_sy;
    s_Trans.XAxis.z *= inv_sz;
    s_Trans.YAxis.z *= inv_sz;
    s_Trans.ZAxis.z *= inv_sz;

    float s_RotationX = abs(s_Trans.XAxis.z) < 0.9999999f ? atan2f(-s_Trans.YAxis.z, s_Trans.ZAxis.z) * c_RAD2DEG
                                                          : atan2f(s_Trans.ZAxis.y, s_Trans.YAxis.y) * c_RAD2DEG;

    float s_RotationY = asinf(std::min(std::max(-1.f, s_Trans.XAxis.z), 1.f)) * c_RAD2DEG;

    float s_RotationZ = abs(s_Trans.XAxis.z) < 0.9999999f ? atan2f(-s_Trans.XAxis.y, s_Trans.XAxis.x) * c_RAD2DEG : 0.f;

    return QneTransform{
        .m_Position = {n41, n42, n43},
        .m_Rotation = {s_RotationX, s_RotationY, s_RotationZ},
        .m_Scale = {sx, sy, sz},
    };
}

SMatrix Editor::QneTransformToMatrix(const QneTransform& p_Transform) {
    // This is adapted from QN: https://github.com/atampy25/quickentity-rs/blob/240ffba9d23dedc864bd39f1f029646837d3916d/src/lib.rs#L2782
    constexpr float c_DEG2RAD = std::numbers::pi / 180.0f;

    const auto x = p_Transform.m_Rotation.x * c_DEG2RAD;
    const auto y = p_Transform.m_Rotation.y * c_DEG2RAD;
    const auto z = p_Transform.m_Rotation.z * c_DEG2RAD;

    const auto c1 = cosf(x / 2.0f);
    const auto c2 = cosf(y / 2.0f);
    const auto c3 = cosf(z / 2.0f);

    const auto s1 = sinf(x / 2.0f);
    const auto s2 = sinf(y / 2.0f);
    const auto s3 = sinf(z / 2.0f);

    const auto quat_x = s1 * c2 * c3 + c1 * s2 * s3;
    const auto quat_y = c1 * s2 * c3 - s1 * c2 * s3;
    const auto quat_z = c1 * c2 * s3 + s1 * s2 * c3;
    const auto quat_w = c1 * c2 * c3 - s1 * s2 * s3;

    const auto x2 = quat_x + quat_x;
    const auto y2 = quat_y + quat_y;
    const auto z2 = quat_z + quat_z;
    const auto xx = quat_x * x2;
    const auto xy = quat_x * y2;
    const auto xz = quat_x * z2;
    const auto yy = quat_y * y2;
    const auto yz = quat_y * z2;
    const auto zz = quat_z * z2;
    const auto wx = quat_w * x2;
    const auto wy = quat_w * y2;
    const auto wz = quat_w * z2;

    SMatrix s_Matrix;

    s_Matrix.XAxis.x = (1.0f - (yy + zz)) * p_Transform.m_Scale.x;
    s_Matrix.XAxis.y = (xy - wz) * p_Transform.m_Scale.y;
    s_Matrix.XAxis.z = (xz + wy) * p_Transform.m_Scale.z;

    s_Matrix.YAxis.x = (xy + wz) * p_Transform.m_Scale.x;
    s_Matrix.YAxis.y = (1.0f - (xx + zz)) * p_Transform.m_Scale.y;
    s_Matrix.YAxis.z = (yz - wx) * p_Transform.m_Scale.z;

    s_Matrix.ZAxis.x = (xz - wy) * p_Transform.m_Scale.x;
    s_Matrix.ZAxis.y = (yz + wx) * p_Transform.m_Scale.y;
    s_Matrix.ZAxis.z = (1.0f - (xx + yy)) * p_Transform.m_Scale.z;

    s_Matrix.Trans.x = p_Transform.m_Position.x;
    s_Matrix.Trans.y = p_Transform.m_Position.y;
    s_Matrix.Trans.z = p_Transform.m_Position.z;

    return s_Matrix;
}

void Editor::ActivateCamera(ZEntityRef p_CameraEntity) {
    TEntityRef<IRenderDestinationEntity> s_ActiveRenderDestination;
    SDK()->Globals()->CameraManagerMain->GetActiveRenderDestinationEntity(s_ActiveRenderDestination);

    m_OriginalCamera = s_ActiveRenderDestination.m_pInterfaceRef->GetSource();

    s_ActiveRenderDestination.m_pInterfaceRef->SetSource(p_CameraEntity);
}

void Editor::DeactivateCamera() {
    TEntityRef<IRenderDestinationEntity> s_ActiveRenderDestination;
    SDK()->Globals()->CameraManagerMain->GetActiveRenderDestinationEntity(s_ActiveRenderDestination);

    s_ActiveRenderDestination.m_pInterfaceRef->SetSource(m_OriginalCamera);
}

std::string Editor::FormatFloat(float p_Value, bool p_Round, uint32_t p_Decimals) {
    if (p_Round) {
        return fmt::format("{:.{}f}", p_Value, p_Decimals);
    }

    return fmt::format("{}", p_Value);
}

void Editor::CopyToClipboard(std::string_view p_String) {
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

    memcpy(s_GlobalDataPtr, p_String.data(), p_String.size());
    static_cast<char*>(s_GlobalDataPtr)[p_String.size()] = '\0';

    GlobalUnlock(s_GlobalData);

    if (!SetClipboardData(CF_TEXT, s_GlobalData)) {
        GlobalFree(s_GlobalData);
    }

    CloseClipboard();
}

bool Editor::ImGuiCopyWidget(std::string_view p_Id) {
    ImGui::SameLine(0, 10.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.5, 0.5});
    ImGui::SetWindowFontScale(0.6);

    const auto s_Label = fmt::format("{}##{}", ICON_MD_CONTENT_COPY, p_Id);

    const auto s_Result = ImGui::ButtonEx(s_Label.c_str(), {m_CopyWidgetButtonSize, m_CopyWidgetButtonSize}, ImGuiButtonFlags_AlignTextBaseLine);

    ImGui::SetWindowFontScale(1.0);
    ImGui::PopStyleVar(2);

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Copy to clipboard");
    }

    return s_Result;
}

void Editor::DrawSettingsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) {
    if (!m_ShowSettingsWindow || !p_HasFocus) {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    ImGui::PushFont(p_Renderer->GetRegularFont());

    if (ImGui::Begin(ICON_MD_VIDEO_SETTINGS " EDITOR", &m_ShowSettingsWindow)) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y), ImGuiCond_FirstUseEver);

        if (ImGui::Checkbox(ICON_MD_VIDEO_SETTINGS "  SHOW ENTITY EDITOR", &m_ShowEntityEditor)) {
            SetSettingBool("general", "entity_editor_visible", m_ShowEntityEditor);
        }

        bool s_ServerEnabled = m_Server.GetEnabled();

        if (ImGui::Checkbox(ICON_MD_TERMINAL "  ENABLE EDITOR SERVER", &s_ServerEnabled)) {
            ToggleEditorServerEnabled();
        }

        ImGui::Spacing();
        ImGui::Text("Entity highlight mode");

        const int32_t s_EntityHighlightMode = static_cast<int32_t>(m_EntityHighlightMode);

        if (ImGui::RadioButton("Lines", s_EntityHighlightMode == 0)) {
            m_EntityHighlightMode = EntityHighlightMode::Lines;
        }

        ImGui::SameLine();

        if (ImGui::RadioButton("Lines and rectangles", s_EntityHighlightMode == 1)) {
            m_EntityHighlightMode = EntityHighlightMode::LinesAndTriangles;
        }

        ImGui::Checkbox("Raycast logging", &m_RaycastLogging);
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Editor::ToggleEditorServerEnabled() {
    EditorServer::SetEnabled(!EditorServer::GetEnabled());
}

void Editor::QueueTask(std::function<void()> p_Task) {
    std::lock_guard lock(m_TaskMutex);
    m_TaskQueue.push_back(std::move(p_Task));
}

void Editor::ProcessTasks() {
    std::vector<std::function<void()>> s_TasksToRun;

    {
        std::lock_guard lock(m_TaskMutex);

        if (m_TaskQueue.empty()) {
            return;
        }

        s_TasksToRun.swap(m_TaskQueue);
    }

    for (const auto& s_Task : s_TasksToRun) {
        s_Task();
    }
}

DEFINE_PLUGIN_DETOUR(Editor, void, OnCreateScene, ZEntitySceneContext* th) {
    if (th->m_SceneConfig) {
        std::vector<ZRuntimeResourceID> s_Bricks;
        const auto& s_MainBrickRIDs = th->m_SceneConfig->m_aMainBrickFactoryRIDs;

        s_Bricks.reserve(s_MainBrickRIDs.size());

        for (const auto& s_BrickRID : s_MainBrickRIDs) {
            s_Bricks.push_back(s_BrickRID);
        }

        m_Server.OnSceneCreating(s_Bricks);
    }

    return {HookAction::Continue()};
}

DEFINE_PLUGIN_DETOUR(Editor, void, OnClearScene, ZEntitySceneContext* th, bool p_FullyUnloadScene) {
    m_SelectedEntity = {};
    m_ScrollToEntity = false;

    {
        std::scoped_lock s_Lock(m_EntityDestructionMutex);
        m_EntitiesToDestroy.clear();
    }

    ClearFilters();

    {
        std::scoped_lock s_Lock(m_CachedEntityTreeMutex);
        m_CachedEntityTree.reset();
        m_CachedEntityTreeMap.clear();
        m_OpenEntityTreeNodes.clear();
        m_EntityNames.clear();
    }

    {
        std::scoped_lock s_Lock(m_CachedEntityTreeMutex);
        m_OpenEntityTreeNodes.clear();
    }

    std::unordered_map<uint64_t, ZEntityRef> s_SpawnedEntitiesToDestroy;
    {
        std::scoped_lock s_Lock(m_CachedEntityTreeMutex);
        s_SpawnedEntitiesToDestroy.swap(m_SpawnedEntities);
    }

    for (const auto& s_Entity : s_SpawnedEntitiesToDestroy | std::views::values) {
        if (s_Entity) {
            SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, s_Entity);
        }
    }

    {
        std::scoped_lock s_Lock(m_PendingDynamicEntitiesMutex);
        m_PendingDynamicEntities.clear();
    }

    m_Server.OnSceneClearing(p_FullyUnloadScene);

    m_SelectedHumanoid = {};

    m_SelectedItem = {};
    m_SelectedItemRootEntity = {};

    m_SelectedDebugEntity = nullptr;

    {
        std::scoped_lock s_Lock(m_DebugEntitiesMutex);
        m_EntityRefToDebugEntities.clear();
    }

    m_EntityRefToFactoryRuntimeResourceIDs.clear();

    m_InputPinValue.Clear();
    m_OutputPinValue.Clear();

    ClearBoxReflectionPreview();

    return {HookAction::Continue()};
}

DEFINE_PLUGIN_DETOUR(
    Editor, void, ZTemplateEntityFactory_ConfigureEntity, ZTemplateEntityFactory* th, ZEntityType** pEntity, void* unk0, void* unk1, void* unk2
) {
    IEntityBlueprintFactory* s_EntityBlueprintFactory = static_cast<IEntityBlueprintFactory*>(th->m_blueprintResource.GetResourceData());

    for (size_t i = 0; i < s_EntityBlueprintFactory->GetSubEntitiesCount(); ++i) {
        const ZEntityRef s_SubEntity = s_EntityBlueprintFactory->GetSubEntity(pEntity, i);
        IEntityFactory* s_SubEntityFactory = th->m_pFactories[i];
        ZRuntimeResourceID s_SubEntityFactoryRuntimeResourceID;

        if (s_SubEntityFactory->IsTemplateEntityFactory()) {
            s_SubEntityFactoryRuntimeResourceID = static_cast<ZTemplateEntityFactory*>(s_SubEntityFactory)->m_ridResource;
        }
        else if (s_SubEntityFactory->IsAspectEntityFactory()) {
            s_SubEntityFactoryRuntimeResourceID = static_cast<ZAspectEntityFactory*>(s_SubEntityFactory)->m_ridResource;
        }
        else if (s_SubEntityFactory->IsCppEntityFactory()) {
            s_SubEntityFactoryRuntimeResourceID = static_cast<ZCppEntityFactory*>(s_SubEntityFactory)->m_ridResource;
        }
        else if (s_SubEntityFactory->IsExtendedCppEntityFactory()) {
            ZExtendedCppEntityFactory* s_ExtendedCppEntityFactory = static_cast<ZExtendedCppEntityFactory*>(s_SubEntityFactory);
            const auto& s_BlueprintResourceInfo = s_ExtendedCppEntityFactory->m_pBlueprintFactory.GetResourceInfo();

            for (const auto& s_ResourceInfo : (*SDK()->Globals()->ResourceContainer)->m_resources) {
                if (s_ResourceInfo.resourceType == 'ECPT') {
                    const uint32_t s_ReferenceIndex =
                        (*SDK()->Globals()->ResourceContainer)->m_references[s_ResourceInfo.firstReferenceIndex + 1].index;
                    const auto& s_ReferenceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[s_ReferenceIndex];

                    if (s_ReferenceInfo.rid == s_BlueprintResourceInfo.rid) {
                        s_SubEntityFactoryRuntimeResourceID = s_ResourceInfo.rid;
                        break;
                    }
                }
            }
        }
        else if (s_SubEntityFactory->IsUIControlEntityFactory()) {
            s_SubEntityFactoryRuntimeResourceID = static_cast<ZUIControlEntityFactory*>(s_SubEntityFactory)->m_ridResource;
        }
        else if (s_SubEntityFactory->IsRenderMaterialEntityFactory()) {
            s_SubEntityFactoryRuntimeResourceID = static_cast<ZRenderMaterialEntityFactory*>(s_SubEntityFactory)->m_ridResource;
        }
        else if (s_SubEntityFactory->IsAudioSwitchEntityFactory()) {
            s_SubEntityFactoryRuntimeResourceID = static_cast<ZAudioSwitchEntityFactory*>(s_SubEntityFactory)->m_ridResource;
        }
        else if (s_SubEntityFactory->IsAudioStateEntityFactory()) {
            s_SubEntityFactoryRuntimeResourceID = static_cast<ZAudioStateEntityFactory*>(s_SubEntityFactory)->m_ridResource;
        }
        else if (s_SubEntityFactory->IsPadEntityFactory()) {
            ZPadEntityFactory* s_PadEntityFactory = static_cast<ZPadEntityFactory*>(s_SubEntityFactory);
            const auto& s_BlueprintResourceInfo = s_PadEntityFactory->m_pBlueprintResource.GetResourceInfo();

            for (const auto& s_ResourceInfo : (*SDK()->Globals()->ResourceContainer)->m_resources) {
                if (s_ResourceInfo.resourceType == 'PETY') {
                    const uint32_t s_ReferenceIndex =
                        (*SDK()->Globals()->ResourceContainer)->m_references[s_ResourceInfo.firstReferenceIndex + 1].index;
                    const auto& s_ReferenceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[s_ReferenceIndex];

                    if (s_ReferenceInfo.rid == s_BlueprintResourceInfo.rid) {
                        s_SubEntityFactoryRuntimeResourceID = s_ResourceInfo.rid;
                        break;
                    }
                }
            }
        }
        else if (s_SubEntityFactory->IsShadernodeEntityFactory()) {
            ZShadernodeEntityFactory* s_ShadernodeEntityFactory = static_cast<ZShadernodeEntityFactory*>(s_SubEntityFactory);
            const auto& s_BlueprintResourceInfo = s_ShadernodeEntityFactory->m_pBlueprintResource.GetResourceInfo();

            for (const auto& s_ResourceInfo : (*SDK()->Globals()->ResourceContainer)->m_resources) {
                if (s_ResourceInfo.resourceType == 'SNET') {
                    const uint32_t s_ReferenceIndex =
                        (*SDK()->Globals()->ResourceContainer)->m_references[s_ResourceInfo.firstReferenceIndex + 1].index;
                    const auto& s_ReferenceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[s_ReferenceIndex];

                    if (s_ReferenceInfo.rid == s_BlueprintResourceInfo.rid) {
                        s_SubEntityFactoryRuntimeResourceID = s_ResourceInfo.rid;
                        break;
                    }
                }
            }
        }

        {
            std::unique_lock s_Lock(m_EntityRefToFactoryRuntimeResourceIDsMutex);

            if (!m_EntityRefToFactoryRuntimeResourceIDs.contains(s_SubEntity)) {
                m_EntityRefToFactoryRuntimeResourceIDs[s_SubEntity] = {s_SubEntityFactoryRuntimeResourceID, th->m_ridResource};
            }
        }
    }

    return {HookAction::Continue()};
}

DEFINE_ZKNT_PLUGIN(Editor)
