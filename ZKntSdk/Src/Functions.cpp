#include "Functions.hpp"
#include "EngineFunctionImpl.hpp"

zknt::Functions::Functions() {
    PATTERN_RELATIVE_FUNCTION(
        "\xE8\x00\x00\x00\x00\x48\x8D\x4D\x10\xC5\xF8\x10\x00", "x????xxxxxxxx", ZEntityManager_NewEntity,
        ZEntityRef * (ZEntityManager * th, ZEntityRef & result, SEntityCreateInfo & entityCreateInfo)
    );

    PATTERN_FUNCTION(
        "\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x00\x48\x8B\xDA\x48\x8B\xF9\xE8\x00\x00\x00\x00\x33\xD2\x4C\x8D\x8C\x24\x80\x00\x00\x00",
        "xxxxxxxxx?xxxxxxx????xxxxxxxxxx", ZEntityManager_DeleteEntity, void(ZEntityManager * th, const ZEntityRef& result)
    );

    PATTERN_RELATIVE_FUNCTION(
        "\xE8\x00\x00\x00\x00\x4C\x8D\x44\x24\x70\x48\x8D\x54\x24\x58", "x????xxxxxxxxxx", SEntityCreateInfo_SEntityCreateInfo,
        SEntityCreateInfo * (SEntityCreateInfo * th, const ZString& sDebugName, ZResourcePtr&, const ZEntityRef& transformParent, uint64_t entityId)
    );

    PATTERN_FUNCTION(
        "\x40\x53\x48\x83\xEC\x00\x48\x8B\xDA\x48\x8D\x54\x24\x20\xE8\x00\x00\x00\x00\x8B\x4C\x24\x28", "xxxxx?xxxxxxxxx????xxxx",
        ZCameraManagerMain_GetActiveMainCamera, TEntityRef<ZCameraEntity> * (ZCameraManagerMain * th, TEntityRef<ZCameraEntity> & result)
    );

    PATTERN_RELATIVE_FUNCTION(
        "\xE8\x00\x00\x00\x00\xC5\xFA\x7E\x53\x64\xC5\xFA\x10\x4B\x6C\xC5\xFA\x10\x05\x00\x00\x00\x00\x48\x8B\x5C\x24\x40",
        "x????xxxxxxxxxxxxxx????xxxxx", ZSpatialEntity_UpdateCachedWorldMat, void(const ZSpatialEntity* th)
    );

    PATTERN_FUNCTION(
        "\x40\x53\x48\x83\xEC\x00\x48\x8B\x0D\x00\x00\x00\x00\x48\x8B\x01\xFF\x90\x18\x04\x00\x00\x48\x8B\xD8\x48\x85\xC0\x74\x00\x48\x8B\x08\x48\x8B"
        "\x51\x28\x48\x8B\xC8\xFF\xD2\x84\xC0\x74\x00\x48\x8B\x0B\x48\x8B\x51\x68\x48\x8B\xCB\xFF\xD2\x83\xF8\x00\x75",
        "xxxxx?xxx????xxxxxxxxxxxxxxxx?xxxxxxxxxxxxxxx?xxxxxxxxxxxxxx?x", GetCurrentCamera, ZCameraEntity * ()
    );

    PATTERN_FUNCTION(
        "\x48\x89\x5C\x24\x10\x48\x89\x6C\x24\x18\x56\x57\x41\x56\x48\x83\xEC\x00\x45\x0F\xB6\xF1", "xxxxxxxxxxxxxxxxx?xxxx",
        ZUpdateEventContainer_AddDelegate,
        void(ZUpdateEventContainer * th, const ZDelegate<void(const SGameUpdateEvent&)>& callback, int32_t nPriority, EUpdateMode eUpdateMode)
    );

    PATTERN_FUNCTION(
        "\x44\x88\x4C\x24\x20\x53\x55\x56\x41\x54", "xxxxxxxxxx", ZUpdateEventContainer_RemoveDelegate,
        void(ZUpdateEventContainer * th, const ZDelegate<void(const SGameUpdateEvent&)>& callback, int32_t nPriority, EUpdateMode eUpdateMode)
    );

    PATTERN_FUNCTION(
        "\x40\x53\x48\x83\xEC\x00\x48\x83\x79\x08\x00\x48\x8B\xD9\x75\x00\xE8\x00\x00\x00\x00\x48\x8B\xCB\x48\x89\x43\x08\xE8\x00\x00\x00\x00\x48\x83"
        "\x3B\x00\x75\x00\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x8B\x03\x48\x85\xC0\x74\x00\x80\x78\x38\x00\x74\x00\xC5\xFA\x10\x40\x58",
        "xxxxx?xxxx?xxxx?x????xxxxxxxx????xxx?x?xxxx????xxxxxxx?xxx?x?xxxxx", ZInputAction_Analog, float(ZInputAction * th)
    );

    PATTERN_FUNCTION(
        "\x40\x53\x48\x83\xEC\x00\x48\x83\x79\x08\x00\x48\x8B\xD9\x75\x00\xE8\x00\x00\x00\x00\x48\x8B\xCB\x48\x89\x43\x08\xE8\x00\x00\x00\x00\x48\x83"
        "\x3B\x00\x75\x00\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x8B\x03\x48\x85\xC0\x74\x00\x80\x78\x38\x00\x74\x00\xC5\xF8\x57\xC0",
        "xxxxx?xxxx?xxxx?x????xxxxxxxx????xxx?x?xxxx????xxxxxxx?xxx?x?xxxx", ZInputAction_Digital, bool(ZInputAction * th)
    );

    PATTERN_FUNCTION(
        "\x48\x89\x5C\x24\x18\x48\x89\x6C\x24\x20\x41\x54\x41\x56\x41\x57\x48\x83\xEC\x00\x48\x8B\x29", "xxxxxxxxxxxxxxxxxxx?xxx", SetPropertyValue,
        bool(ZEntityType * entity, uint32_t propertyId, const ZObjectRef& value, bool invokeChangeHandlers)
    );

    PATTERN_FUNCTION("\x40\x53\x48\x83\xEC\x00\x33\xDB\x89\x5C\x24\x40", "xxxxx?xxxxxx", GetGlobalInputContext, ZInputContext * ());

    PATTERN_FUNCTION(
        "\x48\x8B\xC4\x48\x89\x50\x10\x48\x89\x48\x08\x55\x48\x8D\x68\xA1\x48\x81\xEC\x00\x00\x00\x00\x48\x89\x58\xF0", "xxxxxxxxxxxxxxxxxxx????xxxx",
        AddBindings, void(const char* binds, ZInputContext* inputContext)
    );
}

zknt::Functions::~Functions() {
    delete ZEntityManager_NewEntity;
    delete ZEntityManager_DeleteEntity;
    delete SEntityCreateInfo_SEntityCreateInfo;
    delete ZCameraManagerMain_GetActiveMainCamera;
    delete ZSpatialEntity_UpdateCachedWorldMat;
    delete GetCurrentCamera;
    delete ZUpdateEventContainer_AddDelegate;
    delete ZUpdateEventContainer_RemoveDelegate;
    delete ZInputAction_Analog;
    delete ZInputAction_Digital;
    delete SetPropertyValue;
    delete GetGlobalInputContext;
}
