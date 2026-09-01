#pragma once

#include <cstdint>

enum class EGameplayTag : uint16_t {
    Undefined = 0,

    NPC_Enemy = 1,
    Bond = 2,
    Sports_Car = 3,
    SUV = 4,
    Truck = 5,
    Boat = 6,
    Aston_Martin_DBS = 7,
    Land_Rover_Defender = 8,
    Aston_Martin_Valhalla = 9,

    NPC_Companion = 10,
    NPC_Authority = 11,

    Archetype_Grunt = 12,
    Archetype_Merc = 13,
    Archetype_Merc_Armored = 14,
    Archetype_Specialist = 15,
    Archetype_Leader = 16,
    Archetype_Sniper = 17,
    Archetype_Tank = 18,
    Archetype_Brute = 19,
    Archetype_Android = 20,

    NPC_Civilian = 21,

    AutoAimTarget = 22,
    SUV_Mood = 23,
    Freefall_Belt_Loader = 24,
    SUV_Mood_Passenger = 25,
    Distraction = 26,
    Sedan_Mood = 27,

    Gadget_Blast = 28,
    Gadget_Hack = 29,
    Gadget_Dart = 30,
    Gadget_Smoke = 31,
    Gadget_Shockwave = 32,
    Gadget_MissilePen = 33,
    Gadget_Laser = 34,

    Distraction_Tier1 = 35,
    Distraction_Tier2 = 36,
    Distraction_Tier3 = 37,
    Distraction_Tier4 = 38,
    Distraction_Tier5 = 39,

    FirearmActivated = 40,
    ExplosionActivated = 41,
    StumbleActivated = 42,

    Gadget_Magnolia_Overwatch = 43,
    NPC_Ally = 44,
    Gadget_Blast_TargetBlasted = 45,

    Mining_Truck = 46,

    Camera_Orbital = 47,
    Camera_Cockpit = 48,

    BoatClose = 49,
    SportsCar_TightTurns = 50,
    Motorbike = 51,

    Setpiece_Explosive = 52,
    Setpiece_FallingHazard = 53,

    Distraction_Visual = 54,
    Distraction_Fixable = 55,

    Boat_Magnolia = 56,

    ExcludeCollisionForVFXAndSFX = 57,

    Setpiece_LVA_GoodSmoke = 58,
    Setpiece_LVA_BadSmoke = 59,
    Setpiece_Blinding = 60,

    SportsCar_TightTurns_Lotus = 61
};
