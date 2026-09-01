#pragma once

#include "ZBoundedEntity.hpp"
#include "ZSparseBitArray.hpp"
#include "ZCamera.hpp"
#include "SColorRGB.hpp"

class ZRoomProxyEntity;
class ZBoxReflectionEntity;
class IRoomEntity;

class IGateEntity : public IComponentInterface {};

class ZGateEntity : public ZBoundedEntity, public IGateEntity {
  public:
    PAD(0x38);                                           // 0xC8
    bool m_bMagnetMode;                                  // 0x100
    bool m_bMagnetAllowOutsideMaster;                    // 0x101
    float32 m_nMagnetModeSearchDistance;                 // 0x104
    TEntityRef<ZRoomProxyEntity> m_rMagnetModeRoomProxy; // 0x108
    bool m_bMagnetPrimaryGate;                           // 0x120
    bool m_bIsOpen;                                      // 0x121
    bool m_bSound;                                       // 0x122
    bool m_bVisibility;                                  // 0x123
    bool m_bPerception;                                  // 0x124
    bool m_bReasoning;                                   // 0x125
    bool m_bAutoAssign;                                  // 0x126
    bool m_bDisableCulling;                              // 0x127
    bool m_bUseInFallbacks;                              // 0x128
    bool m_bAutoClientRooms;                             // 0x129
    float32 m_fOpenFraction;                             // 0x12C
    SColorRGB m_ClosedColor;                             // 0x130
    SVector3 m_vPortalSize;                              // 0x13C
    TEntityRef<IRoomEntity> m_RoomLeft;                  // 0x148
    SVector2 m_vLeftFadeAngles;                          // 0x160
    float32 m_fLeftFadeLength;                           // 0x168
    float32 m_fLeftFadeStart;                            // 0x16C
    bool m_bFlipLeftNormal;                              // 0x170
    TEntityRef<IRoomEntity> m_RoomRight;                 // 0x178
    SVector2 m_vRightFadeAngles;                         // 0x190
    float32 m_fRightFadeLength;                          // 0x198
    float32 m_fRightFadeStart;                           // 0x19C
    float32 m_fClipDistance;                             // 0x1A0
    TArray<ZEntityRef> m_Clients;                        // 0x1A8
    ZResourcePtr m_pHelper;                              // 0x1C0
    ZResourcePtr m_pHelperClosed;                        // 0x1C8
    ZEntityRef m_PierceOccluder;                         // 0x1D0
    PAD(0xA);                                            // 0x1E0
    uint16 m_nGateID;                                    // 0x1EA
};

class IRoomEntity : public IComponentInterface {};

class ZRoomEntity : public ZBoundedEntity, public IRoomEntity {
  public:
    TArray<ZGateEntity*> m_Gates;                                    // 0xC8
    PAD(0x18);                                                       // 0xE0
    TArray<ZEntityRef> m_Clients;                                    // 0xF8
    int32 m_PostfilterParametersID;                                  // 0x110
    float32 m_lightRigIntensityMultiplier;                           // 0x114
    ZString m_Tags;                                                  // 0x118
    SVector3 m_vRoomMin;                                             // 0x128
    SVector3 m_vRoomMax;                                             // 0x134
    bool m_bOutside;                                                 // 0x140
    bool m_bOutsideMaster;                                           // 0x141
    bool m_bCSMRoom;                                                 // 0x142
    bool m_bContainsCrowd;                                           // 0x143
    uint8 m_nRoomPriority;                                           // 0x144
    bool m_bGlobalLightOverrideEnable;                               // 0x145
    bool m_bGlobalLightPTOverrideEnable;                             // 0x146
    bool m_modifyLightRig;                                           // 0x147
    float32 m_fMiddleGrayMultiplier;                                 // 0x148
    SVector3 m_vGlobalLightOverrideDirection;                        // 0x14C
    SColorRGB m_GlobalLightOverrideFrontColor;                       // 0x158
    float32 m_fGlobalLightOverrideFrontIntensity;                    // 0x164
    SColorRGB m_GlobalLightOverrideBackColor;                        // 0x168
    float32 m_fGlobalLightOverrideBackIntensity;                     // 0x174
    SColorRGB m_GlobalLightPTOverrideFrontColor;                     // 0x178
    float32 m_fGlobalLightPTOverrideFrontIntensity;                  // 0x184
    SColorRGB m_GlobalLightPTOverrideBackColor;                      // 0x188
    float32 m_fGlobalLightPTOverrideBackIntensity;                   // 0x194
    bool m_bLuminanceOverrideEnable;                                 // 0x198
    bool m_bForceUseOnlyBoxReflectionFallback;                       // 0x199
    uint16 m_nRoomID;                                                // 0x19A
    SVector2 m_vLuminanceMinMax;                                     // 0x19C
    SColorRGB m_lightRigTint;                                        // 0x1A4
    TEntityRef<ZBoxReflectionEntity> m_BoxReflectionGIProbeFallback; // 0x1B0
    TEntityRef<ZBoxReflectionEntity> m_BoxReflectionSSGIFallback;    // 0x1C8
};

class IRoomManager : public IComponentInterface {};

class ZRoomManagerMain : public IRoomManager, public ZSceneLifecycleListener, public IReflectSource {
  public:
    TArray<ZGateEntity*> m_GateEntities; // 0x18
    PAD(0x28);                           // 0x30
    TArray<ZRoomEntity*> m_RoomEntities; // 0x58
    PAD(0x1F8);                          // 0x70
    ZSparseBitArray m_RoomsVisible;      // 0x268
};

struct SGateRenderData {
    PAD(0x100);         // 0x0
    uint16 m_nGateID;   // 0x100
    PAD(0x8E);          // 0x102
    uint16_t m_nRoomID; // 0x190
    PAD(0xE);           // 0x192
};

class IRoomManagerRender {
  public:
    virtual ~IRoomManagerRender() = 0;
};

class ZRoomManagerRender : public IRoomManagerRender {
  public:
    PAD(0x130);                               // 0x8
    ZSparseBitArray m_RoomsVisible;           // 0x138
    ZSparseBitArray m_GatesVisible;           // 0x158
    PAD(0x268);                               // 0x178
    TArray<SGateRenderData> m_GateRenderData; // 0x3E0
};
