#pragma once

#include "IComponentInterface.hpp"
#include "ISavable.hpp"
#include "THashMap.hpp"
#include "THashSet.hpp"
#include "ZEntity.hpp"
#include "ZEvent.hpp"

struct SKeyword {
    uint32 m_level_0; // 0x0
    uint32 m_level_1; // 0x4
    uint32 m_level_2; // 0x8
    uint32 m_level_3; // 0xC
};

class ZGameKeywordManager : public IComponentInterface, public ISavable {
  public:
    ZString GetKeywordString(const SKeyword& p_Keyword) const {
        ZString s_KeywordString;

        auto s_Level0KeywordIt = m_Level0KeywordIDToStringMap.find(p_Keyword.m_level_0);
        auto s_Level1KeywordIt = m_Level1KeywordIDToStringMap.find(p_Keyword.m_level_1);
        auto s_Level2KeywordIt = m_Level2KeywordIDToStringMap.find(p_Keyword.m_level_2);
        auto s_Level3KeywordIt = m_Level3KeywordIDToStringMap.find(p_Keyword.m_level_3);

        if (s_Level0KeywordIt != m_Level0KeywordIDToStringMap.end()) {
            s_KeywordString += s_Level0KeywordIt->second;
        }

        if (s_Level1KeywordIt != m_Level1KeywordIDToStringMap.end()) {
            s_KeywordString += "+";
            s_KeywordString += s_Level1KeywordIt->second;
        }

        if (s_Level2KeywordIt != m_Level2KeywordIDToStringMap.end()) {
            s_KeywordString += "+";
            s_KeywordString += s_Level2KeywordIt->second;
        }

        if (s_Level3KeywordIt != m_Level3KeywordIDToStringMap.end()) {
            s_KeywordString += "+";
            s_KeywordString += s_Level3KeywordIt->second;
        }

        return s_KeywordString;
    }

    TArray<SKeyword> m_aKeywords;                                                                                                     // 0x10
    PAD(0x8);                                                                                                                         // 0x28
    THashMap<int32_t, ZString, TDefaultHashMapPolicy<int32_t>> m_Level0KeywordIDToStringMap;                                          // 0x30
    THashMap<int32_t, ZString, TDefaultHashMapPolicy<int32_t>> m_Level1KeywordIDToStringMap;                                          // 0x50
    THashMap<int32_t, ZString, TDefaultHashMapPolicy<int32_t>> m_Level2KeywordIDToStringMap;                                          // 0x70
    THashMap<int32_t, ZString, TDefaultHashMapPolicy<int32_t>> m_Level3KeywordIDToStringMap;                                          // 0x90
    THashMap<ZEntityRef, TArray<SKeyword>, TDefaultHashMapPolicy<int32_t>> m_KeywordIDToKeywordsMap;                                  // 0xB0
    THashMap<SKeyword, THashSet<ZEntityRef, TDefaultHashSetPolicy<ZEntityRef>>, TDefaultHashMapPolicy<int32_t>> m_KeywordToHolderMap; // 0xD0
    THashMap<SKeyword, ZEvent<const ZEntityRef&, bool>, TDefaultHashMapPolicy<int32_t>> m_KeywordDelegateMap;                         // 0xF0
    THashMap<ZEntityRef, ZEvent<int32_t, bool>, TDefaultHashMapPolicy<ZEntityRef>> m_EntityDelegateMap;                               // 0x110
};
