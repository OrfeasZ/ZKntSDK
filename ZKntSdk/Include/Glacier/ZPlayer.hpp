#pragma

#include "ZEntity.hpp"
#include "ZCharacter.hpp"

class ZCLGetLocalPlayerCharacterImpl : public ZEntityImpl {
  public:
    TEntityRef<ZHumanoidCharacterEntity> m_pCharacter; // 0x18
};

class ZCLGetLocalPlayerHumanoidCharacterImpl : public ZEntityImpl {
  public:
    TEntityRef<ZHumanoidCharacterEntity> m_pCharacter; // 0x18
};

class ZLocalPlayerData {
  public:
    ZCLGetLocalPlayerCharacterImpl* m_pCharacterImpl;        // 0x0
    ZCLGetLocalPlayerHumanoidCharacterImpl* m_pHumanoidImpl; // 0x8
};
