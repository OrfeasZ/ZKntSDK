#pragma once

#include "THashMap.hpp"
#include "ZHumanoid.hpp"
#include "ZItem.hpp"

template<typename T> class ZRegistry {
  public:
    THashMap<T, uint32_t>* m_EntryToIndex; // 0x0
    TArray<T> m_Entries;                   // 0x8
    uint32_t m_ModificationSerial;         // 0x20
};

namespace Gameplay {
    struct SInventoryEntry {
        uint32_t m_Unk0;       // 0x0
        uint32_t m_ItemHandle; // 0x4
        uint64_t m_Unk1;       // 0x8
    };

    struct SBlockInfo {
        TArray<uint8_t> m_Occupancy; // 0x0
        uint32_t m_Capacity;         // 0x18
    };

    template<typename T> class ZKntGPWStore {
      public:
        bool IsValidHandle(uint32_t p_Handle) const {
            const uint16_t s_Index = static_cast<uint16_t>(p_Handle);
            const size_t s_BlockIndex = s_Index / 100;
            const size_t s_EntryIndex = s_Index % 100;

            if (s_BlockIndex >= m_Blocks.size() || s_BlockIndex >= m_BlockInfos.size()) {
                return false;
            }

            const SBlockInfo& s_BlockInfo = m_BlockInfos[s_BlockIndex];

            if (!s_BlockInfo.m_Capacity) {
                return false;
            }

            const size_t s_ByteIndex = s_EntryIndex / 8;
            const uint8_t s_BitMask = static_cast<uint8_t>(1u << (s_EntryIndex & 7));

            if (s_ByteIndex >= s_BlockInfo.m_Occupancy.size() || !(s_BlockInfo.m_Occupancy[s_ByteIndex] & s_BitMask)) {
                return false;
            }

            if (s_Index >= m_Generations.size()) {
                return false;
            }

            return m_Generations[s_Index] == static_cast<uint8_t>(p_Handle >> 16);
        }

        TArray<T*> m_Blocks;             // 0x00
        TArray<SBlockInfo> m_BlockInfos; // 0x18
        TArray<uint8_t> m_Generations;   // 0x30
        uint32_t m_Unk;                  // 0x48
    };

    class ZKntGPWStoreData {
      public:
        PAD(0xE0);                                             // 0x0
        ZKntGPWStore<uint32_t> m_HumanoidToEquippedItemHandle; // 0xE0
        PAD(0xB38);                                            // 0x130
        ZKntGPWStore<uint16_t> m_HumanoidToInventoryIndex;     // 0xC68
        TArray<TArray<SInventoryEntry>> m_Inventories;         // 0xCB8
    };

    class ZKntGPWStoreCollection {
      public:
        PAD(0x38);                      // 0x0
        ZKntGPWStoreData* m_pStoreData; // 0x38
    };

    class ZKntGPWStores {
      public:
        virtual ~ZKntGPWStores() = 0;

        PAD(0x30);                                  // 0x8
        ZKntGPWStoreCollection* m_pStoreCollection; // 0x38
    };

    class ZTransientStoreRegistryData {
      public:
        PAD(0x100);                                                                  // 0x0
        ZKntGPWStore<TInterfaceRef<ZItemCharacterEntityBase>> m_ItemRegistryEntries; // 0x100
    };

    class ZTransientStoreRegistries {
      public:
        ZRegistry<TInterfaceRef<ZHumanoidCharacterEntity>> m_HumanoidRegistry; // 0x0
        PAD(0x28);                                                             // 0x28
        ZRegistry<TInterfaceRef<ZItemCharacterEntityBase>> m_ItemRegistry;     // 0x50
    };

    class ZGPWTransientStores {
      public:
        virtual ~ZGPWTransientStores() = 0;

        ZTransientStoreRegistryData* m_pRegistryData;           // 0x8
        ZTransientStoreRegistries* m_pTransientStoreRegistries; // 0x10
    };
}
