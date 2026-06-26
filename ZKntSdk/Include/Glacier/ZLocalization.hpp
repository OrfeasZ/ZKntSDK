#pragma once

#include "ZResource.hpp"

class ZTextListData {
  public:
    THashMap<int32_t, ZString, TDefaultHashMapPolicy<int32_t>> m_Map;
};

class ZTextLine {
  public:
    ZString GetText() const {
        ZTextListData* s_TextListData = static_cast<ZTextListData*>(m_pTextList.GetResourceData());

        auto s_TextIt = s_TextListData->m_Map.find(m_nNameHash);

        if (s_TextIt == s_TextListData->m_Map.end()) {
            return "";
        }

        ZString s_Name;
        SDK()->Functions()->ZTextListData_DecryptText->Call(s_Name, s_TextIt->second);

        return s_Name;
    }

    TResourcePtr<ZTextListData> m_pTextList; // 0x0
    int32 m_nNameHash;                       // 0x8
};
