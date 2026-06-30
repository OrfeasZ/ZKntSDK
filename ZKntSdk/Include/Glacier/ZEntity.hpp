#pragma once

#include <Common.hpp>
#include "ZResource.hpp"
#include "Reflection.hpp"
#include "ZTypeRegistry.hpp"
#include "ZObject.hpp"
#include <IModSDK.hpp>
#include <Functions.hpp>
#include <Hooks.hpp>

class ZEntityRef;
class ZEntityBlueprintFactoryBase;

class IEntitySceneContext;

class ZEntityManager : public IComponentInterface {
  public:
    IEntitySceneContext* m_pSceneContext;                                           // 0x8
    PAD(0x8);                                                                       // 0x10
    THashMap<uint64_t, TPair<ZEntityRef, TResourcePtr<IEntityFactory>>> m_Entities; // 0x18
};

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
    uint32_t m_nNameHash;             // 0x10
    bool m_bIsArray;                  // 0x14
    TArray<int64_t> m_aEntityOffsets; // 0x18
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

    SExposedEntityData* GetExposedEntityData(const ZString& p_Name) {
        if (!m_pExposedEntityData) {
            return nullptr;
        }

        for (auto& s_ExposedEntityData : *m_pExposedEntityData) {
            if (s_ExposedEntityData.m_sExposedEntityName == p_Name) {
                return &s_ExposedEntityData;
            }
        }

        return nullptr;
    }

    int32_t m_nBorrowedPointersMask;                  // 0x0
    TArray<SPropertyData>* m_pPropertyData;           // 0x8
    TArray<SPropertyData>* m_pResettablePropertyData; // 0x10
    TArray<SPropertyData>* m_pStreamablePropertyData; // 0x18
    TArray<SInterfaceData>* m_pInterfaceData;         // 0x20
    TArray<SExposedEntityData>* m_pExposedEntityData; // 0x28
    TArray<SPinData>* m_pInputPins;                   // 0x30
    TArray<SPinData>* m_pOutputPins;                  // 0x38
    int64_t m_nOwningEntityOffset;                    // 0x40
    uint64_t m_nEntityID;                             // 0x48
    uint64_t m_Unk;                                   // 0x50 (TArray*)
    int64_t m_nLogicalParentEntityOffset;             // 0x58
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
    int32_t m_nEntityPtrIndex;  // 0x10
    int16_t m_nEntityFlags;     // 0x14
};

class ZEntityRef {
  public:
    ZEntityRef() {}

    ZEntityRef(ZEntityType** p_EntityRef) {
        if (!p_EntityRef) {
            return;
        }

        m_pObj = p_EntityRef;

        // Mirrors the logic of ZEntityImpl::GetID.
        const auto s_EntityImpl = reinterpret_cast<ZEntityImpl*>(reinterpret_cast<uintptr_t>(p_EntityRef) - sizeof(uintptr_t));
        const uint64_t s_EntityRefData = *reinterpret_cast<uint64_t*>(
            reinterpret_cast<uintptr_t>(s_EntityImpl) + sizeof(ZEntityImpl) + static_cast<int32_t>(s_EntityImpl->m_nEntityPtrIndex)
        );

        std::memcpy(&m_EntityIndex, &s_EntityRefData, sizeof(s_EntityRefData));
    }

    ZEntityRef(ZEntityType** p_EntityRef, uint32_t p_EntityIndex, uint32_t p_Unk) : m_pObj(p_EntityRef), m_EntityIndex(p_EntityIndex), m_Unk(p_Unk) {}

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

    ZEntityRef GetLogicalParent() const {
        const auto s_Entity = GetEntity();

        if (!s_Entity || !s_Entity->GetType() || s_Entity->GetType()->m_nLogicalParentEntityOffset == 0) {
            return {};
        }

        return {reinterpret_cast<ZEntityType**>(reinterpret_cast<uintptr_t>(m_pObj) + s_Entity->GetType()->m_nLogicalParentEntityOffset)};
    }

    void SetLogicalParent(ZEntityRef entityRef) {
        const auto s_Entity = GetEntity();
        ZEntityType* s_EntityType = SDK()->Functions()->ZEntityImpl_EnsureUniqueType->Call(s_Entity, 0);

        s_EntityType->m_nLogicalParentEntityOffset = reinterpret_cast<uintptr_t>(entityRef.m_pObj) - reinterpret_cast<uintptr_t>(m_pObj);
    }

    ZEntityRef GetOwningEntity() const {
        const auto s_Entity = GetEntity();

        if (!s_Entity || !s_Entity->GetType() || s_Entity->GetType()->m_nOwningEntityOffset == 0) {
            return {};
        }

        return {reinterpret_cast<ZEntityType**>(reinterpret_cast<uintptr_t>(m_pObj) + s_Entity->GetType()->m_nOwningEntityOffset)};
    }

    bool IsAnyParent(const ZEntityRef& p_Other) const {
        if (!p_Other) {
            return false;
        }

        if (!GetEntity()) {
            return false;
        }

        if (GetLogicalParent() == p_Other || GetOwningEntity() == p_Other) {
            return true;
        }

        return GetLogicalParent().IsAnyParent(p_Other);
    }

    // ZEntityBlueprintFactoryBase* GetBlueprintFactory() const {
    //     const auto* s_Entity = GetEntity();

    //    if (!s_Entity) {
    //        return nullptr;
    //    }

    //    const auto* s_Type = s_Entity->GetType();

    //    if (!s_Type) {
    //        return nullptr;
    //    }

    //    if ((s_Type->m_nBorrowedPointersMask & 0x200) == 0) { // IsRootFactoryEntity or something
    //        return nullptr;
    //    }

    //    auto s_RootEntity = QueryInterface<void>();

    //    if (!s_RootEntity) {
    //        return nullptr;
    //    }

    //    // Pointer to IEntityBlueprintFactory stored right before the start of this entity.
    //    return *reinterpret_cast<ZEntityBlueprintFactoryBase**>(reinterpret_cast<uintptr_t>(s_RootEntity) - sizeof(uintptr_t));
    //}

    ZEntityRef GetExposedEntity(const ZString& p_SubEntityName) {
        SExposedEntityData* s_ExposedEntityData = GetEntity()->GetType()->GetExposedEntityData(p_SubEntityName);

        if (!s_ExposedEntityData || s_ExposedEntityData->m_aEntityOffsets.size() == 0) {
            return {};
        }

        const int64_t s_EntityOffset = s_ExposedEntityData->m_aEntityOffsets[0];
        ZEntityRef s_EntityRef = reinterpret_cast<ZEntityType**>(reinterpret_cast<char*>(m_pObj) + s_EntityOffset);

        ZEntityRef s_Result;
        s_EntityRef.GetEntity()->GetID(s_Result);

        return s_Result;
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
        return SDK()->Functions()->SetPropertyValue->Call(m_pObj, p_PropertyId, p_Value, p_InvokeChangeHandlers);
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

    void SignalInputPin(const ZString& p_PinName, const ZObjectRef& p_Data = ZObjectRef()) const {
        SignalInputPin(Hash::Crc32(p_PinName.c_str(), p_PinName.size()), p_Data);
    }

    void SignalInputPin(uint32_t p_PinId, const ZObjectRef& p_Data = ZObjectRef()) const {
        SDK()->Hooks()->SignalInputPin->Call(m_pObj, p_PinId, p_Data);
    }

    void SignalOutputPin(const ZString& p_PinName, const ZObjectRef& p_Data = ZObjectRef()) const {
        SignalOutputPin(Hash::Crc32(p_PinName.c_str(), p_PinName.size()), p_Data);
    }

    void SignalOutputPin(uint32_t p_PinId, const ZObjectRef& p_Data = ZObjectRef()) const {
        SDK()->Hooks()->SignalOutputPin->Call(m_pObj, p_PinId, p_Data);
    }

    ZEntityType** m_pObj = nullptr; // 0x0
    uint32_t m_EntityIndex = 0;     // 0x8
    uint32_t m_Unk = 0;             // 0xC
};

template<> struct std::hash<ZEntityRef> {
    size_t operator()(const ZEntityRef& p_Ref) const noexcept {
        return reinterpret_cast<uintptr_t>(p_Ref.GetEntity());
    }
};

struct SEntityCreateInfo {
    ZResourcePtr m_EntityFactory; // 0x0
    ZString m_Unk;                // 0x8
    uint64_t m_EntityID;          // 0x18
    ZString m_EntityName;         // 0x20
    ZEntityRef m_TransformParent; // 0x30
    PAD(0x7C);
};

template<typename T> class TEntityRef {
  public:
    TEntityRef() = default;

    explicit TEntityRef(ZEntityRef p_Ref) : m_entityRef(p_Ref), m_pInterfaceRef(p_Ref.QueryInterface<T>()) {}

    operator bool() const {
        return m_entityRef && m_pInterfaceRef != nullptr;
    }

    /**
     * Try to spawn an entity of a given type using the provided runtime resource id.
     * @param p_Rrid The runtime resource id of the entity factory template to use.
     */
    static TEntityRef SpawnEntity(ZRuntimeResourceID p_Rrid) {
        TResourcePtr<IEntityFactory> s_EntityFactory;
        SDK()->Globals()->ResourceManager->LoadResource(s_EntityFactory, p_Rrid);
        if (!s_EntityFactory) {
            return {};
        }

        SEntityCreateInfo s_CreationInfo;
        SDK()->Functions()->SEntityCreateInfo_SEntityCreateInfo->Call(&s_CreationInfo, ZString(), s_EntityFactory, ZEntityRef(), -1);

        TEntityRef s_Ref{};

        SDK()->Functions()->ZEntityManager_NewEntity->Call(SDK()->Globals()->EntityManager, s_Ref.m_entityRef, s_CreationInfo);
        s_Ref.m_pInterfaceRef = s_Ref.m_entityRef.QueryInterface<T>();

        if (!s_Ref.m_pInterfaceRef) {
            return {};
        }

        return std::move(s_Ref);
    }

    T* operator->() {
        return m_pInterfaceRef;
    }

    ZEntityRef m_entityRef;
    T* m_pInterfaceRef = nullptr;
};

template<typename T> class TInterfaceRef {
  public:
    // TODO: Verify. Works for now but not 100% sure on the two ints.
    static TInterfaceRef FromEntityRef(ZEntityRef p_Ref) {
        const auto s_pInterface = p_Ref.QueryInterface<T>();

        if (s_pInterface != nullptr) {
            return {.m_pInterface = s_pInterface, .m_EntityIndex = p_Ref.m_EntityIndex, .m_Unk = p_Ref.m_Unk};
        }

        return {};
    }

    operator bool() const {
        return m_pInterface != nullptr;
    }

    T* m_pInterface = nullptr;
    uint32_t m_EntityIndex = 0;
    uint32_t m_Unk = 0;
};

class IEntityRefValue : public IComponentInterface {};

template<typename T> class ITEntityRefValue : public IEntityRefValue {
  public:
    virtual ZEntityRef* GetEntity(ZEntityRef& result) = 0;
};

class IEntityFactory : public IComponentInterface {
  public:
    virtual void IEntityFactory_unk5() = 0;
    virtual void ConfigureEntity() = 0;
    virtual void IEntityFactory_unk7() = 0;
    virtual ZEntityBlueprintFactoryBase* GetBlueprint() = 0;
    virtual void IEntityFactory_unk9() = 0;
    virtual void IEntityFactory_unk10() = 0;
    virtual void IEntityFactory_unk11() = 0;
    virtual void IEntityFactory_unk12() = 0;
    virtual void IEntityFactory_unk13() = 0;

    bool IsTemplateEntityFactory() const {
        return SDK()->Globals()->ZTemplateEntityFactory_vtbl == *(void**)this;
    }

    bool IsAspectEntityFactory() const {
        return SDK()->Globals()->ZAspectEntityFactory_vtbl == *(void**)this;
    }

    bool IsCppEntityFactory() const {
        return SDK()->Globals()->ZCppEntityFactory_vtbl == *(void**)this;
    }

    bool IsExtendedCppEntityFactory() const {
        return SDK()->Globals()->ZExtendedCppEntityFactory_vtbl == *(void**)this;
    }

    bool IsUIControlEntityFactory() const {
        return SDK()->Globals()->ZUIControlEntityFactory_vtbl == *(void**)this;
    }

    bool IsRenderMaterialEntityFactory() const {
        return SDK()->Globals()->ZRenderMaterialEntityFactory_vtbl == *(void**)this;
    }

    bool IsAudioSwitchEntityFactory() const {
        return SDK()->Globals()->ZAudioSwitchEntityFactory_vtbl == *(void**)this;
    }

    bool IsAudioStateEntityFactory() const {
        return SDK()->Globals()->ZAudioStateEntityFactory_vtbl == *(void**)this;
    }

    bool IsPadEntityFactory() const {
        return SDK()->Globals()->ZPadEntityFactory_vtbl == *(void**)this;
    }

    bool IsShadernodeEntityFactory() const {
        return SDK()->Globals()->ZShadernodeEntityFactory_vtbl == *(void**)this;
    }
};

class IEntityBlueprintFactory : public IComponentInterface {
  public:
    virtual void IEntityBlueprintFactory_unk5() = 0;
    virtual void IEntityBlueprintFactory_unk6() = 0;
    virtual void GetMemoryRequirements(uint32_t&, uint32_t&, int64_t&) = 0;
    virtual void IEntityBlueprintFactory_unk8() = 0;
    virtual void IEntityBlueprintFactory_unk9() = 0;
    virtual void IEntityBlueprintFactory_unk10() = 0;
    virtual void IEntityBlueprintFactory_unk11() = 0;
    virtual void IEntityBlueprintFactory_unk12() = 0;
    virtual void IEntityBlueprintFactory_unk13() = 0;
    virtual void IEntityBlueprintFactory_unk14() = 0;
    virtual void IEntityBlueprintFactory_unk15() = 0;
    virtual bool IEntityBlueprintFactory_unk16() = 0;
    virtual void IEntityBlueprintFactory_unk17() = 0;
    virtual void IEntityBlueprintFactory_unk18() = 0;
    virtual void IEntityBlueprintFactory_unk19() = 0;
    virtual bool IEntityBlueprintFactory_unk20() = 0;
    virtual void IEntityBlueprintFactory_unk21() = 0;
    virtual void IEntityBlueprintFactory_unk22() = 0;
    virtual void IEntityBlueprintFactory_unk23() = 0;
    virtual void IEntityBlueprintFactory_unk24() = 0;
    virtual void IEntityBlueprintFactory_unk25() = 0;
    virtual void IEntityBlueprintFactory_unk26() = 0;
    virtual int64_t GetSubEntitiesCount() = 0;
    virtual void IEntityBlueprintFactory_unk28() = 0;
    virtual void IEntityBlueprintFactory_unk29() = 0;
    virtual void IEntityBlueprintFactory_unk30() = 0;
    virtual void IEntityBlueprintFactory_unk31() = 0;
    virtual ZEntityBlueprintFactoryBase* GetSubEntityBlueprint(int index) = 0;
    virtual uint64_t GetSubEntityId(int index) = 0;
    virtual int GetSubEntityIndex(unsigned long long nEntityID) const = 0;
    virtual ZEntityType** GetSubEntity(ZEntityType**, int index) = 0;

    bool IsTemplateEntityBlueprintFactory() const {
        return SDK()->Globals()->ZTemplateEntityBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsAspectEntityBlueprintFactory() const {
        return SDK()->Globals()->ZAspectEntityBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsCppEntityBlueprintFactory() const {
        return SDK()->Globals()->ZCppEntityBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsExtendedCppEntityBlueprintFactory() const {
        return SDK()->Globals()->ZExtendedCppEntityBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsUIControlBlueprintFactory() const {
        return SDK()->Globals()->ZUIControlBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsRenderMaterialEntityBlueprintFactory() const {
        return SDK()->Globals()->ZRenderMaterialEntityBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsAudioSwitchBlueprintFactory() const {
        return SDK()->Globals()->ZAudioSwitchBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsAudioStateBlueprintFactory() const {
        return SDK()->Globals()->ZAudioStateBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsPadEntityBlueprintFactory() const {
        return SDK()->Globals()->ZPadEntityBlueprintFactory_vtbl == *(void**)this;
    }

    bool IsShadernodeEntityBlueprintFactory() const {
        return SDK()->Globals()->ZShadernodeEntityBlueprintFactory_vtbl == *(void**)this;
    }
};

enum class EVirtualPlatformID : int32_t {
    DEFAULT = 0,
    PC = 256,
    PC_ENHANCED = 257,
    PC_GAMESERVER = 258,
    PC_STEAMDECK = 259,
    PC_ASUSROG = 260,
    PC_ASUSROGX = 261,
    PS5 = 1280,
    PS5_PRO = 1281,
    XBOX_SERIES = 1536,
    XBOX_SERIES_X = 1537,
    OUNCE = 1792,
    DARWIN = 2048,
    DARWIN_IOS = 2049,
    DARWIN_MACOS = 2050,
    VR = 65536,
    VR_PC = 65792,
    VR_PS5 = 66816,
    __SUB = 255,
    __BASE = 65280,
    __SYSTEM = 16711680
};

struct SEntityTemplateReference {
    int32 entityIndex;        // 0x0
    uint64 entityID;          // 0x8
    ZString exposedEntity;    // 0x10
    int32 externalSceneIndex; // 0x20
};

struct SEntityTemplateProperty {
    uint32 nPropertyID; // 0x0
    ZObjectRef value;   // 0x8
};

struct SEntityTemplatePlatformSpecificProperty {
    SEntityTemplateProperty propertyValue; // 0x0
    EVirtualPlatformID platform;           // 0x18
    bool postInit;                         // 0x1C
};

struct STemplateFactorySubEntity {
    SEntityTemplateReference logicalParent;                                         // 0x0
    int32 entityTypeResourceIndex;                                                  // 0x28
    TArray<SEntityTemplateProperty> propertyValues;                                 // 0x30
    TArray<SEntityTemplateProperty> postInitPropertyValues;                         // 0x48
    TArray<SEntityTemplatePlatformSpecificProperty> platformSpecificPropertyValues; // 0x60
};

struct SEntityTemplatePropertyOverride {
    SEntityTemplateReference propertyOwner; // 0x0
    SEntityTemplateProperty propertyValue;  // 0x28
    bool isRTEditable;                      // 0x40
};

struct STemplateEntityFactory {
    int32 subType;                        // 0x0
    int32 blueprintIndexInResourceHeader; // 0x4
    int32 rootEntityIndex;                // 0x8
    uint8_t _padC[4]{};
    ZString sourceResourceID;                                   // 0x10
    TArray<STemplateFactorySubEntity> subEntities;              // 0x20
    TArray<SEntityTemplatePropertyOverride> propertyOverrides;  // 0x38
    TArray<int32> externalSceneTypeIndicesInResourceHeader;     // 0x50
    TArray<ZRuntimeResourceID> externalSceneRuntimeResourceIDs; // 0x68
};

struct SEntityTemplatePropertyAlias {
    ZString sAliasName;    // 0x0
    int32 entityID;        // 0x10
    ZString sPropertyName; // 0x18
};

struct SEntityTemplateExposedEntity {
    ZString sName;                             // 0x0
    bool bIsArray;                             // 0x10
    TArray<SEntityTemplateReference> aTargets; // 0x18
};

struct STemplateBlueprintSubEntity {
    SEntityTemplateReference logicalParent;               // 0x0
    int32 entityTypeResourceIndex;                        // 0x28
    uint64 entityId;                                      // 0x30
    bool editorOnly;                                      // 0x38
    TArray<EVirtualPlatformID> excludedPlatforms;         // 0x40
    ZString entityName;                                   // 0x58
    TArray<SEntityTemplatePropertyAlias> propertyAliases; // 0x68
    TArray<SEntityTemplateExposedEntity> exposedEntities; // 0x80
    TArray<TPair<ZString, int32>> exposedInterfaces;      // 0x98
};

struct SEntityTemplatePinConnection {
    int32 fromID;                // 0x0
    int32 toID;                  // 0x4
    ZString fromPinName;         // 0x8
    ZString toPinName;           // 0x18
    ZObjectRef constantPinValue; // 0x28
};

struct SExternalEntityTemplatePinConnection {
    SEntityTemplateReference fromEntity; // 0x0
    SEntityTemplateReference toEntity;   // 0x28
    ZString fromPinName;                 // 0x50
    ZString toPinName;                   // 0x60
    ZObjectRef constantPinValue;         // 0x70
};

struct STemplateEntityBlueprint {
    int32 subType;                                                             // 0x0
    int32 rootEntityIndex;                                                     // 0x4
    ZString sourceResourceID;                                                  // 0x8
    TArray<STemplateBlueprintSubEntity> subEntities;                           // 0x18
    TArray<int32> externalSceneTypeIndicesInResourceHeader;                    // 0x30
    TArray<ZRuntimeResourceID> externalSceneRuntimeResourceIDs;                // 0x48
    TArray<SEntityTemplatePinConnection> pinConnections;                       // 0x60
    TArray<SEntityTemplatePinConnection> inputPinForwardings;                  // 0x78
    TArray<SEntityTemplatePinConnection> outputPinForwardings;                 // 0x90
    TArray<SEntityTemplateReference> overrideDeletes;                          // 0xA8
    TArray<SExternalEntityTemplatePinConnection> pinConnectionOverrides;       // 0xC0
    TArray<SExternalEntityTemplatePinConnection> pinConnectionOverrideDeletes; // 0xD8
};

class ZEntityBlueprintFactoryBase : public IEntityBlueprintFactory {
  public:
    ZRuntimeResourceID m_ridResource;                      // 0x8
    PAD(0x1C);                                             // 0x10
    int32_t m_rootEntityIndex;                             // 0x2C
    PAD(0x8);                                              // 0x30
    TArray<IEntityBlueprintFactory*> m_blueprintResources; // 0x38
    PAD(0x18);                                             // 0x50
};

static_assert(offsetof(ZEntityBlueprintFactoryBase, m_rootEntityIndex) == 0x2C);

class ZCompositeEntityBlueprintFactoryBase : public ZEntityBlueprintFactoryBase {
  public:
    TArray<int64_t> m_aSubEntityOffsets; // 0x68
};

static_assert(offsetof(ZCompositeEntityBlueprintFactoryBase, m_aSubEntityOffsets) == 0x68);

class ZTemplateEntityBlueprintFactory : public ZCompositeEntityBlueprintFactoryBase {
  public:
    PAD(0x68);                                            // 0x80
    TArray<uint64_t> m_aEntityIds;                        // 0xE8
    PAD(0x68);                                            // 0x100
    ZString m_SourceResourceID;                           // 0x168
    PAD(0x50);                                            // 0x178
    STemplateEntityBlueprint* m_pTemplateEntityBlueprint; // 0x1C8
};

class ZCppEntityBlueprintFactory : ZEntityBlueprintFactoryBase {};

class ZCppEntityFactory : public IEntityFactory {
  public:
    PAD(0x68);                                                    // 0x8
    TResourcePtr<ZCppEntityBlueprintFactory> m_blueprintResource; // 0x70
    ZRuntimeResourceID m_ridResource;                             // 0x78
};

class ZAspectEntityBlueprintFactory : public ZCompositeEntityBlueprintFactoryBase {
  public:
    struct SAspectedSubentityEntry {
        uint32 m_nAspectIdx;    // 0x0
        uint32 m_nSubentityIdx; // 0x4
    };

    PAD(0x38);                                                                   // 0x80
    TArray<SAspectedSubentityEntry> m_aSubEntitiesLookUp;                        // 0xB8
    THashMap<uint64, int32, TDefaultHashMapPolicy<uint64>> m_aSubEntityIndexMap; // 0xD0
};

class ZTemplateEntityFactory : public IEntityFactory {
  public:
    struct SDirectlySettableProperty {
        int32 entityIndex;    // 0x0
        int32 unk;            // 0x4
        int64 propertyOffset; // 0x8
        ZObjectRef value;     // 0x10
    };

    struct SDirectlySettablePropertyWithSetter {
        int32 entityIndex;          // 0x0
        int32 unk;                  // 0x4
        int64 propertyOffset;       // 0x8
        const SPropertyInfo* pInfo; // 0x10
        ZObjectRef value;           // 0x18
    };

    struct SResourceIDProperty {
        ZObjectRef value;  // 0x0
        int32 entityIndex; // 0x10
        uint32 propertyID; // 0x14
        int32 unk0;        // 0x18
        bool unk1;         // 0x1C
    };

    STemplateEntityFactory* m_pResourceData;                                            // 0x8
    bool m_bHasCalculatedPropertyValues;                                                // 0x10
    int32 m_rootEntityIndex;                                                            // 0x14
    TArray<IEntityFactory*> m_pFactories;                                               // 0x18
    PAD(0x18);                                                                          // 0x30
    ZRuntimeResourceID m_ridResource;                                                   // 0x48
    ZString m_SourceResourceID;                                                         // 0x50
    TResourcePtr<ZTemplateEntityBlueprintFactory> m_blueprintResource;                  // 0x60
    PAD(0xA8);                                                                          // 0x68
    TArray<SResourceIDProperty> m_resourceIDPropertyValues;                             // 0x110
    TArray<SDirectlySettableProperty> m_directlySettableProperties;                     // 0x128
    TArray<SDirectlySettablePropertyWithSetter> m_directlySettablePropertiesWithSetter; // 0x140
};

struct SEntityResource {
    ZEntityRef entityRef;         // 0x0
    ZResourcePtr factoryResource; // 0x10
};
