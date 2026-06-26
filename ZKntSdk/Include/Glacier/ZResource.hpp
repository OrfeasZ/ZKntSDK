#pragma once

#include "ZResourceID.hpp"
#include "TNonReallocatingArray.hpp"
#include "TArray.hpp"
#include "THashMap.hpp"
#include "THashSet.hpp"
#include <Globals.hpp>
#include <IModSDK.hpp>
#include <Functions.hpp>

class ZResourceIndex {
  public:
    ZResourceIndex() : val(-1) {}
    ZResourceIndex(int val) : val(val) {}

    int32_t val;
};

enum EResourceStatus {
    RESOURCE_STATUS_UNKNOWN = 0,
    RESOURCE_STATUS_LOADING = 1,
    RESOURCE_STATUS_INSTALLING = 2,
    RESOURCE_STATUS_FAILED = 3,
    RESOURCE_STATUS_VALID = 4,
};

class ZResourceContainer {
  public:
    struct SResourceInfo {
        ZRuntimeResourceID rid;
        void* resourceData;
        unsigned long long dataOffset;
        uint32_t dataSize;
        uint32_t compressedDataSize;
        EResourceStatus status;
        long refCount;
        ZResourceIndex nextNewestIndex;
        int firstReferenceIndex;
        int numReferences;
        unsigned int resourceType;
        int32_t monitorId;
        short priority;
        int8_t packageId;
    };

    /**
     * Packed reference information.
     * - 8 bits for the flags
     * - 1 bit for some flag that inverts the index (multiplies it by -1), not sure why
     * - 23 bits for the index
     */
    struct SResourceReferenceInfo {
        uint32_t flags : 8;
        uint32_t unknown : 1;
        uint32_t index : 23;
    };

  public:
    TNonReallocatingArray<SResourceInfo, 4194304> m_resources;
    PAD(0x18); // TArray
    TArray<SResourceReferenceInfo> m_references;
    THashMap<ZRuntimeResourceID, ZResourceIndex, TDefaultHashMapPolicy<ZRuntimeResourceID>> m_indices;
    TArray<ZString> m_MountedPackages;
    TArray<uint32_t> m_firstResourceIndexPerPackage;
    TArray<uint32_t> m_firstReferenceIndexPerPackage;
    TArray<uint8_t> m_firstPackageIndexPerMountedPartition;
    uint8_t m_firstDynamicPackageIndex;
    uint8_t m_firstBaseLanguagePackageIndex;
    THashSet<ZRuntimeResourceID, TDefaultHashSetPolicy<ZRuntimeResourceID>> m_dynamicResources;
};

static_assert(sizeof(ZResourceContainer::SResourceInfo) == 64);

class ZResourcePtr {
  public:
    ZResourcePtr() {
        m_nResourceIndex.val = -1;
    }

    ZResourcePtr(const ZResourcePtr& p_Other) {
        m_nResourceIndex = p_Other.m_nResourceIndex;

        if (m_nResourceIndex.val != -1) {
            auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_nResourceIndex.val];

            InterlockedIncrement(&s_ResourceInfo.refCount);
        }
    }

    ZResourcePtr(ZResourceIndex p_ResourceIndex) {
        m_nResourceIndex = p_ResourceIndex;

        if (m_nResourceIndex.val != -1) {
            auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_nResourceIndex.val];

            InterlockedIncrement(&s_ResourceInfo.refCount);
        }
    }

    ~ZResourcePtr() {
        if (m_nResourceIndex.val < 0) {
            return;
        }

        auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_nResourceIndex.val];

        if (InterlockedDecrement(&s_ResourceInfo.refCount) == 0 && s_ResourceInfo.resourceData) {
            SDK()->Functions()->ZResourceManager_UninstallResource->Call(SDK()->Globals()->ResourceManager, m_nResourceIndex);
        }
    }

    bool Exists() const {
        return m_nResourceIndex.val != -1;
    }

  public:
    ZResourceContainer::SResourceInfo& GetResourceInfo() const {
        auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_nResourceIndex.val];

        return s_ResourceInfo;
    }

    void* GetResourceData() const {
        if (m_nResourceIndex.val < 0) {
            return nullptr;
        }

        auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_nResourceIndex.val];

        return s_ResourceInfo.resourceData;
    }

    operator bool() const {
        return GetResourceData() != nullptr;
    }

  public:
    ZResourceIndex m_nResourceIndex;
    uint32_t m_Padding = 0;
};

static_assert(sizeof(ZResourcePtr) == 8);

template<typename T> class TResourcePtr : public ZResourcePtr {
  public:
    TResourcePtr() = default;

    explicit TResourcePtr(const ZResourceIndex p_Index) {
        m_nResourceIndex = p_Index;
    }

  public:
    T* GetResource() const {
        return static_cast<T*>(GetResourceData());
    }

    operator T*() const {
        return GetResource();
    }
};

class ZResourceManager : public IComponentInterface {
  public:
    virtual ~ZResourceManager() {}
    virtual void ZResourceManager_unk5() = 0;
    virtual void ZResourceManager_unk6() = 0;
    virtual void ZResourceManager_unk7() = 0;
    virtual ZResourcePtr* GetResourcePtr(ZResourcePtr& result, const ZRuntimeResourceID& ridResource, int nPriority) = 0;
    virtual ZResourcePtr* LoadResource(ZResourcePtr& result, const ZRuntimeResourceID& ridResource) = 0;
};
