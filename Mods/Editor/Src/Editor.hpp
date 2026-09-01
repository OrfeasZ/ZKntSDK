#pragma once

#include <WinSock2.h>

#include <shared_mutex>
#include <unordered_set>

#include <directx/d3dx12.h>

#include <DirectXTex.h>

#include <imgui.h>
#include "ImGuizmo.h"

#include <Glacier/ZCamera.hpp>
#include <Glacier/ZCurve.hpp>
#include <Glacier/ZPlayer.hpp>
#include <Glacier/ZItem.hpp>
#include <Glacier/EDebugChannel.hpp>
#include <Glacier/ZDebugGizmoEntity.hpp>

#include "IPluginInterface.hpp"
#include "EntityTreeNode.hpp"
#include "Util/D3DUtils.hpp"
#include "EditorServer.hpp"

class ZCLSetHumanoidOutfitEntity;
class ZCLSetHumanoidHealth;
class ZCLDisarmNpcHumanoid;

class Editor : public zknt::IPluginInterface {
  public:
    Editor();
    ~Editor() override;

    void Init() override;
    void OnEngineInitialized() override;
    void OnDrawMenu(zknt::IImGuiRenderer* p_Renderer) override;
    void OnDrawUI(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus) override;
    void OnDraw3D(zknt::IDirectXTKRenderer* p_Renderer) override;
    void OnDepthDraw3D(zknt::IDirectXTKRenderer* p_Renderer) override;

  private:
    struct QneTransform {
        SVector3 m_Position;
        SVector3 m_Rotation;
        SVector3 m_Scale;
    };

    bool SpawnEntities();

    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

#pragma region Entity Tree

    enum EntityViewMode {
        All,
        ScenesAndBricks,
        DynamicEntities,
    };

    void DrawEntityTreeWindow();
    void DrawEntity(std::shared_ptr<EntityTreeNode> p_Node);

    bool HasVisibleChildren(const std::shared_ptr<EntityTreeNode>& p_Node) const;

    void FilterEntityTree();
    bool FilterEntityTree(EntityTreeNode* p_Node, EntityTreeNode*& p_OutSingleMatchNode);
    void ClearFilters();
    std::shared_ptr<EntityTreeNode> FindMatchByIndex(const std::shared_ptr<EntityTreeNode>& p_Node, size_t p_TargetIndex, size_t& p_CurrentCounter);

    void UpdateEntities();
    void UpdateEntityTree(
        std::unordered_map<ZEntityRef, std::shared_ptr<EntityTreeNode>>& p_NodeMap,
        const std::vector<std::pair<ZEntityRef, IEntityBlueprintFactory*>>& p_Entities, bool p_AreBricksDynamic, bool p_AreEntitiesDynamic
    );
    void AddDynamicEntitiesToEntityTree(
        const std::shared_ptr<EntityTreeNode>& p_SceneNode, std::unordered_map<ZEntityRef, std::shared_ptr<EntityTreeNode>>& p_NodeMap
    );

    bool IsSpecialEntityTreeNode(ZEntityRef p_Entity) const;

    static std::shared_ptr<EntityTreeNode> GetRootEntity(std::shared_ptr<EntityTreeNode> p_Node);

  public:
    bool IsEditorEntity(const ZEntityRef& p_Entity) const;

  private:
    std::atomic_bool m_IsBuildingEntityTree = false;
    std::shared_mutex m_CachedEntityTreeMutex;
    std::unordered_map<ZEntityRef, std::shared_ptr<EntityTreeNode>> m_CachedEntityTreeMap;
    std::shared_ptr<EntityTreeNode> m_CachedEntityTree;
    std::unordered_set<EntityTreeNode*> m_OpenEntityTreeNodes;

    std::unordered_map<uint64_t, ZEntityRef> m_SpawnedEntities;
    std::unordered_map<ZEntityRef, std::string> m_EntityNames;

    std::mutex m_EntityDestructionMutex;
    std::vector<std::tuple<ZEntityRef, std::optional<std::string>>> m_EntitiesToDestroy;

    std::mutex m_PendingDynamicEntitiesMutex;
    std::unordered_map<ZEntityRef, IEntityBlueprintFactory*> m_PendingDynamicEntities;
    std::mutex m_PendingDynamicBricksMutex;
    std::unordered_map<ZEntityRef, IEntityBlueprintFactory*> m_PendingDynamicBricks;
    std::mutex m_PendingNodeDeletionsMutex;
    std::vector<std::weak_ptr<EntityTreeNode>> m_PendingNodeDeletions;
    std::unordered_map<uint64_t, std::string> m_EntityIDToUUID;

    EntityViewMode m_EntityViewMode = EntityViewMode::All;
    EntityViewMode m_LastEntityViewMode = EntityViewMode::All;
    const std::vector<std::string> m_EntityViewModes = {"All", "Scenes/Bricks", "Dynamic Entities"};

    std::string m_EntityIDSearchInput;
    std::string m_EntityTypeSearchInput;
    std::string m_EntityNameSearchInput;
    std::unordered_set<EntityTreeNode*> m_FilteredEntityTreeNodes;
    size_t m_CurrentEntitySearchResultIndex = 0;
    size_t m_TotalMatchCount = 0;
    bool m_HasActiveFilters = false;
    bool m_HasActiveSearch = false;

    inline static ZEntityRef m_DynamicBricksNodeEntityRef = ZEntityRef(reinterpret_cast<ZEntityType**>(0x1), -1, 0);
    inline static ZEntityRef m_DynamicEntitiesNodeEntityRef = ZEntityRef(reinterpret_cast<ZEntityType**>(0x2), -1, 0);
    inline static ZEntityRef m_UnparentedEntitiesNodeEntityRef = ZEntityRef(reinterpret_cast<ZEntityType**>(0x3), -1, 0);

#pragma endregion

#pragma region Entity Events

    void OnSelectEntity(ZEntityRef p_Entity, bool p_ShouldScrollToEntity, std::optional<std::string> p_ClientId);
    void OnDestroyEntity(ZEntityRef p_Entity, std::optional<std::string> p_ClientId);
    void OnEntityTransformChange(ZEntityRef p_Entity, SMatrix p_Transform, bool p_Relative, std::optional<std::string> p_ClientId);
    void OnEntityNameChange(ZEntityRef p_Entity, const std::string& p_Name, std::optional<std::string> p_ClientId);
    void OnSetPropertyValue(ZEntityRef p_Entity, uint32_t p_PropertyId, const ZObjectRef& p_Value, std::optional<std::string> p_ClientId);
    void OnSignalEntityPin(ZEntityRef p_Entity, const std::string& p_Pin, bool p_Output, const ZObjectRef& p_Data = ZObjectRef());
    void OnSignalEntityPin(ZEntityRef p_Entity, uint32_t p_PinId, bool p_Output, const ZObjectRef& p_Data = ZObjectRef());

#pragma endregion

#pragma region Entity Destruction

    void DestroyEntityInternal(ZEntityRef p_Entity, std::optional<std::string> p_ClientId);
    void DestroyEntityNodeInternal(const std::shared_ptr<EntityTreeNode>& p_NodeToRemove, std::optional<std::string> p_ClientId);

#pragma endregion

#pragma region Entity Selection

    enum class EntityHighlightMode {
        Lines,
        LinesAndTriangles,
    };

    void CreateSelectionForFreeCameraEditorStyleEntity();

    void OnMouseDown(const SVector2& p_Position, bool p_IsFirstClick);

    float4 m_From;
    float4 m_To;
    float4 m_Hit;
    float4 m_Normal;

    bool m_RaycastLogging;

    ZEntityRef m_SelectedEntity;
    bool m_ScrollToEntity = false;

    EntityHighlightMode m_EntityHighlightMode = EntityHighlightMode::Lines;

    TEntityRef<ZSelectionForFreeCameraEditorStyleEntity> m_SelectionForFreeCameraEditorStyleEntity;

    TEntityRef<ZCLGetOutfitRefFromOutfit> m_OutfitRefGetter;
    TEntityRef<ZCLGetHumanoidFromOutfitEntity> m_HumanoidGetter;

#pragma endregion

#pragma region Entity Manipulation

    void DrawEntityManipulator(bool p_HasFocus);

    ImGuizmo::OPERATION m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::MODE m_GizmoSpace = ImGuizmo::MODE::WORLD;

    bool m_HoldingMouse = false;

    bool m_UseSnap = false;
    bool m_UseAngleSnap = false;
    bool m_UseScaleSnap = false;

    bool m_UseQneTransforms = false;

    double m_SnapValue = 1.0;
    double m_AngleSnapValue = 90.0;
    double m_ScaleSnapValue = 1.0;

#pragma endregion

    void DrawEntityAABB(zknt::IDirectXTKRenderer* p_Renderer);

#pragma region Entity Properties

    void DrawEntityPropertiesWindow(zknt::IImGuiRenderer* p_Renderer);
    bool DrawEntityPropertyValue(
        zknt::IImGuiRenderer* p_Renderer, const std::string& p_Id, const std::string& p_PropertyName, const std::string& p_TypeName,
        const STypeID* p_TypeID, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data
    );
    bool DrawArrayElements(
        zknt::IImGuiRenderer* p_Renderer, const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data,
        const IArrayType* p_ArrayType
    );

    void DrawEntityPinValue(const std::string& p_Id, const std::string& p_TypeName, void* p_Data);

    void UnsupportedProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);

    void ZEntityRefProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    void TEntityRefProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    void EntityRefProperty(const std::string& p_Id, ZEntityRef p_Entity);

    void ZRepositoryIDProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    void ZGuidProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);

    bool StringProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool BoolProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Uint8Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Uint16Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Uint32Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Uint64Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Int8Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Int16Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Int32Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Int64Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Float32Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool Float64Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool EnumProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);

    bool SVector2Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool SVector3Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool SVector4Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);

    bool SMatrix43Property(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);

    bool SColorRGBProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    bool SColorRGBAProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);

    void ResourcePtrProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);
    void ZRuntimeResourceIDProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);

    bool ArrayProperty(
        zknt::IImGuiRenderer* p_Renderer, const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data,
        const std::string& s_PropertyName, const STypeID* p_TypeID
    );

    bool ZGameTimeProperty(const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data);

    bool ZCurveProperty(
        zknt::IImGuiRenderer* p_Renderer, const std::string& p_Id, ZEntityRef p_Entity, SPropertyData* p_Property, void* p_Data,
        const std::string& s_PropertyName, const STypeID* p_TypeID
    );
    static void PlotZCurve(const ZCurve* p_Curve);
    static float EvaluateZCurveSegment(const TFixedArray<float, 8>& p_Key, const TFixedArray<float, 8>& p_NextKey, float p_Time);

    std::unordered_map<ZEntityRef, std::pair<ZRuntimeResourceID, ZRuntimeResourceID>> m_EntityRefToFactoryRuntimeResourceIDs;
    std::shared_mutex m_EntityRefToFactoryRuntimeResourceIDsMutex;
    std::mutex m_ExtendedCppEntityFactoryResourceMapsMutex;

    bool m_CameraActive = false;
    ZEntityRef m_OriginalCamera;

#pragma endregion

#pragma region Entity Pins

    struct PinInfo {
        std::string m_Name;
        std::string m_Description;
    };

    struct PinLists {
        std::vector<PinInfo> m_InputPins;
        std::vector<PinInfo> m_OutputPins;
    };

    std::vector<PinInfo> GetPins(ZEntityRef p_EntityRef, bool p_OutputPins);

    std::map<std::string, PinLists> ParsePinsJson(const std::string& p_PinsJson);

    std::map<std::string, PinLists> m_ClassToInputAndOutputPins;
    std::vector<std::pair<std::string, STypeID*>> m_PinDataTypes;
    ZObjectRef m_InputPinValue;
    ZObjectRef m_OutputPinValue;

    std::vector<std::string> m_ClassNames;

#pragma endregion

#pragma region Transform Conversion

    static QneTransform MatrixToQneTransform(const SMatrix& p_Matrix);
    static SMatrix QneTransformToMatrix(const QneTransform& p_Transform);

#pragma endregion

#pragma region Camera

    void ActivateCamera(ZEntityRef p_CameraEntity);
    void DeactivateCamera();

#pragma endregion

#pragma region Formatting

    static std::string FormatFloat(float p_Value, bool p_Round, uint32_t p_Decimals);

#pragma endregion

#pragma region Clipboard

    static void CopyToClipboard(std::string_view p_String);
    static bool ImGuiCopyWidget(std::string_view p_Id);

    static std::string FormatZCurveForClipboard(ZCurve* p_Curve, void* p_Data, const IArrayType* p_ArrayType);

    bool m_RoundCopiedMatrixValues = false;
    int32_t m_CopyDecimalPlaces = 3;

    static constexpr float m_CopyWidgetButtonSize = 20.f;
    static constexpr float m_CopyWidgetSpacing = 10.f;
    static constexpr float m_CopyWidgetWidth = m_CopyWidgetButtonSize + m_CopyWidgetSpacing;

#pragma endregion

#pragma region Humanoids

    struct OutfitInfo {
        std::vector<std::pair<std::string, size_t>> m_Variations;
        ZRuntimeResourceID m_OutfitSet;
    };

    enum class EAliveFilter {
        Any,
        Alive,
        Dead,
    };

    void DrawHumanoidsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus);

    void LoadAllOutfitSets();
    void SetHumanoidOutfit(ZHumanoidCharacterEntity* p_HumanoidCharacterEntity, const ZRuntimeResourceID& p_OutfitSet, size_t p_OutfitVariationIndex);

    void SetHumanoidHealth(ZEntityRef p_Humanoid, float p_Health);
    void SetHumanoidImmuneToDamage(ZEntityRef p_Humanoid, bool p_Invulnerable);

    void TeleportHumanoid(ZEntityRef p_Humanoid, ZSpatialEntity* p_Target);

    void DisarmHumanoid(ZEntityRef p_Humanoid, EFirearmDisarmType p_FirearmDisarmType);

    static ZItemCharacterEntityBase* GetEquippedItem(ZHumanoidCharacterEntity* p_HumanoidCharacterEntity);

    bool MatchesHumanoidFilters(ZEntityRef p_HumanoidEntity, const std::string& p_HumanoidName);

    bool m_ShowHumanoidsWindow = false;

    TInterfaceRef<ZHumanoidCharacterEntity> m_SelectedHumanoid;
    bool m_SelectHumanoidOnMouseClick = false;
    bool m_ScrollToHumanoid = false;
    char m_HumanoidEntityName[2048]{""};

    TEntityRef<ZCLTeleportHumanoidEntity> m_HumanoidTeleporter;
    TEntityRef<ZSpatialEntity> m_TeleportTarget;

    TEntityRef<ZCLValueEntityRefEntity> m_TargetEntityRefValue;
    TEntityRef<ZCLGetHumanoidRefFromEntityRef> m_HumanoidRefFromEntityRefGetter;

    TEntityRef<ZCLSetHumanoidOutfitEntity> m_HumanoidOutfitSetter;
    std::map<std::string, OutfitInfo> m_OutfitSetToOutfitInfo;
    std::unordered_map<ZRuntimeResourceID, std::string> m_OutfitSetRuntimeResourceIDToName;

    TEntityRef<ZCLGetHumanoidHealth> m_HumanoidHealthGetter;
    TEntityRef<ZCLGetHumanoidMaxHealth> m_HumanoidMaxHealthGetter;
    TEntityRef<ZCLSetHumanoidHealth> m_HumanoidHealthSetter;
    TEntityRef<ZCLValueFloatEntity> m_HealthAmmountFloatValue;
    TEntityRef<ZCLValueBoolEntity> m_InterruptPassiveRegenerationBoolValue;
    TEntityRef<ZCLSetHumanoidImmuneToDamage> m_HumanoidImmuneToDamageSetter;
    TEntityRef<ZCLValueBoolEntity> m_InvulnerableBoolValue;
    std::unordered_map<uint64_t, bool> m_HumanoidEntityIDToInvincibility;

    TEntityRef<ZCLDisarmNpcHumanoid> m_HumanoidDisarmer;

    TEntityRef<ZCLIsHumanoidAlive> m_IsHumanoidAliveGetter;
    EAliveFilter m_AliveFilter = EAliveFilter::Any;

    bool m_IsGamePlayTagFilteringEnabled = true;
    bool m_FilterByType = true;
    bool m_FilterByArchetype = true;

    bool m_ShowEnemies = true;
    bool m_ShowCompanions = true;
    bool m_ShowAuthorities = true;
    bool m_ShowCivilians = true;
    bool m_ShowAllies = true;

    bool m_ShowGrunts = true;
    bool m_ShowMercs = true;
    bool m_ShowArmoredMercs = true;
    bool m_ShowSpecialists = true;
    bool m_ShowLeaders = true;
    bool m_ShowSnipers = true;
    bool m_ShowTanks = true;
    bool m_ShowBrutes = true;
    bool m_ShowAndroids = true;

#pragma endregion

#pragma region Items

    void DrawItemsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus);

    void DrawItemInfo(ZItemCharacterEntityBase* p_Item, ZEntityRef p_ItemRootEntity, bool p_ShowOwnerEntityName);

    ZItemCharacterDefinitionBase* GetItemDefinition(ZItemCharacterEntityBase* p_Item);

    static const char* ItemTypeToString(EItemType p_ItemType);

    bool MatchesItemFilters(ZItemCharacterEntityBase* p_Item, const std::string& p_ItemName);

    bool m_ShowItemsWindow = false;

    TInterfaceRef<ZItemCharacterEntityBase> m_SelectedItem;
    ZEntityRef m_SelectedItemRootEntity;
    bool m_SelectItemOnMouseClick = false;
    bool m_ScrollToItem = false;
    char m_ItemEntityName[2048]{""};

    bool m_FilterByItemType = false;

    bool m_ShowNoItems = true;
    bool m_ShowHandguns = true;
    bool m_ShowTasers = true;
    bool m_ShowDartGuns = true;
    bool m_ShowSubmachineGuns = true;
    bool m_ShowAssaultRifles = true;
    bool m_ShowShotguns = true;
    bool m_ShowSmokePellets = true;
    bool m_ShowBlastDevices = true;
    bool m_ShowBricks = true;
    bool m_ShowVases = true;
    bool m_ShowBusts = true;
    bool m_ShowUndefinedItems = true;
    bool m_ShowLongRifles = true;
    bool m_ShowCosmetics = true;

#pragma endregion

#pragma region Rooms

    void DrawRoomsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus);

    bool m_ShowRoomsWindow = false;

    bool m_ShowOnlyVisibleRooms = false;
    bool m_ShowOnlyVisibleGates = false;

#pragma endregion

#pragma region Box reflections

    struct BoxReflectionPreview {
        int32_t m_BoxReflectionId = SIZE_MAX;

        std::array<ScopedD3DRef<ID3D12Resource>, 6> m_Textures;
        std::array<zknt::ImGuiTexture, 6> m_ImGuiTextures;

        std::array<ScopedD3DRef<ID3D12Resource>, 6> m_DiffuseTextures;
        std::array<zknt::ImGuiTexture, 6> m_DiffuseImGuiTextures;
    };

    void DrawBoxReflectionsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus);

    bool UpdateBoxReflectionPreview(ZRenderGraphNodeBoxReflection* p_BoxReflectionGraphNode);
    bool UpdateBoxReflectionCubemapPreview(
        ZRenderTexture2D* p_SourceTexture, uint32_t p_CubeIndex, std::array<ScopedD3DRef<ID3D12Resource>, 6>& p_OutTextures,
        std::array<zknt::ImGuiTexture, 6>& p_OutImGuiTextures
    );
    void ClearBoxReflectionPreview();
    void DrawBoxReflectionCross(const std::array<zknt::ImGuiTexture, 6>& p_Textures, float p_FaceSize);

    static bool
    CreateBoxReflectionFaceTexture(const DirectX::Image& p_Image, ScopedD3DRef<ID3D12Resource>& p_OutTexture, zknt::ImGuiTexture& p_OutImGuiTexture);

    static bool ExportAllBoxReflectionCubemaps(const std::filesystem::path& p_OutputFolder, bool p_Diffuse);
    static bool
    ExportBoxReflectionCubemap(const DirectX::ScratchImage& p_CapturedImage, uint32_t p_CubeIndex, const std::filesystem::path& p_OutputFilePath);
    static bool ExportBoxReflectionCubemap(ID3D12Resource* p_Resource, uint32_t p_CubeIndex, const std::filesystem::path& p_OutputFilePath);
    static bool GetBoxReflectionTexture(
        ZRenderGraphNodeBoxReflection* p_BoxReflectionGraphNode, bool p_Diffuse, ZRenderTexture2D*& p_OutTexture, uint32_t& p_OutCubeIndex
    );
    static std::filesystem::path
    GetBoxReflectionExportPath(const ZRenderGraphNodeBoxReflection* p_BoxReflection, const std::filesystem::path& p_OutputFolder, bool p_Diffuse);

    bool GenerateBoxReflectionCacheResources(const std::filesystem::path& p_OutputFolder);
    bool GenerateBoxReflectionCacheResource(const ZRenderGraphNodeBoxReflection* p_BoxReflection, const std::filesystem::path& p_OutputFolder);
    bool CaptureBoxReflectionTextures(uint32_t p_Chunk, DirectX::ScratchImage& p_OutBC6HImage, DirectX::ScratchImage& p_OutR11G11B10Image);
    bool WriteBoxReflectionCacheResource(
        const ZRenderGraphNodeBoxReflection* p_BoxReflection, uint32_t p_CubeIndex, const DirectX::ScratchImage& p_BC6HImage,
        const DirectX::ScratchImage& p_R11G11B10Image, const std::filesystem::path& p_OutputFilePath
    );
    static size_t CalculateCubemapSize(const DirectX::ScratchImage& p_Image, uint32_t p_CubeIndex);

    bool m_ShowBoxReflectionsWindow = false;

    ZRenderGraphNodeBoxReflection* m_SelectedBoxReflectionGraphNode = nullptr;
    BoxReflectionPreview m_BoxReflectionPreview;

    std::string m_BoxReflectionOutputFolder;

#pragma endregion

#pragma region Debug channels

    enum DebugEntityTypeName {
        PFBoxEntity,
        PFObstacleEntity,
        PFStaticObstacleBoxEntity,
        PFSeedPoint,
        PathfinderConfigurationBase,
        DebugGizmoEntity,
        DebugPOIEntity,
        DebugTextEntity,
        LightEntity,
        DarkLightEntity,
        BoxReflectionEntity,
        CubemapProbeEntity,
        ParticleEmitterBoxEntity,
        ParticleEmitterEmitterEntity,
        ParticleEmitterMeshEntity,
        ParticleEmitterPointEntity,
        ParticleGlobalAttractorEntity,
        ParticleKillVolumeEntity,
        GateEntity,
        OccluderEntity,
        DecalsSpawnEntity,
        StaticDecalEntity,
        CrowdActorGroupEntity,
        CrowdActorGroupFocalPointEntity,
        CrowdEntity,
        ManualActorEntity,
        SplineCrowdFlowEntity,
        HeightStampEntity,
        BoxShapeAspect,
        CapsuleShapeAspect,
        SphereShapeAspect,
        WindEntity,
        AISoundConnector,
        PostfilterAreaBoxEntity,
        CheckpointEntity,
        KntSpawnpointEntity,
        DynamicGameplaySpawnerEntryEntity,
        HumanoidSequenceSpatial,
        CameraEntity,
        GameCameraEntityBase,
        BoxVolumeEntity,
        SphereVolumeEntity,
        CapsuleVolumeEntity,
        VolumeShapeEntity,
        OrientationEntity,
        ScatterContainerEntity,
        TrailShapeEntity,
        SplineEntity,
        SplineControlPointEntity,
        AudioEmitterSpatialAspect,
        AudioEmitterVolumetricAspect,
        ClothWireEntity,
        Count
    };

    struct DebugEntity {
        std::string m_TypeName;
        ZEntityRef m_EntityRef;
        EDebugChannel m_DebugChannel;
        bool m_HasGizmo;
    };

    struct GizmoEntity : DebugEntity {
        ZRuntimeResourceID m_RuntimeResourceID;
        ZResourcePtr m_PrimResourcePtr;
        SVector4 m_Color;
        SMatrix m_Transform;
    };

    enum class DebugDrawMode {
        SelectedChannelsAndTypes,
        SelectedEntity,
        All,
    };

    struct RayCastResult {
        ZEntityRef m_Entity;
        std::string m_TypeName;
        EDebugChannel m_DebugChannel;
        float m_Distance = FLT_MAX;
    };

    void DrawDebugChannelsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus);

    void InitializeDebugChannels();
    void InitializeDebugEntityTypeIDs();

    void DrawDebugEntities(zknt::IDirectXTKRenderer* p_Renderer);
    void DrawGizmo(GizmoEntity& p_GizmoEntity, zknt::IDirectXTKRenderer* p_Renderer);
    void DrawShapes(const DebugEntity& p_DebugEntity, zknt::IDirectXTKRenderer* p_Renderer);

    void GetDebugEntities(const std::shared_ptr<EntityTreeNode>& p_EntityTreeNode);
    void AddDebugEntity(const ZEntityRef p_EntityRef, const std::string& p_TypeName, const EDebugChannel p_DebugChannel);
    void AddGizmoEntity(
        const ZEntityRef p_EntityRef, const std::string& p_TypeName, const EDebugChannel p_DebugChannel, const ZRuntimeResourceID p_RuntimeResourceID,
        const SVector4& p_Color = SVector4(1.f, 1.f, 1.f, 1.f), const SMatrix& p_Transform = SMatrix()
    );
    void AddGizmoEntity(
        const ZEntityRef p_EntityRef, const std::string& p_TypeName, const EDebugChannel p_DebugChannel, const std::string& p_PropertyName,
        const SVector4& p_Color = SVector4(1.f, 1.f, 1.f, 1.f), const SMatrix& p_Transform = SMatrix()
    );
    void DeleteDebugEntities(const std::shared_ptr<EntityTreeNode>& p_RootNode);
    EDebugChannel ConvertDrawLayerToDebugChannel(ZDebugGizmoEntity::EDrawLayer p_DrawLayer);
    static bool EntityIDMatches(void* p_Interface, uint64 p_EntityID);

    bool RayCastDebugEntities(const SVector3& p_WorldPosition, const SVector3& p_Direction);
    void RayCastGizmos(const SVector3& p_WorldPosition, const SVector3& p_Direction, RayCastResult& p_Result);
    void RayCastShapes(const SVector3& p_WorldPosition, const SVector3& p_Direction, RayCastResult& p_Result);

    bool m_ShowDebugChannelsWindow = false;

    std::unordered_map<ZEntityRef, std::vector<std::unique_ptr<DebugEntity>>> m_EntityRefToDebugEntities;
    std::shared_mutex m_DebugEntitiesMutex;
    std::vector<std::pair<std::string, EDebugChannel>> m_DebugChannels;
    std::unordered_map<std::string, std::vector<std::string>> m_DebugChannelNameToTypeNames;
    std::unordered_map<EDebugChannel, uint32> m_DebugChannelToDebugEntityCount;
    std::unordered_map<EDebugChannel, std::unordered_map<std::string, uint32_t>> m_DebugChannelToTypeNameToDebugEntityCount;
    std::unordered_map<EDebugChannel, bool> m_DebugChannelToGizmoState;
    std::unordered_map<EDebugChannel, bool> m_DebugChannelToShapeState;
    std::unordered_map<EDebugChannel, std::unordered_map<std::string, bool>> m_DebugChannelToTypeNameToGizmoState;
    std::unordered_map<EDebugChannel, std::unordered_map<std::string, bool>> m_DebugChannelToTypeNameToShapeState;
    std::vector<STypeID*> m_DebugEntityTypeIds;

    bool m_DrawGizmos = true;
    DebugDrawMode m_GizmoDrawMode = DebugDrawMode::SelectedChannelsAndTypes;

    bool m_DrawShapes = false;
    DebugDrawMode m_ShapeDrawMode = DebugDrawMode::SelectedChannelsAndTypes;

    ZEntityRef m_SelectedDebugEntity = nullptr;

#pragma endregion

#pragma region Settings

    void DrawSettingsWindow(zknt::IImGuiRenderer* p_Renderer, bool p_HasFocus);

    bool m_ShowSettingsWindow = false;
    bool m_ShowEntityEditor = true;

#pragma endregion

#pragma region Tasks

  public:
    void QueueTask(std::function<void()> p_Task);

  private:
    void ProcessTasks();

    std::mutex m_TaskMutex;
    std::vector<std::function<void()>> m_TaskQueue;

#pragma endregion

#pragma region Editor Server

    static void ToggleEditorServerEnabled();

    EditorServer m_Server;

#pragma endregion

#pragma region Editor API

  public:
    void SelectEntity(EntitySelector p_Selector, std::optional<std::string> p_ClientId);
    void SetEntityTransform(EntitySelector p_Selector, SMatrix p_Transform, bool p_Relative, std::optional<std::string> p_ClientId);
    // void SpawnQnEntity(const std::string& p_QnJson, uint64_t p_EntityId, std::string p_Name, std::optional<std::string> p_ClientId);
    // void CreateEntityResources(const std::string& p_QnJson, std::optional<std::string> p_ClientId);
    void DestroyEntity(EntitySelector p_Selector, std::optional<std::string> p_ClientId);
    void SetEntityName(EntitySelector p_Selector, std::string p_Name, std::optional<std::string> p_ClientId);
    void SetEntityProperty(EntitySelector p_Selector, uint32_t p_PropertyId, std::string_view p_JsonValue, std::optional<std::string> p_ClientId);
    void SignalEntityPin(EntitySelector p_Selector, uint32_t p_PinId, bool p_Output);

    std::shared_ptr<EntityTreeNode> GetEntityTree() {
        return m_CachedEntityTree;
    }

    std::string_view GetEntityName(ZEntityRef p_Entity, bool p_WithID = true);
    ZRuntimeResourceID GetContainingBlueprintResourceID(ZEntityRef p_Entity);

    ZEntityRef FindEntity(EntitySelector p_Selector);

    void RebuildEntityTree();

    static std::string GetCollisionHash(auto p_SelectedEntity);
    void FindMeshes(
        const std::function<
            void(std::vector<NavKitMeshEntity>&, std::map<std::string, NavKitMatiTextures>&, std::map<std::string, std::vector<std::string>>&, bool)>&
            p_SendEntitiesCallback,
        const std::function<void()>& p_RebuiltCallback
    );
    std::vector<std::tuple<std::vector<std::string>, Quat, ZEntityRef>>
    FindEntitiesByType(const std::string& p_EntityType, const std::string& p_Hash);

    static void FindAlocAndPrimForZGeomEntityNode(
        std::vector<NavKitMeshEntity>& p_Entities, const std::shared_ptr<EntityTreeNode>& p_Node, const TArray<SInterfaceData>& p_Interfaces,
        const char*& p_EntityType, const std::unordered_map<std::string, std::string>& p_RoomNameToFolderName,
        std::map<std::string, NavKitMatiTextures>& p_MatiTextures, std::map<std::string, std::vector<std::string>>& p_PrimMatis
    );
    static void FindAlocAndPrimForZPrimitiveProxyEntityNode(
        std::vector<NavKitMeshEntity>& p_Entities, const std::shared_ptr<EntityTreeNode>& p_Node, const TArray<SInterfaceData>& p_Interfaces,
        const char*& p_EntityType, const std::unordered_map<std::string, std::string>& p_RoomNameToFolderName,
        std::map<std::string, NavKitMatiTextures>& p_MatiTextures, std::map<std::string, std::vector<std::string>>& p_PrimMatis
    );

    static bool IsPropertyValueTrue(const SPropertyData* s_Property, const ZEntityRef& p_Entity);
    static bool IsExcludedFromNavMeshExport(const ZEntityRef& p_Entity);

#pragma endregion

  private:
    struct PropertyDeleter {
        template<typename T> void operator()(T* p_Data) const {
            if (!p_Data) {
                return;
            }

            if (m_Type) {
                if (const auto* s_TypeInfo = m_Type->GetTypeInfo()) {
                    if (s_TypeInfo->m_pTypeFunctions) {
                        s_TypeInfo->m_pTypeFunctions->destruct(p_Data);
                    }
                }
            }

            (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->Free(p_Data);
        }

        STypeID* m_Type = nullptr;
    };

    template<typename T> static std::unique_ptr<T, PropertyDeleter> GetProperty(ZEntityRef p_Entity, const SPropertyData* p_Property);
    static Quat GetQuatFromProperty(ZEntityRef p_Entity);
    static Quat GetParentQuat(ZEntityRef p_Entity);
    std::pair<std::string, std::string>
    FindRoomForEntity(ZEntityRef p_Entity, const std::unordered_map<std::string, std::string>& roomNameToFolderName);

    DECLARE_PLUGIN_DETOUR(Editor, void, OnCreateScene, ZEntitySceneContext* th);
    DECLARE_PLUGIN_DETOUR(Editor, void, OnClearScene, ZEntitySceneContext* th, bool bFullyUnloadScene);

    DECLARE_PLUGIN_DETOUR(
        Editor, void, ZEntitySceneContext_LoadDynamicBrick, ZEntitySceneContext* th, const ZRuntimeResourceID& runtimeResourceID,
        ZEntityRef entityRef, IEntityFactory* entityFactory
    );
    DECLARE_PLUGIN_DETOUR(Editor, void, ZEntitySceneContext_UnloadDynamicBrick, ZEntitySceneContext* th, const ZRuntimeResourceID& runtimeResourceID);

    DECLARE_PLUGIN_DETOUR(
        Editor, void, ZTemplateEntityFactory_ConfigureEntity, ZTemplateEntityFactory* th, ZEntityType** pEntity, void* unk0, void* unk1, void* unk2
    );

    DECLARE_PLUGIN_DETOUR(
        Editor, ZEntityRef*, ZEntityManager_NewUninitializedEntity, ZEntityManager* th, ZEntityRef& result, const ZString& sDebugName,
        TResourcePtr<IEntityFactory>& entityFactory, const ZEntityRef& logicalParent, uint64_t entityID, void* unk0, bool unk1
    );
    DECLARE_PLUGIN_DETOUR(Editor, uint64_t, GenerateEntityIdentifier, SEntityIdentifier* p_Identifier);
    DECLARE_PLUGIN_DETOUR(Editor, void, ZEntityManager_DeleteEntity, ZEntityManager* th, const ZEntityRef& entityRef);
};

DECLARE_ZKNT_PLUGIN(Editor)
