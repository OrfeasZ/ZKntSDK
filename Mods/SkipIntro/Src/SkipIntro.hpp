#pragma once

#include <IPluginInterface.hpp>

class SkipIntro : public zknt::IPluginInterface {
  public:
    void Init() override;

  private:
    DECLARE_PLUGIN_DETOUR(SkipIntro, void, ZKntStartingCheckpoint_Start, ZKntStartingCheckpoint* th);
};

DECLARE_ZKNT_PLUGIN(SkipIntro)
