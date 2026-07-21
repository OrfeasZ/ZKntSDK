#include "ModSDK.hpp"

#include <filesystem>

#include <simdjson.h>

#include <Glacier/ZResource.hpp>

#include <ResourceLib_KNT.h>

#include <Globals.hpp>
#include <Functions.hpp>
#include <Logging.hpp>

#include <Glacier/CompileReflection.hpp>

static void WaitForResources() {
    while (!SDK()->Globals()->ResourceManager->DoneLoading()) {
        Logger::Debug("Waiting for resources to load (left: {})!", SDK()->Globals()->ResourceManager->m_nNumProcessing);
        SDK()->Globals()->ResourceManager->Update(true);
    }
}

namespace zknt {
    std::tuple<ZResourceIndex, ZRuntimeResourceID>
    ModSDK::LoadResourceFromBIN1(ResourceMem* p_ResourceMem, std::string_view p_MetaJson, std::function<void(ZResourcePending*)> p_Install) {
        // Parse meta, create resource, and register references.
        simdjson::ondemand::parser s_Parser;
        const auto s_Json = simdjson::padded_string(p_MetaJson);
        simdjson::ondemand::document s_JsonMsg = s_Parser.iterate(s_Json);

        const std::string s_ResIdStr{std::string_view(s_JsonMsg["id"])};
        const auto s_ResId = ZRuntimeResourceID::FromString(s_ResIdStr);

        std::string_view s_TypeStr = std::string_view(s_JsonMsg["type"]);

        uint32_t s_Type =
            (static_cast<uint32_t>(static_cast<uint8_t>(s_TypeStr[0])) << 24) | (static_cast<uint32_t>(static_cast<uint8_t>(s_TypeStr[1])) << 16)
            | (static_cast<uint32_t>(static_cast<uint8_t>(s_TypeStr[2])) << 8) | (static_cast<uint32_t>(static_cast<uint8_t>(s_TypeStr[3])));

        Logger::Debug("Loading resource {}...", s_ResId);

        // Create a new resource index.
        ZResourceIndex s_Index;
        SDK()->Functions()->ZResourceContainer_AddResourceInternal->Call(*SDK()->Globals()->ResourceContainer, s_Index, s_ResId);

        Logger::Debug("Got resource index {} for rid {}.", s_Index.val, s_ResId);
        Logger::Debug("Collecting references from meta...");

        std::vector<std::pair<ZRuntimeResourceID, SResourceReferenceFlags>> s_References;
        for (auto s_Ref : s_JsonMsg["references"]) {
            const std::string_view s_ResourceId = s_Ref.get_string();

            const auto s_RefId = ZRuntimeResourceID::FromString(std::string(s_ResourceId));

            s_References.emplace_back(s_RefId, SResourceReferenceFlags{.flags = 0x1F});
        }

        Logger::Debug("Found {} references!", s_References.size());

        auto& s_ResInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[s_Index.val];
        s_ResInfo.dataOffset = 0;
        s_ResInfo.dataSize = p_ResourceMem->DataSize;
        s_ResInfo.compressedDataSize = p_ResourceMem->DataSize;
        s_ResInfo.refCount = 2; // One reference held by the output TResourcePtr, one by ZResourcePending during installation.
        s_ResInfo.firstReferenceIndex = (*SDK()->Globals()->ResourceContainer)->m_references.size();
        s_ResInfo.numReferences = s_References.size();
        s_ResInfo.resourceType = s_Type;

        for (const auto& [s_RefId, s_RefFlags] : s_References) {
            Logger::Debug("Adding reference {} -> {} (flags = {:x}).", s_ResId, s_RefId, s_RefFlags.flags);
            SDK()->Functions()->ZResourceContainer_AddResourceReferenceInternal->Call(*SDK()->Globals()->ResourceContainer, s_RefId, s_RefFlags);
        }

        SDK()->Functions()->ZResourceContainer_AcquireResourceReferences->Call(*SDK()->Globals()->ResourceContainer, s_Index);
        WaitForResources();

        Logger::Debug("All references loaded! Creating readers and installing resource...");

        auto* s_Buffer = static_cast<ZResourceDataBuffer*>(
            (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->AllocateAligned(sizeof(ZResourceDataBuffer), alignof(ZResourceDataBuffer))
        );

        new (s_Buffer) ZResourceDataBuffer();

        s_Buffer->m_pData = const_cast<void*>(p_ResourceMem->ResourceData);
        s_Buffer->m_nSize = p_ResourceMem->DataSize;
        s_Buffer->m_iRefCount = 1;
        s_Buffer->m_nCapacity = p_ResourceMem->DataSize;
        s_Buffer->m_bOwnsDataPtr = false;

        ZResourceDataPtr s_DataPtr{.m_pObject = s_Buffer};

        auto* s_Reader = static_cast<ZResourceReader*>(
            (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->AllocateAligned(sizeof(ZResourceReader), alignof(ZResourceReader))
        );

        SDK()->Functions()->ZResourceReader_ZResourceReader->Call(s_Reader, s_Index, &s_DataPtr, p_ResourceMem->DataSize);

        ZResourcePending s_Pending{};
        s_Pending.m_pResource.m_ResourceIndex = s_Index.val;
        s_Pending.m_pResourceReader.m_pObject = s_Reader;
        s_Pending.m_pResourceReader.m_pObject->m_iRefCount = 1;

        // Increment m_nNumProcessing by 1 because installing will set the
        // resource status to valid, which will decrement m_nNumProcessing.
        InterlockedIncrement(&SDK()->Globals()->ResourceManager->m_nNumProcessing);
        p_Install(&s_Pending);

        WaitForResources();

        SDK()->Functions()->ZResourceReader_Dtor->Call(s_Reader);

        return std::make_tuple(s_Index, s_ResId);
    }

    bool ModSDK::LoadCppEntity(
        const ZString& p_BlueprintJson, const ZString& p_BlueprintMetaJson, const ZString& p_EntityJson, const ZString& p_EntityMetaJson,
        TResourcePtr<ZCppEntityBlueprintFactory>& p_OutBlueprintFactory, TResourcePtr<ZCppEntityFactory>& p_OutTemplateFactory
    ) {
        Logger::Debug("Generating BIN1 resources from RT JSON...");

        const auto s_ResourceCpptMem =
            KNT_GetGeneratorForResource("CPPT")->FromJsonStringToResourceMem(p_EntityJson.c_str(), p_EntityJson.size(), false);

        const auto s_ResourceCbluMem =
            KNT_GetGeneratorForResource("CBLU")->FromJsonStringToResourceMem(p_BlueprintJson.c_str(), p_BlueprintJson.size(), false);

        std::string s_FactoryMetaJson{p_EntityMetaJson.c_str(), p_EntityMetaJson.size()};
        std::string s_BlueprintMetaJson{p_BlueprintMetaJson.c_str(), p_BlueprintMetaJson.size()};

        if (!s_ResourceCbluMem || !s_ResourceCpptMem) {
            Logger::Error("Failed to generate editor resources.");

            return false;
        }

        Logger::Debug("Creating CBLU resource...");

        auto [s_CbluIndex, s_CbluId] =
            LoadResourceFromBIN1(s_ResourceCbluMem, s_BlueprintMetaJson, [&p_OutBlueprintFactory, s_ResourceCbluMem](ZResourcePending* r) {
                const bool s_IsInstalled = SDK()->Functions()->ZCppEntityBlueprintInstaller_Install->Call(nullptr, r);

                if (s_IsInstalled) {
                    p_OutBlueprintFactory.m_ResourceIndex.val = r->m_pResource.m_ResourceIndex.val;
                }

                KNT_GetGeneratorForResource("CBLU")->FreeResourceMem(s_ResourceCbluMem);
            });

        Logger::Debug("CBLU rid = {}, index = {}", s_CbluId, s_CbluIndex.val);

        Logger::Debug("Creating CPPT resource...");

        auto [s_CpptIndex, s_CpptId] =
            LoadResourceFromBIN1(s_ResourceCpptMem, s_FactoryMetaJson, [&p_OutTemplateFactory, s_ResourceCpptMem](ZResourcePending* r) {
                const bool s_IsInstalled = SDK()->Functions()->ZCppEntityTypeInstaller_Install->Call(nullptr, r);

                if (s_IsInstalled) {
                    p_OutTemplateFactory.m_ResourceIndex.val = r->m_pResource.m_ResourceIndex.val;
                }

                KNT_GetGeneratorForResource("CPPT")->FreeResourceMem(s_ResourceCpptMem);
            });

        Logger::Debug("CPPT rid = {}, index = {}", s_CpptId, s_CpptIndex.val);

        return true;
    }
}
