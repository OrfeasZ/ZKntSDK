#pragma once

#include "ZEntity.hpp"
#include "ZLocalization.hpp"

class ZKntLoadoutCollectionEntity : public ZEntityImpl {
  public:
    TArray<TResourcePtr<IEntityFactory>> m_firearmsCategories;    // 0x18
    TArray<TResourcePtr<IEntityFactory>> m_outfitCategories;      // 0x30
    TArray<TResourcePtr<IEntityFactory>> m_collectibleCategories; // 0x48
    TArray<TResourcePtr<IEntityFactory>> m_gadgets;               // 0x60
};

class ZCategoryBase : public ZEntityImpl {
  public:
    ZString m_titleRaw;                         // 0x18
    TResourcePtr<ZTextLine> m_titleSweet;       // 0x28
    ZString m_descriptionRaw;                   // 0x30
    TResourcePtr<ZTextLine> m_descriptionSweet; // 0x40
    ZRuntimeResourceID m_image;                 // 0x48
    ZString m_contextualData;                   // 0x50
    PAD(0x70);                                  // 0x60
};

class ZOutfitCategory : public ZCategoryBase {
  public:
    TArray<TResourcePtr<IEntityFactory>> m_outfits; // 0xD0
};

namespace KntLoadoutCollection {
    enum class EGameMode : int8_t { TacSimAndCampaign = 0, TacSim = 1, Campaign = 2 };

    enum class EOwnership : int8_t { Owned = 0, Locked = 1, LockedByCheckpoint = 2 };
}

class ZOutfitDefinitionEntity : public ZEntityImpl {
  public:
    ZString m_outfitIconID;                           // 0x18
    ZString m_outfitDisplayNameRaw;                   // 0x28
    TResourcePtr<ZTextLine> m_outfitDisplayNameSweet; // 0x38
    ZString m_outfitDescriptionRaw;                   // 0x40
    TResourcePtr<ZTextLine> m_outfitDescriptionSweet; // 0x50
    ZRuntimeResourceID m_image;                       // 0x58
    ZRuntimeResourceID m_highResImage;                // 0x60
    ZRuntimeResourceID m_heroImage;                   // 0x68
    TResourcePtr<ZEntityRef> m_outfitSet;             // 0x70
    ZRuntimeResourceID m_outfit;                      // 0x78
    KntLoadoutCollection::EGameMode m_gameMode;       // 0x80
    ZGuid m_onlineId;                                 // 0x88
};
