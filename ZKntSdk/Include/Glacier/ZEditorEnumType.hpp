#pragma once

#include "TArray.hpp"

struct ZEditorEnumType {
    struct SEnumType {
        ZString sName;
        TArray<ZString> aItemLabels;
        TArray<ZString> aItemNames;
        TArray<uint32_t> aItemValues;
    };

    SEnumType* m_pEnumType;
};
