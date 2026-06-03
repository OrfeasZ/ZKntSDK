#pragma once

#include <Common.hpp>
#include "ZResource.hpp"
#include "Reflection.hpp"
#include "ZTypeRegistry.hpp"
#include "ZObject.hpp"
#include <IModSDK.hpp>
#include <Functions.hpp>

class ZEntityRef;

struct SPropertyData {
    SNamedPropertyInfo* GetPropertyInfo() const {
        return reinterpret_cast<SNamedPropertyInfo*>(reinterpret_cast<uintptr_t>(m_pPropertyInfo) - offsetof(SNamedPropertyInfo, m_propertyInfo));
    }

    SPropertyInfo* m_pPropertyInfo; // 0x0
    int64_t m_nPropertyOffset;      // 0x8
    uint32_t m_nPropertyID;         // 0x10
    uint32_t m_nPropertyFlags;      // 0x14

    /*
     * Assigned from the extraData field of SExtendedCppEntityProperty.
     *
     * May contain, for example, the runtime resource ID of a material instance.
     */
    uint64_t m_nExtraData; // 0x18

    /*
     * Property offset which is calculated in:
     *  - ZTemplateEntityBlueprintFactory::CalculateEntityTypes (property aliases)
     *  - ZAspectEntityBlueprintFactory::CreateEntityType (properties of aspect-referenced entities)
     *
     * Copied into:
     *  - SDirectlySettableProperty::unk
     *  - SDirectlySettablePropertyWithSetter::unk
     *
     * Used in:
     *  - ZTemplateEntityFactory::ConfigureEntity
     */
    int32_t m_nUnk; // 0x20
};

struct SInterfaceData {
    STypeID* m_Type;            // 0x0
    int64_t m_nInterfaceOffset; // 0x8
};

struct SExposedEntityData {
    ZString m_sExposedEntityName;     // 0x0
    bool m_bIsArray;                  // 0x10
    TArray<int64_t> m_aEntityOffsets; // 0x18
};

struct SSubsetData {
    ZString m_sSubsetName;                                 // 0x0
    uint32 m_nSubsetFlags;                                 // 0x10
    TArray<int64_t> m_aEntityOffsets;                      // 0x18
    TArray<TPair<int64_t, SSubsetData*>> m_aEntitySubsets; // 0x30
};

struct SPinData {
    int64_t m_nOffsetToThisPtr; // 0x0
    SPinInfo m_pInfo;           // 0x8
};

class ZEntityType {
  public:
    SPropertyData* FindProperty(uint32_t p_PropertyId) const {
        if (!m_pPropertyData) {
            return nullptr;
        }

        auto s_Property = std::find_if(m_pPropertyData->begin(), m_pPropertyData->end(), [p_PropertyId](const SPropertyData& p_Property) {
            return p_Property.m_nPropertyID == p_PropertyId;
        });

        if (s_Property != m_pPropertyData->end()) {
            return s_Property;
        }

        return nullptr;
    }

    int32_t m_nBorrowedPointersMask;                  // 0x0
    TArray<SPropertyData>* m_pPropertyData;           // 0x8
    TArray<SPropertyData>* m_pResettablePropertyData; // 0x10
    TArray<SPropertyData>* m_pStreamablePropertyData; // 0x18
    TArray<SInterfaceData>* m_pInterfaceData;         // 0x20
    TArray<SExposedEntityData>* m_pExposedEntityData; // 0x28
    TArray<SSubsetData>* m_pSubsets;                  // 0x30
    TArray<SPinData>* m_pInputPins;                   // 0x38
    TArray<SPinData>* m_pOutputPins;                  // 0x40
    uint64_t m_nEntityID;                             // 0x48
};

class IEntity : public IComponentInterface {
  public:
    virtual ~IEntity() = 0;
    virtual ZEntityRef* GetID(ZEntityRef& result) const = 0;
    virtual void Activate(int) = 0;
    virtual void Deactivate(int) = 0;
    virtual void IEntity_unk8() = 0;
    virtual void IEntity_unk9() = 0;
};

// Size = 0x18
class ZEntityImpl : public IEntity {
  public:
    enum class EEntityFlags {
        ENTITYFLAG_INITIALIZED = 1,
        ENTITYFLAG_POSTINITIALIZED = 2,
        ENTITYFLAG_ACTIVATED = 4,
        ENTITYFLAG_PREDELETED = 8,
        ENTITYFLAG_EDITMODE = 16,
        ENTITYFLAG_READONLY_FLAG_SET = 32,
        ENTITYFLAG_READONLY = 64
    };

    virtual ~ZEntityImpl() = 0;
    virtual void ZEntityImpl_unk10() = 0;
    virtual void ZEntityImpl_unk11() = 0;
    virtual void ZEntityImpl_unk12() = 0;
    virtual void ZEntityImpl_unk13() = 0;
    virtual void ZEntityImpl_unk14() = 0;
    virtual void ZEntityImpl_unk15() = 0;
    virtual void ZEntityImpl_unk16() = 0;

    inline ZEntityType* GetType() const {
        if ((reinterpret_cast<ptrdiff_t>(m_pEntityType) & 1) == 0) {
            return m_pEntityType;
        }

        return *reinterpret_cast<ZEntityType**>(reinterpret_cast<intptr_t>(&m_pEntityType) + (reinterpret_cast<ptrdiff_t>(m_pEntityType) >> 1));
    }

    ZEntityType* m_pEntityType; // 0x8
    uint32_t m_nEntityPtrIndex; // 0x10
    uint32_t m_nEntityFlags;    // 0x14
};

class ZEntityRef {
  public:
    ZEntityRef() {}

    ZEntityRef(ZEntityType** p_EntityRef) : m_pObj(p_EntityRef) {}

    bool operator==(const ZEntityRef& p_Other) const {
        return GetEntity() == p_Other.GetEntity();
    }

    operator bool() const {
        return GetEntity() != nullptr;
    }

    ZEntityImpl* GetEntity() const {
        if (!m_pObj) {
            return nullptr;
        }

        auto s_RealPtr = reinterpret_cast<uintptr_t>(m_pObj) - sizeof(uintptr_t);
        return reinterpret_cast<ZEntityImpl*>(s_RealPtr);
    }

    ZEntityImpl* operator->() const {
        return GetEntity();
    }

    template<typename T> T* QueryInterface() const {
        const auto s_Entity = GetEntity();

        if (!s_Entity || !*SDK()->Globals()->TypeRegistry || !s_Entity->GetType()) {
            return nullptr;
        }

        const auto s_TypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID(ZHMTypeName<T>);

        if (!s_TypeID) {
            return nullptr;
        }

        for (const auto& s_Interface : *s_Entity->GetType()->m_pInterfaceData) {
            if (s_Interface.m_Type == s_TypeID) {
                return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(m_pObj) + s_Interface.m_nInterfaceOffset);
            }
        }

        return nullptr;
    }

    template<typename T> T* QueryInterface(STypeID* p_TypeID) const {
        const auto s_Entity = GetEntity();

        if (!s_Entity || !*SDK()->Globals()->TypeRegistry || !s_Entity->GetType()) {
            return nullptr;
        }

        for (const auto& s_Interface : *s_Entity->GetType()->m_pInterfaceData) {
            if (s_Interface.m_Type == p_TypeID) {
                return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(m_pObj) + s_Interface.m_nInterfaceOffset);
            }
        }

        return nullptr;
    }

    void* QueryInterface(const STypeID* p_TypeID) const {
        const auto s_Entity = GetEntity();

        if (!s_Entity || !*SDK()->Globals()->TypeRegistry || !s_Entity->GetType()) {
            return nullptr;
        }

        for (const auto& s_Interface : *s_Entity->GetType()->m_pInterfaceData) {
            if (s_Interface.m_Type == p_TypeID) {
                return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_pObj) + s_Interface.m_nInterfaceOffset);
            }
        }

        return nullptr;
    }

    template<typename T> bool HasInterface() const {
        const auto s_Entity = GetEntity();

        if (!s_Entity || !*SDK()->Globals()->TypeRegistry) {
            return false;
        }

        const auto s_TypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID(ZHMTypeName<T>);

        if (!s_TypeID) {
            return false;
        }

        for (const auto& s_Interface : *s_Entity->GetType()->m_pInterfaceData) {
            if (s_Interface.m_Type == s_TypeID) {
                return true;
            }
        }

        return false;
    }

    bool HasInterface(const ZString& p_TypeName) const {
        const auto s_Entity = GetEntity();

        if (!s_Entity || !*SDK()->Globals()->TypeRegistry || !s_Entity->GetType()) {
            return false;
        }

        const auto s_TypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID(p_TypeName);

        if (!s_TypeID) {
            return false;
        }

        for (const auto& s_Interface : *s_Entity->GetType()->m_pInterfaceData) {
            if (s_Interface.m_Type == s_TypeID) {
                return true;
            }
        }

        return false;
    }

    template<typename T> ZVariant<T> GetProperty(const uint32_t nPropertyID) const {
        ZVariant<T> s_PropertyValue;

        const auto s_Entity = GetEntity();

        if (!s_Entity || !*SDK()->Globals()->MemoryManager) {
            return s_PropertyValue;
        }

        const auto s_Type = s_Entity->GetType();

        if (!s_Type || !s_Type->m_pPropertyData) {
            return s_PropertyValue;
        }

        for (uint32_t i = 0; i < s_Type->m_pPropertyData->size(); ++i) {
            const SPropertyData* s_Property = &(*s_Type->m_pPropertyData)[i];

            if (s_Property->m_nPropertyID != nPropertyID) {
                continue;
            }

            const auto* s_PropertyInfo = s_Property->GetPropertyInfo();
            const auto s_PropertyAddress = reinterpret_cast<uintptr_t>(m_pObj) + s_Property->m_nPropertyOffset;

            const uint16_t s_TypeSize = s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->m_nTypeSize;
            const uint16_t s_TypeAlignment = s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->m_nTypeAlignment;

            auto* s_Data = (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->AllocateAligned(s_TypeSize, s_TypeAlignment);

            if (s_PropertyInfo->m_propertyInfo.m_Flags & EPropertyInfoFlags::E_HAS_GETTER_SETTER) {
                s_PropertyInfo->m_propertyInfo.m_PropetyGetter(
                    reinterpret_cast<void*>(s_PropertyAddress), s_Data, s_PropertyInfo->m_propertyInfo.m_nExtraData
                );
            }
            else {
                s_PropertyInfo->m_propertyInfo.m_Type->GetTypeInfo()->m_pTypeFunctions->placementCopyConstruct(
                    s_Data, reinterpret_cast<void*>(s_PropertyAddress)
                );
            }

            s_PropertyValue.UNSAFE_Assign(s_PropertyInfo->m_propertyInfo.m_Type, s_Data);

            break;
        }

        return s_PropertyValue;
    }

    template<typename T> ZVariant<T> GetProperty(const ZString& p_PropertyName) const {
        return GetProperty<T>(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()));
    }

    bool SetProperty(uint32_t p_PropertyId, const ZObjectRef& p_Value, bool p_InvokeChangeHandlers = true) {
        return SDK()->Functions()->SetPropertyValue->Call(*m_pObj, p_PropertyId, p_Value, p_InvokeChangeHandlers);
    }

    bool SetProperty(const ZString& p_PropertyName, const ZObjectRef& p_Value, bool p_InvokeChangeHandlers = true) {
        return SetProperty(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()), p_Value, p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(uint32_t p_PropertyId, const T& p_Value, bool p_InvokeChangeHandlers = true) {
        return SetProperty(p_PropertyId, ZVariant<T>(p_Value), p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(const ZString& p_PropertyName, const T& p_Value, bool p_InvokeChangeHandlers = true) {
        return SetProperty<T>(Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()), p_Value, p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(uint32_t p_PropertyId, const ZVariant<T>& p_Value, bool p_InvokeChangeHandlers = true) {
        return SetProperty(p_PropertyId, static_cast<const ZObjectRef&>(p_Value), p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(const ZString& p_PropertyName, const ZVariant<T>& p_Value, bool p_InvokeChangeHandlers = true) {
        return SetProperty<T>(
            Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()), static_cast<const ZObjectRef&>(p_Value), p_InvokeChangeHandlers
        );
    }

    template<typename T> bool SetProperty(uint32_t p_PropertyId, const ZVariantRef<T>& p_Value, bool p_InvokeChangeHandlers = true) {
        return SetProperty(p_PropertyId, static_cast<const ZObjectRef&>(p_Value), p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(const ZString& p_PropertyName, const ZVariantRef<T>& p_Value, bool p_InvokeChangeHandlers = true) {
        return SetProperty<T>(
            Hash::Crc32(p_PropertyName.c_str(), p_PropertyName.size()), static_cast<const ZObjectRef&>(p_Value), p_InvokeChangeHandlers
        );
    }

    ZEntityType** m_pObj = nullptr; // 0x0
    uint32_t m_EntityIndex = 0;     // 0x8
    uint32_t m_Unk = 0;             // 0xC
};

template<typename T> class TEntityRef {
  public:
    TEntityRef() = default;

    explicit TEntityRef(ZEntityRef p_Ref) : m_entityRef(p_Ref), m_pInterfaceRef(p_Ref.QueryInterface<T>()) {}

    operator bool() const {
        return m_entityRef && m_pInterfaceRef != nullptr;
    }

    ZEntityRef m_entityRef;
    T* m_pInterfaceRef = nullptr;
};

struct SEntityCreateInfo {
    ZResourcePtr m_EntityFactory; // 0x0
    ZString m_Unk;                // 0x8
    uint64_t m_EntityID;          // 0x18
    ZString m_EntityName;         // 0x20
    ZEntityRef m_TransformParent; // 0x30
    PAD(0x7C);
};
