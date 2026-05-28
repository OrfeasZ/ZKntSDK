#include "Globals.hpp"
#include "GlobalsImpl.hpp"

zknt::Globals::Globals() {
    // PATTERN_RELATIVE_GLOBAL(
    //     "\x48\x8D\x0D\x00\x00\x00\x00\x4C\x89\x74\x24\x48\xE8",
    //     "xxx????xxxxxx",
    //     3,
    //     SomeType*, SomeGlobal
    // );
}
