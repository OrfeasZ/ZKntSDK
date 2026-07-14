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
struct SVector2;
struct SVector3;
struct SMatrix;

namespace zknt {
    class Hooks;
    class Functions;
    class Globals;
    class IPluginInterface;

    class IModSDK {
      public:
        virtual ~IModSDK() = default;

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

        /**
         * Set a plugin setting value for the given name.
         * @param p_Plugin The plugin to set the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        virtual void SetPluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, const ZString& p_Value) = 0;

        /**
         * Set a plugin setting integer value for the given name.
         * @param p_Plugin The plugin to set the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        virtual void SetPluginSettingInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, int64_t p_Value) = 0;

        /**
         * Set a plugin setting unsigned integer value for the given name.
         * @param p_Plugin The plugin to set the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        virtual void SetPluginSettingUInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, uint64_t p_Value) = 0;

        /**
         * Set a plugin setting double value for the given name.
         * @param p_Plugin The plugin to set the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        virtual void SetPluginSettingDouble(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, double p_Value) = 0;

        /**
         * Set a plugin setting boolean value for the given name.
         * @param p_Plugin The plugin to set the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_Value The value of the setting.
         */
        virtual void SetPluginSettingBool(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, bool p_Value) = 0;

        /**
         * Get a plugin setting string value for the given name.
         * @param p_Plugin The plugin to get the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to use if the setting does not exist.
         * @param p_OutValue Receives the value of the setting, or p_DefaultValue if the setting does not exist.
         */
        virtual void GetPluginSetting(
            IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, const ZString& p_DefaultValue, ZString& p_OutValue
        ) = 0;

        /**
         * Get a plugin setting integer value for the given name.
         * @param p_Plugin The plugin to get the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to return if the setting does not exist or is not an integer.
         * @return The value of the setting, or the default value if the setting does not exist or is not an integer.
         */
        virtual int64_t GetPluginSettingInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, int64_t p_DefaultValue) = 0;

        /**
         * Get a plugin setting unsigned integer value for the given name.
         * @param p_Plugin The plugin to get the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to return if the setting does not exist or is not an unsigned integer.
         * @return The value of the setting, or the default value if the setting does not exist or is not an unsigned integer.
         */
        virtual uint64_t
        GetPluginSettingUInt(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, uint64_t p_DefaultValue) = 0;

        /**
         * Get a plugin setting double value for the given name.
         * @param p_Plugin The plugin to get the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to return if the setting does not exist or is not a double.
         * @return The value of the setting, or the default value if the setting does not exist or is not a double.
         */
        virtual double GetPluginSettingDouble(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, double p_DefaultValue) = 0;

        /**
         * Get a plugin setting boolean value for the given name.
         * @param p_Plugin The plugin to get the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @param p_DefaultValue The default value to return if the setting does not exist or is not a boolean.
         * @return The value of the setting, or the default value if the setting does not exist or is not a boolean.
         */
        virtual bool GetPluginSettingBool(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name, bool p_DefaultValue) = 0;

        /**
         * Check if a plugin setting with the given name exists.
         * @param p_Plugin The plugin to check the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         * @return True if the setting exists, false otherwise.
         */
        virtual bool HasPluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name) = 0;

        /**
         * Remove a plugin setting with the given name.
         * @param p_Plugin The plugin to remove the setting for.
         * @param p_Section The section of the setting in the INI file.
         * @param p_Name The name of the setting.
         */
        virtual void RemovePluginSetting(IPluginInterface* p_Plugin, const ZString& p_Section, const ZString& p_Name) = 0;

        /**
         * Reload the settings for the given plugin.
         * @param p_Plugin The plugin to reload the settings for.
         */
        virtual void ReloadPluginSettings(IPluginInterface* p_Plugin) = 0;

        /**
         * Convert a position in 3D world space to a 2D position on the screen.
         * @param p_WorldPos The 3D world position.
         * @param p_Out The output 2D position on the screen.
         * @return True if the conversion was successful, false otherwise (eg. if the position is behind the camera).
         */
        virtual bool WorldToScreen(const SVector3& p_WorldPos, SVector2& p_Out) = 0;

        /**
         * Convert a 2D position on the screen to a 3D world position.
         * @param p_ScreenPos The 2D position on the screen.
         * @param p_WorldPosOut The output 3D world position.
         * @param p_DirectionOut The output direction of the ray.
         * @return True if the conversion was successful, false otherwise (eg. if the position is outside the screen bounds).
         */
        virtual bool ScreenToWorld(const SVector2& p_ScreenPos, SVector3& p_WorldPosOut, SVector3& p_DirectionOut) = 0;

        /**
         * Get the current camera view matrix.
         * @return The camera view matrix used for the current frame.
         */
        virtual SMatrix GetViewMatrix() const = 0;

        /**
         * Get the current camera projection matrix.
         * @return The camera projection matrix used for the current frame.
         */
        virtual SMatrix GetProjectionMatrix() const = 0;
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
