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
    class ZTransientStoreRegistries {
      public:
        ZRegistry<TInterfaceRef<ZHumanoidCharacterEntity>> m_HumanoidRegistry; // 0x0
        ZRegistry<TInterfaceRef<ZItemCharacterEntityBase>> m_ItemRegistry;     // 0x28
    };

    class ZGPWTransientStores {
      public:
        virtual ~ZGPWTransientStores() = 0;

        void* m_pUnknown;                                       // 0x8
        ZTransientStoreRegistries* m_pTransientStoreRegistries; // 0x10
    };
}
