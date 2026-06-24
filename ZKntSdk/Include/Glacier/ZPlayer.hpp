#pragma once

#include "ZEntity.hpp"
#include "ZHumanoid.hpp"
#include "ZValue.hpp"

class ZCLGetLocalPlayerCharacterImpl : public ZEntityImpl {
  public:
    TEntityRef<ZHumanoidCharacterEntity> m_pCharacter; // 0x18
};

class ZCLGetLocalPlayerHumanoidCharacter : public ZEntityImpl {};

class ZCLGetLocalPlayerHumanoidCharacterImpl : public ZEntityImpl {
  public:
    TEntityRef<ZHumanoidCharacterEntity> m_pCharacter; // 0x18
};

class ZCLGetLocalPlayerID : public ZEntityImpl {};

class ZCLBlockHumanoidPlayerMoveInput : public ZEntityImpl {};

class ZCLUnblockHumanoidPlayerMoveInput : public ZEntityImpl {};

class ZCLBlockPlayerGadgetInput : public ZEntityImpl {};

class ZCLUnblockPlayerGadgetInput : public ZEntityImpl {};

class ZCLBlockHumanoidPlayerCloseCombatInput : public ZEntityImpl {};

class ZCLUnblockHumanoidPlayerCloseCombatInput : public ZEntityImpl {};

class ZLocalPlayerData {
  public:
    ZCLGetLocalPlayerCharacterImpl* m_pCharacterImpl;        // 0x0
    ZCLGetLocalPlayerHumanoidCharacterImpl* m_pHumanoidImpl; // 0x8
};

class ZCLGetMaximumPlayerResource : public ZEntityImpl, public IFloatValue {
  public:
    TResourcePtr<ZEntityRef> m_resourceDefinition; // 0x20
};

class ZCLGetCurrentPlayerResource : public ZEntityImpl, public IFloatValue {
  public:
    TResourcePtr<ZEntityRef> m_resourceDefinition; // 0x20
    TInterfaceRef<IIntValue> m_playerID;           // 0x28
};
