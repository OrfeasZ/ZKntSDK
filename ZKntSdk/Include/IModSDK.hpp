#pragma once

#include "Common.hpp"
#include <spdlog/spdlog.h>
#include <string_view>

#include "imgui.h"

template<typename T> class TResourcePtr;
class ZTemplateEntityBlueprintFactory;
class ZTemplateEntityFactory;
class ZCppEntityBlueprintFactory;
class ZCppEntityFactory;

struct ImGuiContext;

class ZString;

namespace zknt {
    class Hooks;
    class Functions;
    class Globals;

    class IModSDK {
      public:
        virtual ~IModSDK() = default;

        virtual ImGuiContext* GetImGuiContext() = 0;
        virtual ImGuiMemAllocFunc GetImGuiAlloc() = 0;
        virtual ImGuiMemFreeFunc GetImGuiFree() = 0;
        virtual void* GetImGuiAllocatorUserData() = 0;
        virtual ImFont* GetImGuiLightFont() = 0;
        virtual ImFont* GetImGuiRegularFont() = 0;
        virtual ImFont* GetImGuiMediumFont() = 0;
        virtual ImFont* GetImGuiBoldFont() = 0;
        virtual ImFont* GetImGuiBlackFont() = 0;

        virtual zknt::Hooks* Hooks() = 0;
        virtual zknt::Functions* Functions() = 0;
        virtual zknt::Globals* Globals() = 0;

        virtual void Log(spdlog::level::level_enum p_Level, std::string_view p_Msg) = 0;

        /**
         * Search for a pattern in the game's memory and patch it with the given code.
         * @param p_Pattern A sequence of bytes to
         * search for in the game's memory.
         * @param p_Mask A mask to use when searching for the pattern. x = pattern byte, ? = any byte (eg.
         * xxx????x).
         * @param p_NewCode A buffer containing the new code to write to the location where the pattern was found.
         *
         * @param p_CodeSize The size of the code buffer.
         * @param p_Offset The offset to add to the address where the pattern was found.

         * @return True if the pattern was found and patched, false otherwise.
         */
        virtual bool PatchCode(const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_Offset) = 0;

        /**
         * Search for a pattern in the game's memory and patch it with the given code, storing the original code in a provided buffer.
         * @param p_Pattern A sequence of bytes to search for in the game's memory.
         * @param p_Mask A mask to use when searching for the
         * pattern. x = pattern byte, ? = any byte (eg. xxx????x).
         * @param p_NewCode A buffer containing the new code to write to the
         * location where the pattern was found.
         * @param p_CodeSize The size of the code buffer.
         * @param p_Offset The offset to
         * add to the address where the pattern was found.
         * @param p_OriginalCode A buffer to store the original code.
         * @return
         * True if the pattern was found and patched, false otherwise.
         */
        virtual bool PatchCodeStoreOriginal(
            const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_Offset, void* p_OriginalCode
        ) = 0;

        virtual void AllocateZString(ZString* p_Target, const char* p_Str, uint32_t p_Size) = 0;
        virtual void FreeZString(ZString* p_Target) = 0;

        /**
         * Load C++ entity resources from JSON. This makes them available
         * to the engine for spawning.
         *
         * @param p_BlueprintJson The C++ entity blueprint JSON string.
         * @param p_BlueprintMetaJson The C++ entity blueprint metadata JSON string.
         * @param p_EntityJson The C++ entity JSON string.
         * @param p_EntityMetaJson The C++ entity metadata JSON string.
         * @param p_BlueprintFactoryOut The resulting C++ entity blueprint factory resource.
         * @param p_TemplateFactoryOut The resulting C++ entity factory resource.
         * @return True if the resources were loaded successfully, false otherwise.
         */
        virtual bool LoadCppEntity(
            const ZString& p_BlueprintJson, const ZString& p_BlueprintMetaJson, const ZString& p_EntityJson, const ZString& p_EntityMetaJson,
            TResourcePtr<ZCppEntityBlueprintFactory>& p_BlueprintFactoryOut, TResourcePtr<ZCppEntityFactory>& p_TemplateFactoryOut
        ) = 0;
    };
}

// Per-DLL pointer to the active SDK instance. The SDK's own startup path
// sets it from inside `ZKntSdk.dll`; each mod sets its own copy in
// `cr_main(CR_LOAD)` from `HostServices::m_SDK`. `inline` guarantees each
// linkage unit (DLL) gets its own storage, so the pointer is never
// imported across modules.
inline zknt::IModSDK*& g_SDKInstance() {
    static zknt::IModSDK* s_Instance = nullptr;
    return s_Instance;
}

inline zknt::IModSDK* SDK() {
    return g_SDKInstance();
}

inline void SetSDK(zknt::IModSDK* p_Sdk) {
    g_SDKInstance() = p_Sdk;
}
