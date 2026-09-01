#pragma once

#include "ZMath.hpp"
#include "ZEntity.hpp"

class IPxObject {
  public:
    virtual ~IPxObject() = 0;
};

class IPhysicsObject {
  public:
    virtual ~IPhysicsObject() = 0;
};

class IPhysicsObjectBase {
  public:
    virtual ~IPhysicsObjectBase() = 0;
};

class IPhysicsUserDataAccessor {
  public:
    virtual ~IPhysicsUserDataAccessor() = 0;
};

class ZPhysicsObjectBasePhysX : public IPxObject, public IPhysicsObject, public IPhysicsObjectBase {
  public:
    virtual ~ZPhysicsObjectBasePhysX() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk1() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk2() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk3() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk4() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk5() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk6() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk7() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk8() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk9() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk10() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk11() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk12() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk13() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk14() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk15() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk16() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk17() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk18() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk19() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk20() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk21() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk22() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk23() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk24() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk25() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk26() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk27() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk28() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk29() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk30() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk31() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk32() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk33() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk34() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk35() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk36() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk37() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk38() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk39() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk40() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk41() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk42() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk43() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk44() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk45() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk46() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk47() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk48() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk49() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk50() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk51() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk52() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk53() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk54() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk55() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk56() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk57() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk58() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk59() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk60() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk61() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk62() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk63() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk64() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk65() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk66() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk67() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk68() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk69() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk70() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk71() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk72() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk73() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk74() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk75() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk76() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk77() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk78() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk79() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk80() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk81() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk82() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk83() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk84() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk85() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk86() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk87() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk88() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk89() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk90() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk91() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk92() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk93() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk94() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk95() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk96() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk97() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk98() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk99() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk100() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk101() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk102() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk103() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk104() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk105() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk106() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk107() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk108() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk109() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk110() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk111() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk112() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk113() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk114() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk115() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk116() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk117() = 0;
    virtual void ZPhysicsObjectBasePhysX_unk118() = 0;
    virtual void SetTransform(const SMatrix& mNewTransform) = 0;
};

class ZPhysicsObject : public ZPhysicsObjectBasePhysX {};

class ZPhysicsBaseEntity : public ZEntityImpl {
  public:
    PAD(0x8); // 0x18
};

class ICollisionShapeListener : public IComponentInterface {};

class IPhysicsAccessor : public IComponentInterface {};

class IStaticPhysics : public IPhysicsAccessor {};

class IDebugPhysicsSpatialAccessor {
  public:
    virtual void IDebugPhysicsSpatialAccessor_unk0() = 0;
};

class ZStaticPhysicsAspect : public ZPhysicsBaseEntity, public ICollisionShapeListener, public IStaticPhysics, public IDebugPhysicsSpatialAccessor {
  public:
    bool m_bRemovePhysics;                     // 0x38
    bool m_bDisableNavmeshExport;              // 0x39
    bool m_bSmartStairSurface;                 // 0x3A
    PAD(0x8);                                  // 0x40
    ZPhysicsObjectBasePhysX* m_pPhysicsObject; // 0x48
    PAD(0x18);                                 // 0x50
};
