#pragma once

#include "ZBoundedEntity.hpp"

class ZRenderGraphNode;

class ZRenderableEntity : public ZBoundedEntity {
  public:
    ZRenderGraphNode* m_pRenderGraphData; // 0xC0
    PAD(0x20);                            // 0xC8
};
