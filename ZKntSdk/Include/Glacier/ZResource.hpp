#pragma once

#include "ZResourceID.hpp"
#include "TNonReallocatingArray.hpp"
#include "TArray.hpp"
#include "THashMap.hpp"
#include "THashSet.hpp"
#include "ZSharedPointerTarget.hpp"
#include "TSharedPointer.hpp"
#include "TMap.hpp"

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

struct SResourceReferenceFlags {
    union {
        struct {
            uint8_t languageCode : 5;
            uint8_t acquired : 1;
            int8_t referenceType : 2;
        };

        uint8_t flags;
    };
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
        m_ResourceIndex.val = -1;
    }

    ZResourcePtr(const ZResourcePtr& p_Other) {
        m_ResourceIndex = p_Other.m_ResourceIndex;

        if (m_ResourceIndex.val != -1) {
            auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_ResourceIndex.val];

            InterlockedIncrement(&s_ResourceInfo.refCount);
        }
    }

    ZResourcePtr(ZResourceIndex p_ResourceIndex) {
        m_ResourceIndex = p_ResourceIndex;

        if (m_ResourceIndex.val != -1) {
            auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_ResourceIndex.val];

            InterlockedIncrement(&s_ResourceInfo.refCount);
        }
    }

    ~ZResourcePtr() {
        if (m_ResourceIndex.val < 0) {
            return;
        }

        auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_ResourceIndex.val];

        if (InterlockedDecrement(&s_ResourceInfo.refCount) == 0 && s_ResourceInfo.resourceData) {
            SDK()->Functions()->ZResourceManager_UninstallResource->Call(SDK()->Globals()->ResourceManager, m_ResourceIndex);
        }
    }

    bool Exists() const {
        return m_ResourceIndex.val != -1;
    }

  public:
    ZResourceContainer::SResourceInfo& GetResourceInfo() const {
        auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_ResourceIndex.val];

        return s_ResourceInfo;
    }

    void* GetResourceData() const {
        if (m_ResourceIndex.val < 0) {
            return nullptr;
        }

        auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_ResourceIndex.val];

        return s_ResourceInfo.resourceData;
    }

    operator bool() const {
        return GetResourceData() != nullptr;
    }

  public:
    ZResourceIndex m_ResourceIndex;
    uint32_t m_Padding = 0;
};

static_assert(sizeof(ZResourcePtr) == 8);

template<typename T> class TResourcePtr : public ZResourcePtr {
  public:
    TResourcePtr() = default;

    explicit TResourcePtr(const ZResourceIndex p_Index) {
        m_ResourceIndex = p_Index;
    }

  public:
    T* GetResource() const {
        return static_cast<T*>(GetResourceData());
    }

    operator T*() const {
        return GetResource();
    }
};

class IResourceInstaller;

class ZResourceManager : public IComponentInterface {
  public:
    virtual ~ZResourceManager() {}
    virtual void ZResourceManager_unk5() = 0;
    virtual void ZResourceManager_unk6() = 0;
    virtual void ZResourceManager_unk7() = 0;
    virtual ZResourcePtr* GetResourcePtr(ZResourcePtr& result, const ZRuntimeResourceID& ridResource, int nPriority) = 0;
    virtual ZResourcePtr* LoadResource(ZResourcePtr& result, const ZRuntimeResourceID& ridResource) = 0;
    virtual void ZResourceManager_unk10() = 0;
    virtual void ZResourceManager_unk11() = 0;
    virtual void ZResourceManager_unk12() = 0;
    virtual void ZResourceManager_unk13() = 0;
    virtual void ZResourceManager_unk14() = 0;
    virtual void ZResourceManager_unk15() = 0;
    virtual void ZResourceManager_unk16() = 0;
    virtual void ZResourceManager_unk17() = 0;
    virtual void ZResourceManager_unk18() = 0;
    virtual void ZResourceManager_unk19() = 0;
    virtual void ZResourceManager_unk20() = 0;
    virtual void ZResourceManager_unk21() = 0;
    virtual void ZResourceManager_unk22() = 0;
    virtual void ZResourceManager_unk23() = 0;
    virtual void ZResourceManager_unk24() = 0;
    virtual void Update(bool bSendStatusChangedNotifications) = 0;
    virtual void ZResourceManager_unk26() = 0;
    virtual void ZResourceManager_unk27() = 0;
    virtual void ZResourceManager_unk28() = 0;
    virtual void ZResourceManager_unk29() = 0;
    virtual void ZResourceManager_unk30() = 0;
    virtual void ZResourceManager_unk31() = 0;
    virtual void ZResourceManager_unk32() = 0;
    virtual void ZResourceManager_unk33() = 0;
    virtual void ZResourceManager_unk34() = 0;
    virtual void ZResourceManager_unk35() = 0;
    virtual void ZResourceManager_unk36() = 0;
    virtual void ZResourceManager_unk37() = 0;
    virtual void ZResourceManager_unk38() = 0;
    virtual void ZResourceManager_unk39() = 0;
    virtual void ZResourceManager_unk40() = 0;
    virtual void ZResourceManager_unk41() = 0;
    virtual void ZResourceManager_unk42() = 0;
    virtual void ZResourceManager_unk43() = 0;
    virtual void ZResourceManager_unk44() = 0;
    virtual void ZResourceManager_unk45() = 0;
    virtual void ZResourceManager_unk46() = 0;
    virtual void ZResourceManager_unk47() = 0;
    virtual void ZResourceManager_unk48() = 0;
    virtual void ZResourceManager_unk49() = 0;
    virtual void ZResourceManager_unk50() = 0;
    virtual void ZResourceManager_unk51() = 0;
    virtual bool DoneLoading() = 0;

    PAD(0x68);                                                // 0x8
    TMap<uint32_t, IResourceInstaller*> m_ResourceInstallers; // 0x70
    PAD(0x124);                                               // 0x98
    volatile LONG m_nNumProcessing;                           // 0x1BC
};

class ZResourceDataBuffer : public ZSharedPointerTarget {
  public:
    void* m_pData = nullptr;
    uint32 m_nSize = 0;
    uint32 m_nCapacity = 0;
    bool m_bOwnsDataPtr = false;
};

using ZResourceDataPtr = TSharedPointer<ZResourceDataBuffer>;

class ZResourceReader : public ZSharedPointerTarget {
  public:
    ZResourceIndex m_ResourceIndex;
    ZResourceDataPtr m_pResourceData;
    uint32 m_nResourceDataSize = 0;
    TArray<ZResourceIndex> m_ReferenceIndices;
    TArray<SResourceReferenceFlags> m_ReferenceFlags;
};

static_assert(sizeof(ZResourceReader) == 88);

using ZResourceReaderPtr = TSharedPointer<ZResourceReader>;

class ZResourcePending {
  public:
    ZResourcePtr m_pResource;
    ZResourceReaderPtr m_pResourceReader;
};

class IPackageManager {
  public:
    enum EPartitionType {
        Standard,
        Addon,
    };

    struct SPartitionInfo {
        int32_t m_nIndex;                  // 0
        ZString m_sPartitionID;            // 8
        EPartitionType m_eType;            // 24
        int32_t m_patchLevel;              // 28
        uint64_t a32;                      // 32
        uint64_t a40;                      // 40
        ZString m_sMountPath;              // 48
        uint64_t a64;                      // 64
        bool a72;                          // 72
        SPartitionInfo* m_pParent;         // 80
        TArray<SPartitionInfo*> m_aAddons; // 88
    };

    virtual ~IPackageManager() {}
};

static_assert(sizeof(IPackageManager::SPartitionInfo) == 112);
