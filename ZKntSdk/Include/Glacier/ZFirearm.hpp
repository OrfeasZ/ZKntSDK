#pragma once

#include "ZItem.hpp"
#include "ZCurve.hpp"

class ZSpreadPatternEntity;
class ZRecoilPatternEntity;
class ZEmissionConfig;
class ZKntProjectileConfig;
class IProjectileFlightPath;
class ZFirearmDamageDefinition;

enum class EFirearmClass : int32_t {
    HandGun = 0,
    SubMachineGun = 1,
    AssaultRifle = 2,
    Taser = 3,
    TranquilizerDartGun = 4,
    Shotgun = 5,
    LongRifle = 6,
    HeavyHandGun = 7
};

enum class EFireMode : int32_t { SemiAutomatic = 0, Automatic = 1, Burst = 2, Charge = 3 };

enum class EFirearmChamberingType : int32_t { Autoloading = 0, BoltAction = 1 };

enum class EFirearmSpecialReactionTriggerType : int32_t { AnyShot = 0, FirstShot = 1, NoShot = 2 };

enum class EFirearmReloadType : int32_t { Magazine = 0, SingleBullet = 1 };

class ZFirearmDefinition : public ZItemCharacterDefinitionBase {
  public:
    float32 m_fireRate;                                                 // 0x1D0
    float32 m_changeDuration;                                           // 0x1D4
    float32 m_chargeDelayDuration;                                      // 0x1D8
    float32 m_chargeSidestepWindowDuration;                             // 0x1DC
    uint32 m_magazineSize;                                              // 0x1E0
    EFireMode m_fireMode;                                               // 0x1E4
    uint32 m_burstRounds;                                               // 0x1E8
    float32 m_burstDelay;                                               // 0x1EC
    EFirearmChamberingType m_firearmChamberingType;                     // 0x1F0
    EFirearmClass m_firearmClass;                                       // 0x1F4
    bool m_droppedByPlayerWhenOutOfAmmunition;                          // 0x1F8
    bool m_automaticPlayerReload;                                       // 0x1F9
    EFirearmReloadType m_reloadType;                                    // 0x1FC
    float32 m_reloadTime;                                               // 0x200
    float32 m_chamberingTime;                                           // 0x204
    EFirearmSpecialReactionTriggerType m_disarmTriggerType;             // 0x208
    EFirearmSpecialReactionTriggerType m_weakpointTriggerType;          // 0x20C
    TInterfaceRef<ZSpreadPatternEntity> m_spreadPattern;                // 0x210
    TInterfaceRef<ZRecoilPatternEntity> m_recoilPattern;                // 0x220
    ZCurve m_bulletMagnetismRangeMultiplierCurve;                       // 0x230
    float32 m_bulletMagnetismNoShootingTimeBeforeMultiplierReset;       // 0x278
    TResourcePtr<ZEntityRef> m_overrideDistanceToRadiusConfig;          // 0x27C
    TEntityRef<ZEmissionConfig> m_emitterConfig;                        // 0x288
    TEntityRef<ZKntProjectileConfig> m_projectileConfig;                // 0x2A0
    TEntityRef<IProjectileFlightPath> m_flightPathConfig;               // 0x2B8
    TResourcePtr<ZEntityRef> m_projectileCollisionPreset;               // 0x2D0
    bool m_suppressed;                                                  // 0x2D8
    uint32 m_maximumDroppedMagazineAmmunitionForUsedFirearm;            // 0x2DC
    uint32 m_minimumDroppedMagazineAmmunitionForUsedFirearm;            // 0x2E0
    uint32 m_maximumDroppedMagazineAmmunitionForUnusedFirearm;          // 0x2E4
    uint32 m_minimumDroppedMagazineAmmunitionForUnusedFirearm;          // 0x2E8
    uint32 m_maximumAmmunitionForAutoCollection;                        // 0x2EC
    uint32 m_minimumAmmunitionForAutoCollection;                        // 0x2F0
    float32 m_lootHighlightDistance;                                    // 0x2F4
    ZString m_lootUIIcon;                                               // 0x2F8
    ZString m_lootAvailableUIIcon;                                      // 0x308
    TInterfaceRef<ZFirearmDamageDefinition> m_damageToNpcDefinition;    // 0x318
    TInterfaceRef<ZFirearmDamageDefinition> m_damageToPlayerDefinition; // 0x328
    float32 m_friendlyFireMultiplier;                                   // 0x338
    ZRuntimeResourceID m_audioDefinitionResource;                       // 0x33C
    TResourcePtr<ZEntityRef> m_playerTrailEffectDefinitionResource;     // 0x344
    TResourcePtr<ZEntityRef> m_npcTrailEffectDefinitionResource;        // 0x34C
    TResourcePtr<ZEntityRef> m_impactMaterialDescriptorOverride;        // 0x354
    float32 m_quickThrowSpeed;                                          // 0x35C
    SVector3 m_quickThrowRotationSpeed;                                 // 0x360
    ZCurve m_npcPartialReloadProbabilityCurve;                          // 0x370
    bool m_isQPistol;                                                   // 0x3B8
    TResourcePtr<ZEntityRef> m_resourceDefinition;                      // 0x3BC
};
