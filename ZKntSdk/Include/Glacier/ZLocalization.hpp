#pragma once

#include "ZResource.hpp"

class ZTextListData {
  public:
    THashMap<int32_t, ZString, TDefaultHashMapPolicy<int32_t>> m_Map;
};

class ZTextLine {
  public:
    TResourcePtr<ZTextListData> m_pTextList; // 0x0
    int32 m_nNameHash;                       // 0x8
};
