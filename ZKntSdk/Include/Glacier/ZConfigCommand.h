#pragma once

#include <cstdint>

#include <Glacier/Hash.hpp>

#include <IModSDK.hpp>
#include <Util/StringUtils.hpp>

class ZConfigFloat;
class ZConfigInt;
class ZConfigString;

class ZConfigCommand {
  public:
    enum class ECLASSTYPE {
        ECLASS_FLOAT,
        ECLASS_INT,
        ECLASS_STRING,
        ECLASS_UNKNOWN,
    };

    virtual ECLASSTYPE GetType() = 0;

    template<typename T> T* As() {
        return GetType() == GetClassType<T>() ? static_cast<T*>(this) : nullptr;
    }

    static ZConfigCommand* Get(ZString p_CommandName) {
        return SDK()->Functions()->ZConfigCommand_GetConfigCommand->Call(Hash::Fnv1a(knt::util::ToLowerCase(p_CommandName.c_str()).c_str()));
    }

  private:
    template<typename T> static ECLASSTYPE GetClassType() {
        if (std::is_same<T, ZConfigFloat>::value) {
            return ECLASSTYPE::ECLASS_FLOAT;
        }
        else if (std::is_same<T, ZConfigInt>::value) {
            return ECLASSTYPE::ECLASS_INT;
        }
        else if (std::is_same<T, ZConfigString>::value) {
            return ECLASSTYPE::ECLASS_STRING;
        }

        return ECLASSTYPE::ECLASS_UNKNOWN;
    }

  public:
    void** m_pVftable;       // 0x0
    uint32_t m_iNameHash;    // 0x8
    bool m_Unk;              // 0xC
    ZConfigCommand* m_pNext; // 0x10
};

class ZConfigFloat : public ZConfigCommand {
  public:
    float m_Value;
};

class ZConfigInt : public ZConfigCommand {
  public:
    uint32_t m_Value;
};

class ZConfigString : public ZConfigCommand {
  public:
    char m_szValue[256];
};
