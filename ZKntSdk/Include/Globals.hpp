#pragma once

#include "Common.hpp"
#include <cstdint>

class ZGameSceneflowModule;
class ZEntityManager;
class ZResourceContainer;
class ZMemoryManager;
class ZCameraManagerMain;
class ZApplicationEngineWin32;
class ZResourceManager;
class ZTypeRegistry;
class ZGameLoopManager;
class ZLocalPlayerData;
class ZGameTimeManager;
class ZCollisionManager;

namespace zknt {
    class Globals {
      public:
        Globals();
        ~Globals() = default;

        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;

        ZGameSceneflowModule* GameSceneflowModule = nullptr;
        ZEntityManager* EntityManager = nullptr;
        ZResourceContainer** ResourceContainer = nullptr;
        ZMemoryManager** MemoryManager = nullptr;
        ZCameraManagerMain* CameraManagerMain = nullptr;
        ZApplicationEngineWin32** ApplicationEngineWin32 = nullptr;
        ZResourceManager* ResourceManager = nullptr;
        ZTypeRegistry** TypeRegistry = nullptr;
        ZGameLoopManager* GameLoopManager = nullptr;
        ZLocalPlayerData* LocalPlayerData = nullptr;
        ZGameTimeManager* GameTimeManager = nullptr;
        ZCollisionManager* CollisionManager = nullptr;
    };
}
