#pragma once

#include <set>

#include <IPluginInterface.hpp>
#include <Glacier/ZCamera.hpp>
#include <Glacier/ZValue.hpp>
#include <Glacier/ZPlayer.hpp>
#include <Glacier/ZSpawner.hpp>
#include <Glacier/ZGadget.hpp>
#include <Glacier/ZFirearm.hpp>

class ZCLSetHumanoidOutfitEntity;
class ZCLSetPlayerInvisibleToNpcs;
class ZCLGiveResourceToPlayer;
class ZCLAttachItemToHumanoid;
class ZCLAssignGadgetToSlot;
class ZItemCharacterEntityBase;
class ZCLGiveHumanoidPlayerAmmunition;
class ZCLSetPlayerEquippedItem;

class Cheats : public zknt::IPluginInterface {
  public:
    Cheats();
    ~Cheats();

    void Init() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(bool p_HasFocus) override;

  private:
    struct OutfitInfo {
        std::vector<std::pair<std::string, size_t>> m_Variations;
        ZRuntimeResourceID m_OutfitSet;
    };

    struct GadgetInfo {
        std::string m_Name;
        ZRuntimeResourceID m_GadgetItemDefinition;
        ZRuntimeResourceID m_ItemTemplate;
    };

    enum class SpawnMode { AddToWorld, AddToInventory };

    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    void CleanupSpawnedEntities();
    bool EnsureEntitiesSpawned();
    void ApplyPlayerModifiers();
    bool AnyCheatEnabled() const;

    void LoadPlayerOutfitSets();
    void LoadAllOutfitSets();
    void SetPlayerOutfit(const ZRuntimeResourceID& p_OutfitSet, size_t p_OutfitVariationIndex);

    void LoadGadgets();
    void SpawnGadget(const ZRuntimeResourceID& p_ItemDefinition, const ZRuntimeResourceID& p_ItemResource, Gameplay::EGadgetActivationSlot p_Slot);

    void LoadFirearms();
    void SpawnFirearm(const ZRuntimeResourceID& p_ItemResource);
    static const char* FirearmClassToString(EFirearmClass p_FirearmClass);

    DECLARE_PLUGIN_DETOUR(
        Cheats, ZKntLoadoutCollectionEntity*, ZKntLoadoutCollectionEntity_ZKntLoadoutCollectionEntity, ZKntLoadoutCollectionEntity* th, bool unk
    );

    bool m_FrameUpdateRegistered = false;
    bool m_NoclipEnabled = false;
    bool m_NoclipWasEnabled = false;
    SMatrix m_PlayerPosition = {};

    // Cheats helpers.
    TEntityRef<ZCLTeleportHumanoidEntity> m_Teleporter;
    TEntityRef<ZSpatialEntity> m_TeleportTarget;
    TEntityRef<ZCLGetLocalPlayerHumanoidCharacter> m_LocalPlayerHumanoidGetter;
    TEntityRef<ZCLEnableDisableHumanoidCollision> m_CollisionModifier;
    TEntityRef<ZCLSetHumanoidImmuneToDamage> m_ImmuneModifier;
    TEntityRef<ZCLSetHumanoidUnkillableByDamage> m_UnkillableModifier;
    TEntityRef<ZCLSetHumanoidInfiniteClipAmmo> m_InfiniteAmmoModifier;
    TEntityRef<ZCLSetPlayerInvisibleToNpcs> m_InvisibleModifier;
    TEntityRef<ZCLGetLocalPlayerID> m_LocalPlayerIDGetter;
    TEntityRef<ZCLSetHumanoidOutfitEntity> m_SetHumanoidOutfit;
    TEntityRef<ZCLValueBoolEntity> m_ImmuneBoolValue;
    TEntityRef<ZCLValueBoolEntity> m_UnkillableBoolValue;
    TEntityRef<ZCLValueBoolEntity> m_InvisibleBoolValue;
    TEntityRef<ZCLGetCurrentPlayerResource> m_CurrentElectricityGetter;
    TEntityRef<ZCLGetCurrentPlayerResource> m_CurrentChemicalGetter;
    TEntityRef<ZCLGetMaximumPlayerResource> m_MaximumElectricityGetter;
    TEntityRef<ZCLGetMaximumPlayerResource> m_MaximumChemicalGetter;
    TEntityRef<ZCLGiveResourceToPlayer> m_ElectricityGiver;
    TEntityRef<ZCLGiveResourceToPlayer> m_ChemicalGiver;
    TEntityRef<ZCLValueFloatEntity> s_ElectricityAmountFloatValue;
    TEntityRef<ZCLValueFloatEntity> s_ChemicalAmountFloatValue;
    TEntityRef<ZDynamicGameplaySpawnerEntity> m_GadgetSpawner;
    TEntityRef<ZDynamicGameplaySpawnerItemEntryEntity> m_GadgetSpawnerItemEntry;
    TEntityRef<ZCLAttachItemToHumanoid> m_GadgetAttacher;
    TEntityRef<ZCLAssignGadgetToSlot> m_GadgetSlotAssigner;
    TEntityRef<ZDynamicGameplaySpawnerEntity> m_FirearmSpawner;
    TEntityRef<ZDynamicGameplaySpawnerItemEntryEntity> m_FirearmSpawnerItemEntry;
    TEntityRef<ZCLSetPlayerEquippedItem> m_EquippedItemSetter;
    std::array<TEntityRef<ZCLGetPlayerInventoryAmmunition>, 8> m_AmmunitionGetters;
    std::array<TEntityRef<ZCLGiveHumanoidPlayerAmmunition>, 8> m_AmmunitionSetters;

    bool m_ShowPanel = false;
    bool m_DisableCollision = false;
    bool m_GodMode = false;
    bool m_Unkillable = false;
    bool m_InfiniteAmmo = false;
    bool m_Invisible = false;
    bool m_InfiniteElectricity = false;
    bool m_InfiniteChecmical = false;
    bool m_StateDirty = false;

    ZInputAction m_ToggleNoclipAction;
    ZInputAction m_ForwardAction;
    ZInputAction m_BackwardAction;
    ZInputAction m_LeftAction;
    ZInputAction m_RightAction;
    ZInputAction m_FastAction;

    ZKntLoadoutCollectionEntity* m_KntLoadoutCollectionEntity = nullptr;
    std::vector<std::string> m_OutfitCategories;
    std::unordered_map<std::string, std::set<std::string>> m_OutfitCategoryToOutfits;
    std::unordered_map<std::string, OutfitInfo> m_OutfitNameToOutfitInfo;

    std::map<std::string, OutfitInfo> m_AllOutfitSets;

    std::vector<GadgetInfo> m_Gadgets;
    bool m_AssignGadgetToSlot = false;

    std::map<std::string, std::set<std::string>> m_FirearmCategoryToFirearmNames;
    std::unordered_map<std::string, ZRuntimeResourceID> m_FirearmNameToItemResource;
    SpawnMode m_SpawnMode = SpawnMode::AddToWorld;
    bool m_EquipFirearm = false;
};

DECLARE_ZKNT_PLUGIN(Cheats)
