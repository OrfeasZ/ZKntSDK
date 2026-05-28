#pragma once

#include "Common.hpp"
#include "EngineFunction.hpp"

namespace zknt {
    class Functions {
      public:
        Functions();
        ~Functions();

        Functions(const Functions&) = delete;
        Functions& operator=(const Functions&) = delete;

        // EngineFunction<void(SomeType*)>* SomeType_DoThing = nullptr;
    };
}
