#pragma once

#include "ZEntity.hpp"

class ZDynamicGameplaySpawnerEntryEntity;
class ZDynamicGameplaySpawnerItemEntryEntity;

class ZDynamicGameplaySpawnerBaseEntity : public ZEntityImpl {
  public:
    TArray<TInterfaceRef<ZDynamicGameplaySpawnerEntryEntity>> m_entries; // 0x18
    PAD(0x20);                                                           // 0x30
    bool IsSpawning;                                                     // 0x50
};

class ZDynamicGameplaySpawnerEntity : public ZDynamicGameplaySpawnerBaseEntity {};
