#pragma once

#include "EngineFunction.hpp"
#include "ModSDK.hpp"
#include "Util/ProcessUtils.hpp"
#include "Logging.hpp"
#include "Failures.hpp"

#include <cstdint>
#include <fmt/format.h>

namespace zknt {
    // PatternEngineFunction: locates a function by scanning for the byte
    // pattern at the start of its prologue.
    template<class T> class PatternEngineFunction;

    template<class ReturnType, class... Args> class PatternEngineFunction<ReturnType(Args...)> final : public EngineFunction<ReturnType(Args...)> {
      public:
        PatternEngineFunction(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask)
            : EngineFunction<ReturnType(Args...)>(GetTarget(p_Pattern, p_Mask)) {
            if (this->m_Address == nullptr) {
                Fail();
                Logger::Error(
                    "Could not locate address for function '{}'. This probably means that the game was updated and the SDK requires changes.",
                    p_FunctionName
                );
                return;
            }

            Logger::Debug("Successfully located function '{}' at address {}.", p_FunctionName, fmt::ptr(this->m_Address));
        }

      private:
        static void* GetTarget(const char* p_Pattern, const char* p_Mask) {
            const auto* s_Pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
            return reinterpret_cast<void*>(
                Util::ProcessUtils::SearchPattern(ModSDK::GetInstance()->GetModuleBase(), ModSDK::GetInstance()->GetSizeOfCode(), s_Pattern, p_Mask)
            );
        }
    };

    // PatternRelativeEngineFunction: locates a CALL site and follows its
    // 32-bit relative displacement to the target function.
    template<class T> class PatternRelativeEngineFunction;

    template<class ReturnType, class... Args>
    class PatternRelativeEngineFunction<ReturnType(Args...)> final : public EngineFunction<ReturnType(Args...)> {
      public:
        PatternRelativeEngineFunction(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask)
            : EngineFunction<ReturnType(Args...)>(GetTarget(p_FunctionName, p_Pattern, p_Mask)) {
            if (this->m_Address == nullptr) {
                Fail();
                Logger::Error(
                    "Could not locate address for function '{}'. This probably means that the game was updated and the SDK requires changes.",
                    p_FunctionName
                );
                return;
            }

            Logger::Debug("Successfully located function '{}' at address {}.", p_FunctionName, fmt::ptr(this->m_Address));
        }

      private:
        static void* GetTarget(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask) {
            const auto* s_Pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
            const auto s_Target =
                Util::ProcessUtils::SearchPattern(ModSDK::GetInstance()->GetModuleBase(), ModSDK::GetInstance()->GetSizeOfCode(), s_Pattern, p_Mask);

            if (s_Target == 0) {
                return nullptr;
            }

            if (*reinterpret_cast<uint8_t*>(s_Target) != 0xE8) {
                Logger::Error(
                    "Expected a call instruction for function '{}' at address {} but instead got 0x{:02X}.", p_FunctionName,
                    fmt::ptr(reinterpret_cast<void*>(s_Target)), *reinterpret_cast<uint8_t*>(s_Target)
                );
                return nullptr;
            }

            const uintptr_t s_TargetFn = s_Target + 5 + *reinterpret_cast<int32_t*>(s_Target + 1);
            return reinterpret_cast<void*>(s_TargetFn);
        }
    };

    // PatternVtableEngineFunction: locates a vtable assignment (LEA reg,
    // [rip+disp32]) and indexes into the resolved vtable to fetch the
    // function pointer at the given slot.
    template<class T> class PatternVtableEngineFunction;

    template<class ReturnType, class... Args>
    class PatternVtableEngineFunction<ReturnType(Args...)> final : public EngineFunction<ReturnType(Args...)> {
      public:
        PatternVtableEngineFunction(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask, size_t p_VtableIndex)
            : EngineFunction<ReturnType(Args...)>(GetTarget(p_FunctionName, p_Pattern, p_Mask, p_VtableIndex)) {
            if (this->m_Address == nullptr) {
                Logger::Error(
                    "Could not locate address for function '{}'. This probably means that the game was updated and the SDK requires changes.",
                    p_FunctionName
                );
                return;
            }

            Logger::Debug("Successfully located function '{}' at address {}.", p_FunctionName, fmt::ptr(this->m_Address));
        }

      private:
        static void* GetTarget(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask, size_t p_VtableIndex) {
            const auto* s_Pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
            const auto s_Target =
                Util::ProcessUtils::SearchPattern(ModSDK::GetInstance()->GetModuleBase(), ModSDK::GetInstance()->GetSizeOfCode(), s_Pattern, p_Mask);

            if (s_Target == 0) {
                return nullptr;
            }

            if (*reinterpret_cast<uint8_t*>(s_Target) != 0x48) {
                Logger::Error(
                    "Expected a rex prefix for vtable function '{}' at address {} but instead got 0x{:02X}.", p_FunctionName,
                    fmt::ptr(reinterpret_cast<void*>(s_Target)), *reinterpret_cast<uint8_t*>(s_Target)
                );
                return nullptr;
            }

            const uintptr_t s_VtableAddr = s_Target + 7 + *reinterpret_cast<int32_t*>(s_Target + 3);
            const uintptr_t s_VtableSlot = s_VtableAddr + p_VtableIndex * sizeof(void*);
            return *reinterpret_cast<void**>(s_VtableSlot);
        }
    };
}

#define PATTERN_FUNCTION(Pattern, Mask, FunctionName, FunctionType) \
    this->FunctionName = new ::zknt::PatternEngineFunction<FunctionType>(#FunctionName, Pattern, Mask)

#define PATTERN_RELATIVE_FUNCTION(Pattern, Mask, FunctionName, FunctionType) \
    this->FunctionName = new ::zknt::PatternRelativeEngineFunction<FunctionType>(#FunctionName, Pattern, Mask)

/**
 * Find the pointer of a function by searching for a vtable assignment matching the given
 * pattern, and then indexing into the vtable at the given index.
 */
#define PATTERN_VTABLE_FUNCTION(Pattern, Mask, VtableIndex, FunctionName, FunctionType) \
    this->FunctionName = new ::zknt::PatternVtableEngineFunction<FunctionType>(#FunctionName, Pattern, Mask, VtableIndex)
