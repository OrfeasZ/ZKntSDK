#pragma once

#include "Common.hpp"
#include <cstdint>

namespace zknt {
    class Globals {
      public:
        Globals();
        ~Globals() = default;

        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;

        // SomeType* SomeGlobal = nullptr;
    };
}
