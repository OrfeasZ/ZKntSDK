#include "Functions.hpp"
#include "EngineFunctionImpl.hpp"

zknt::Functions::Functions() {
    // PATTERN_FUNCTION(
    //     "\x40\x53\x48\x83\xEC\x00\x48\x8B\x05",
    //     "xxxxx?xxx",
    //     SomeType_DoThing,
    //     void(SomeType*)
    // );
}

zknt::Functions::~Functions() {
    // delete SomeType_DoThing;
}
