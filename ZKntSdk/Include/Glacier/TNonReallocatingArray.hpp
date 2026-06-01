#pragma once

#include "ZObjectPool.hpp"

template<typename T, size_t N> class TNonReallocatingArray {
  public:
    T& operator[](size_t p_Index) {
        return m_Buffer.m_pData[p_Index];
    }

    const T& operator[](size_t p_Index) const {
        return m_Buffer.m_pData[p_Index];
    }

    T* begin() {
        return m_Buffer.m_pData;
    }

    T* end() {
        return m_Buffer.m_pData + m_nSize;
    }

    const T* begin() const {
        return m_Buffer.m_pData;
    }

    const T* end() const {
        return m_Buffer.m_pData + m_nSize;
    }

    size_t size() const {
        return m_nSize;
    }

    size_t capacity() const {
        return m_nCapacity;
    }

  private:
    uint32_t m_nSize;
    uint32_t m_nCapacity;
    ZInfiniteBuffer<T> m_Buffer;
};
