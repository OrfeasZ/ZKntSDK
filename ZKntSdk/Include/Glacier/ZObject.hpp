#pragma once

#include "Reflection.hpp"
#include "ZTypeRegistry.hpp"
#include "ZMemory.hpp"
#include <IModSDK.hpp>

class STypeID;
class ZString;

class ZObjectRef {
  public:
    static STypeID* GetVoidType() {
        static STypeID* s_VoidType = (*SDK()->Globals()->TypeRegistry)->GetTypeID("void");

        return s_VoidType;
    }

    ZObjectRef() {
        m_pTypeID = GetVoidType();
    }

    ZObjectRef(const ZObjectRef& p_Other) : m_pTypeID(p_Other.m_pTypeID) {
        if (p_Other.m_pTypeID != GetVoidType()) {
            m_pData = (*SDK()->Globals()->MemoryManager)
                          ->m_pNormalAllocator->AllocateAligned(m_pTypeID->GetTypeInfo()->m_nTypeSize, m_pTypeID->GetTypeInfo()->m_nTypeAlignment);

            m_pTypeID->GetTypeInfo()->m_pTypeFunctions->placementCopyConstruct(m_pData, p_Other.m_pData);
        }
    }

    ZObjectRef(ZObjectRef&& p_Other) noexcept : m_pTypeID(p_Other.m_pTypeID), m_pData(p_Other.m_pData) {
        p_Other.m_pTypeID = GetVoidType();
        p_Other.m_pData = nullptr;
    }

    ~ZObjectRef() {
        Clear();
    }

    template<class T> static ZObjectRef From(const T& p_Variant) {
        ZObjectRef s_Obj;
        s_Obj.Replace(p_Variant);
        return s_Obj;
    }

    ZObjectRef& operator=(const ZObjectRef& p_Other) {
        Clear();

        m_pTypeID = p_Other.m_pTypeID;
        m_pData = nullptr;

        if (p_Other.m_pTypeID != GetVoidType()) {
            m_pData = (*SDK()->Globals()->MemoryManager)
                          ->m_pNormalAllocator->AllocateAligned(m_pTypeID->GetTypeInfo()->m_nTypeSize, m_pTypeID->GetTypeInfo()->m_nTypeAlignment);

            m_pTypeID->GetTypeInfo()->m_pTypeFunctions->placementCopyConstruct(m_pData, p_Other.m_pData);
        }

        return *this;
    }

    ZObjectRef& operator=(ZObjectRef&& p_Other) noexcept {
        Clear();

        m_pTypeID = p_Other.m_pTypeID;
        m_pData = p_Other.m_pData;

        p_Other.m_pTypeID = GetVoidType();
        p_Other.m_pData = nullptr;

        return *this;
    }

    template<class T> [[nodiscard]] bool Is() const {
        if (m_pTypeID == nullptr || m_pTypeID->GetTypeInfo() == nullptr || m_pTypeID->GetTypeInfo()->pszTypeName == nullptr) {
            return false;
        }

        return ZHMTypeId<T> == Hash::Crc32(m_pTypeID->GetTypeInfo()->pszTypeName);
    }

    template<class T> [[nodiscard]] T* As() const {
        if (!Is<T>()) {
            return nullptr;
        }

        return static_cast<T*>(m_pData);
    }

    void Clear() {
        if (!m_pData) {
            m_pTypeID = GetVoidType();
            return;
        }

        if (m_pTypeID->m_nFlags != 2) {
            m_pTypeID->m_pType->m_pTypeFunctions->destruct(m_pData);
        }

        m_pTypeID = GetVoidType();

        // if (Globals::ZVariantPool1->m_pData && Globals::ZVariantPool1->BelongsToPool(m_pData)) {
        //     // TODO: Fix resource leak.
        //     // Globals::ZVariantPool1->Free(m_pData);
        // }
        // else if (Globals::ZVariantPool2->m_pData && Globals::ZVariantPool2->BelongsToPool(m_pData)) {
        //     // TODO: Fix resource leak.
        //     // Globals::ZVariantPool2->Free(m_pData);
        // }
        // else if (m_pTypeID != GetVoidType()) {
        //     (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->Free(m_pData);
        // }

        if (m_pTypeID != GetVoidType()) {
            (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->Free(m_pData);
        }

        m_pData = nullptr;
    }

    template<class T> void Replace(const T& p_Value) {
        Clear();

        m_pTypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID(ZHMTypeName<T>);
        m_pData = (*SDK()->Globals()->MemoryManager)
                      ->m_pNormalAllocator->AllocateAligned(m_pTypeID->GetTypeInfo()->m_nTypeSize, m_pTypeID->GetTypeInfo()->m_nTypeAlignment);

        m_pTypeID->GetTypeInfo()->m_pTypeFunctions->placementCopyConstruct(m_pData, &p_Value);
    }

    void Assign(STypeID* p_Type, void* p_Data) {
        Clear();

        m_pTypeID = p_Type;
        m_pData = (*SDK()->Globals()->MemoryManager)
                      ->m_pNormalAllocator->AllocateAligned(m_pTypeID->GetTypeInfo()->m_nTypeSize, m_pTypeID->GetTypeInfo()->m_nTypeAlignment);

        m_pTypeID->GetTypeInfo()->m_pTypeFunctions->placementCopyConstruct(m_pData, p_Data);
    }

    template<class T> ZObjectRef& operator=(const T& p_Value) {
        Replace(p_Value);
        return *this;
    }

    void UNSAFE_Assign(STypeID* p_Type, void* p_Data) {
        Clear();

        m_pTypeID = p_Type;
        m_pData = p_Data;
    }

    void UNSAFE_SetType(STypeID* p_Type) {
        m_pTypeID = p_Type;
    }

    bool IsEmpty() const {
        return m_pTypeID == nullptr || m_pData == nullptr || Hash::Crc32(m_pTypeID->GetTypeInfo()->pszTypeName) == ZHMTypeId<void>;
    }

    STypeID* GetTypeID() const {
        return m_pTypeID;
    }

    ZObjectRef Clone() const {
        ZObjectRef s_Obj;

        if (m_pTypeID == GetVoidType()) {
            return s_Obj;
        }

        s_Obj.m_pTypeID = m_pTypeID;
        s_Obj.m_pData = (*SDK()->Globals()->MemoryManager)
                            ->m_pNormalAllocator->AllocateAligned(m_pTypeID->GetTypeInfo()->m_nTypeSize, m_pTypeID->GetTypeInfo()->m_nTypeAlignment);

        m_pTypeID->GetTypeInfo()->m_pTypeFunctions->placementCopyConstruct(s_Obj.m_pData, m_pData);

        return s_Obj;
    }

  protected:
    STypeID* m_pTypeID = nullptr;
    void* m_pData = nullptr;
};

template<class T> class ZVariant : public ZObjectRef {
  public:
    ZVariant() {
        Replace(T());
    }

    ZVariant(const T& p_Value) {
        Replace(p_Value);
    }

    ZVariant(const ZVariant<T>& p_Other) {
        Replace(p_Other);
    }

    ZVariant(ZVariant<T>&& p_Other) noexcept {
        m_pTypeID = p_Other.m_pTypeID;
        m_pData = p_Other.m_pData;

        p_Other.m_pTypeID = GetVoidType();
        p_Other.m_pData = nullptr;
    }

    T& Get() {
        return *As<T>();
    }

    ZVariant<T>& operator=(const T& p_Value) {
        Replace(p_Value);
        return *this;
    }

    ZVariant<T>& operator=(ZVariant<T>&& p_Other) noexcept {
        Clear();

        m_pTypeID = p_Other.m_pTypeID;
        m_pData = p_Other.m_pData;

        p_Other.m_pTypeID = GetVoidType();
        p_Other.m_pData = nullptr;

        return *this;
    }
};

template<class T> class ZVariantRef : public ZObjectRef {
  public:
    ZVariantRef(T* p_Value) {
        m_pTypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID(ZHMTypeName<T>);
        m_pData = p_Value;
    }

    T* Get() {
        return static_cast<T*>(m_pData);
    }

    ZVariantRef<T>& operator=(T* p_Value) {
        Clear();

        m_pData = p_Value;
        return *this;
    }
};
