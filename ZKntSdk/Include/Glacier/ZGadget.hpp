#pragma once

#include "ZItem.hpp"

namespace Gameplay {
    enum class EGadgetActivationSlot : int32_t {
        Slot1 = 0,
        Slot2 = 1,
        Slot3 = 2,
        Slot4 = 3,
    };
}

class ZGadgetItemDefinition : public ZItemCharacterDefinitionBase {};
