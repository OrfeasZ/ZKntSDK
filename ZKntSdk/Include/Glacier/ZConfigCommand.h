#pragma once

#include <cstdint>

class ZConfigCommand {
  public:
    enum class ECLASSTYPE { ECLASS_FLOAT, ECLASS_INT, ECLASS_STRING, ECLASS_UNKNOWN };

    virtual ECLASSTYPE GetType() = 0;

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
