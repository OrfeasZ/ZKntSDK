#pragma once

#include <IPluginInterface.hpp>

class CertPinBypass : public zknt::IPluginInterface {
public:
    void Init() override;

private:
    DECLARE_PLUGIN_DETOUR(CertPinBypass, bool, Check_SSL_Cert, void*, void*);
};

DECLARE_ZKNT_PLUGIN(CertPinBypass)
