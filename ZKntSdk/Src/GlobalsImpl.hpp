#pragma once

#include <cstdint>
#include <fmt/format.h>

#include "ModSDK.hpp"
#include "Util/ProcessUtils.hpp"
#include "Logging.hpp"
#include "Failures.hpp"

namespace zknt {
    template<class T> T PatternGlobalRelative(const char* p_GlobalName, const char* p_Pattern, const char* p_Mask, ptrdiff_t p_Offset) {
        static_assert(std::is_pointer_v<T>, "Global type must be a pointer type.");

        const auto* s_Pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
        const auto s_Target =
            Util::ProcessUtils::SearchPattern(ModSDK::GetInstance()->GetModuleBase(), ModSDK::GetInstance()->GetSizeOfCode(), s_Pattern, p_Mask);

        if (s_Target == 0) {
            Fail();
            Logger::Error(
                "Could not find address for global '{}'. This probably means that the game was updated and the SDK requires changes.", p_GlobalName
            );
            return nullptr;
        }

        const uintptr_t s_DispPtr = s_Target + p_Offset;
        const int32_t s_Disp = *reinterpret_cast<int32_t*>(s_DispPtr);
        const uintptr_t s_FinalAddr = s_DispPtr + s_Disp + sizeof(int32_t);

        Logger::Debug("Successfully located global '{}' at address {}.", p_GlobalName, fmt::ptr(reinterpret_cast<void*>(s_FinalAddr)));
        return reinterpret_cast<T>(s_FinalAddr);
    }
}

#define PATTERN_RELATIVE_GLOBAL(Pattern, Mask, Offset, GlobalType, GlobalName) \
    this->GlobalName = ::zknt::PatternGlobalRelative<GlobalType>(#GlobalName, Pattern, Mask, Offset)
