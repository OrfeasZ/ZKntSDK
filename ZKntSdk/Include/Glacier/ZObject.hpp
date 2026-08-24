#pragma once

#include "Reflection.hpp"
#include "ZTypeRegistry.hpp"
#include "ZMemory.hpp"
#include "ZObjectPool.hpp"

#include <IModSDK.hpp>

class STypeID;
class ZString;

class ZObjectRef {
  public:
    static STypeID* GetVoidType() {
        if (!SDK()->Globals()->TypeRegistry || !*SDK()->Globals()->TypeRegistry) {
            return nullptr;
        }

        static STypeID* s_VoidType = (*SDK()->Globals()->TypeRegistry)->GetTypeID("void");

        return s_VoidType;
    }

    ZObjectRef() {
        m_pTypeID = GetVoidType();
    }

    ZObjectRef(const ZObjectRef& p_Other) : m_pTypeID(p_Other.m_pTypeID) {
        if (p_Other.m_pTypeID != GetVoidType()) {
            AllocateMemory();

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
        if (this == &p_Other) {
            return *this;
        }

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
        DestroyMemory();

        m_pTypeID = GetVoidType();
    }

    template<class T> void Replace(const T& p_Value) {
        DestroyMemory();

        m_pTypeID = (*SDK()->Globals()->TypeRegistry)->GetTypeID(ZHMTypeName<T>);

        AllocateMemory();

        m_pTypeID->GetTypeInfo()->m_pTypeFunctions->placementCopyConstruct(m_pData, &p_Value);
    }

    void Assign(STypeID* p_Type, void* p_Data) {
        DestroyMemory();

        m_pTypeID = p_Type;

        AllocateMemory();

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

    void* GetData() const {
        return m_pData;
    }

    void* AllocateMemory() {
        uint32_t s_Size;

        if (m_pTypeID->m_nFlags == 2) {
            s_Size = 8;
        }
        else {
            s_Size = m_pTypeID->GetTypeInfo()->m_nTypeSize;
        }

        if (s_Size <= 8) {
            if (SDK()->Globals()->Variant8BytePool && SDK()->Globals()->Variant8BytePool->m_pData) {
                m_pData = SDK()->Globals()->Variant8BytePool->Alloc();
            }
        }
        else if (s_Size <= 0x20) {
            if (SDK()->Globals()->Variant32BytePool && SDK()->Globals()->Variant32BytePool->m_pData) {
                m_pData = SDK()->Globals()->Variant32BytePool->Alloc();
            }
        }

        if (!m_pData) {
            const uint32_t s_Alignment = m_pTypeID->m_nFlags == 2 ? 8 : m_pTypeID->GetTypeInfo()->m_nTypeAlignment;

            m_pData = (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->AllocateAligned(s_Size, s_Alignment);
        }

        return m_pData;
    }

    void DestroyMemory() {
        if (!m_pData) {
            return;
        }

        // char[INT32_MAX] has type number 21.
        if (m_pTypeID->m_nTypeNum != 21) {
            if (m_pTypeID->m_nFlags != 2) {
                m_pTypeID->GetTypeInfo()->m_pTypeFunctions->destruct(m_pData);
            }

            if (SDK()->Globals()->Variant8BytePool && SDK()->Globals()->Variant8BytePool->Contains(m_pData)) {
                SDK()->Globals()->Variant8BytePool->Free(m_pData);
            }
            else if (SDK()->Globals()->Variant32BytePool && SDK()->Globals()->Variant32BytePool->Contains(m_pData)) {
                SDK()->Globals()->Variant32BytePool->Free(m_pData);
            }
            else {
                (*SDK()->Globals()->MemoryManager)->m_pNormalAllocator->Free(m_pData);
            }
        }

        m_pData = nullptr;
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
