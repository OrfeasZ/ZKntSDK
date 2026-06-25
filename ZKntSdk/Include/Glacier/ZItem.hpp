#pragma once

#include "ZEntity.hpp"
#include "ZLocalization.hpp"

class ZKeywordEntity;

enum class EItemType : int32_t {
    NoItem = 0,
    HandGun = 1,
    Taser = 2,
    DartGun = 3,
    SubMachineGun = 4,
    AssaultRifle = 5,
    Shotgun = 6,
    SmokePellets = 7,
    BlastDevice = 8,
    Brick = 9,
    Vase = 10,
    Bust = 11,
    Undefined = 12,
    LongRifle = 13,
    Cosmetic = 14
};

enum class EInventorySlotType : int32_t {
    ThrowableItem = 0,
    FirearmOneHanded = 1,
    FirearmTwoHanded = 2,
    ServicePistol = 3,
    SpecialWeapon = 4,
    GadgetA = 5,
    GadgetB = 6,
    GadgetC = 7,
    GadgetD = 8,
    GadgetE = 9,
    Empty = 10
};

enum class EItemAttachmentSlot : int32_t {
    None = -1,
    RightHand = 0,
    LeftHand = 1,
    Back = 2,
    Head = 3,
    Neck = 4,
    RightWrist = 5,
    LeftWrist = 6,
    Chest = 7
};

class ZItemCharacterDefinitionBase : public ZEntityImpl {
  public:
    float32 m_equipTime;                                 // 0x18
    float32 m_unequipTime;                               // 0x1C
    float32 m_dropTime;                                  // 0x20
    bool m_playerPickupEnabled;                          // 0x24
    bool m_npcPickupEnabled;                             // 0x25
    float32 m_pickupTime;                                // 0x28
    float32 m_maxItemGroundOffset;                       // 0x2C
    bool m_npcDropOnDeathEnabled;                        // 0x30
    bool m_availableInSocial;                            // 0x31
    bool m_availableInTrespassing;                       // 0x32
    bool m_availableInLicenseToKill;                     // 0x33
    EItemType m_itemType;                                // 0x34
    EInventorySlotType m_inventorySlot;                  // 0x38
    bool m_selectableByPlayer;                           // 0x3C
    EItemAttachmentSlot m_equippedAttachmentSlot;        // 0x40
    EItemAttachmentSlot m_unequippedAttachmentSlot;      // 0x44
    bool m_overrideAttachOffset;                         // 0x48
    uint32 m_inventoryAttachOffset;                      // 0x4C
    uint32 m_equipAttachOffset;                          // 0x50
    uint32 m_unequipAttachOffset;                        // 0x54
    ZGuid m_itemId;                                      // 0x58
    TArray<TEntityRef<ZKeywordEntity>> m_keywords;       // 0x68
    PAD(0x20);                                           // 0x80
    ZString m_itemIconID;                                // 0xA0
    ZString m_itemDisplayNameRaw;                        // 0xB0
    TResourcePtr<ZTextLine> m_itemDisplayNameSweet;      // 0xC0
    ZString m_itemDescriptionRaw;                        // 0xC8
    TResourcePtr<ZTextLine> m_itemDescriptionSweet;      // 0xD8
    ZString m_itemDescriptionShortRaw;                   // 0xE0
    TResourcePtr<ZTextLine> m_itemDescriptionShortSweet; // 0xF0
    ZRuntimeResourceID m_itemImage;                      // 0xF8
    ZRuntimeResourceID m_itemHighResImage;               // 0x100
    ZRuntimeResourceID m_itemThumbnail;                  // 0x108
    ZRuntimeResourceID m_itemIcon;                       // 0x110
    ZRuntimeResourceID m_itemVideo;                      // 0x118
};
