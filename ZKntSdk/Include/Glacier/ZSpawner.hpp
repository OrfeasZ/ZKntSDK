#pragma once

#include "ZEntity.hpp"

class ZDynamicGameplaySpawnerEntryEntity : public ZSpatialEntity {
  public:
    virtual ZRuntimeResourceID GetTemplate() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk46() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk47() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk48() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk49() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk50() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk51() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk52() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk53() = 0;
    virtual void ZDynamicGameplaySpawnerEntryEntity_unk54() = 0;
    virtual bool IsTemplateValid() = 0;

    PAD(0x60);                                    // 0xA8
    ZResourcePtr m_invalidCharacterGizmoResource; // 0x108
    ZResourcePtr m_validCharacterGizmoResource;   // 0x110
    bool m_showPreview;                           // 0x118
};

class ZDynamicGameplaySpawnerHumanoidEntryEntity : public ZDynamicGameplaySpawnerEntryEntity {
  public:
    ZRuntimeResourceID m_humanoidTemplate;   // 0x120
    int32 m_selectedOutfit;                  // 0x128
    ZRuntimeResourceID m_overrideDefinition; // 0x12C
    bool m_saveHumanoid;                     // 0x134
    bool m_forceClothWarmUp;                 // 0x135
    PAD(0x4A);                               // 0x136
};

class ZDynamicGameplaySpawnerItemEntryEntity : public ZDynamicGameplaySpawnerEntryEntity {
  public:
    ZRuntimeResourceID m_itemTemplate; // 0x120
};

class ZDynamicGameplaySpawnerBaseEntity : public ZEntityImpl {
  public:
    TArray<TInterfaceRef<ZDynamicGameplaySpawnerEntryEntity>> m_entries; // 0x18
    PAD(0x20);                                                           // 0x30
    bool IsSpawning;                                                     // 0x50
};

class ZDynamicGameplaySpawnerEntity : public ZDynamicGameplaySpawnerBaseEntity {};
