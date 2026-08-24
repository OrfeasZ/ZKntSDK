#pragma once

#include "EventDispatcher.hpp"
#include "Common.hpp"

#include "Glacier/TArray.hpp"
#include "Glacier/ZString.hpp"

namespace zknt {
    class Events {
      public:
        Events();
        ~Events();

        Events(const Events&) = delete;
        Events& operator=(const Events&) = delete;

        EventDispatcher<TArray<ZString>&>* OnConsoleCommand = nullptr;
    };
}
