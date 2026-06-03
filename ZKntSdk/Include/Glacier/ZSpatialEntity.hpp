#pragma once

#include "ZEntity.hpp"
#include "ZMath.hpp"

#include <Functions.hpp>

class ZSpatialEntity : public ZEntityImpl {
  public:
    enum ERoomBehaviour { ROOM_STATIC = 0, ROOM_DYNAMIC = 1, ROOM_STATIC_OUTSIDE_CLIENT = 2 };

    virtual void ZSpatialEntity_unk17() = 0;
    virtual void ZSpatialEntity_unk18() = 0;
    virtual void ZSpatialEntity_unk19() = 0;
    virtual void ZSpatialEntity_unk20() = 0;
    virtual void ZSpatialEntity_unk21() = 0;
    virtual void ZSpatialEntity_unk22() = 0;
    virtual void ZSpatialEntity_unk23() = 0;
    virtual void ZSpatialEntity_unk24() = 0;
    virtual void ZSpatialEntity_unk25() = 0;
    virtual void ZSpatialEntity_unk26() = 0;
    virtual void ZSpatialEntity_unk27() = 0;
    virtual void ZSpatialEntity_unk28() = 0;
    virtual void ZSpatialEntity_unk29() = 0;
    virtual void ZSpatialEntity_unk30() = 0;
    virtual void ZSpatialEntity_unk31() = 0;
    virtual void SetObjectToWorldMatrixFromEditor(SMatrix mObjectToWorld) = 0;
    virtual void ZSpatialEntity_unk33() = 0;
    virtual void ZSpatialEntity_unk34() = 0;
    virtual void ZSpatialEntity_unk35() = 0;
    virtual void ZSpatialEntity_unk36() = 0;
    virtual void ZSpatialEntity_unk37() = 0;
    virtual void ZSpatialEntity_unk38() = 0;
    virtual void ZSpatialEntity_unk39() = 0;
    virtual void ZSpatialEntity_unk40() = 0;
    virtual void ZSpatialEntity_unk41() = 0;

    SMatrix GetObjectToWorldMatrix() const {
        if (m_bWorldTransformDirty) {
            SDK()->Functions()->ZSpatialEntity_UpdateCachedWorldMat->Call(this);
        }

        return m_mTransform;
    }

    PAD(0x28);
    SMatrix43 m_mTransform; // 0x40
    PAD(0x1C);              // 0x70
    uint32_t m_UnkFlag0 : 1;
    uint32_t m_UnkFlag1 : 1;
    uint32_t m_bWorldTransformDirty : 1; // 0x8C
};
