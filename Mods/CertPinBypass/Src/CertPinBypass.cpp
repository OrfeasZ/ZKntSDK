#include "CertPinBypass.hpp"

#include <Logging.hpp>

void CertPinBypass::Init() {
   SDK()->Hooks()->Check_SSL_Cert->AddDetour(this, &CertPinBypass::Check_SSL_Cert);
}

DEFINE_PLUGIN_DETOUR(CertPinBypass, bool, Check_SSL_Cert, void* a, void* b) {
    return {HookAction::Return(), true};
}

DEFINE_ZKNT_PLUGIN(CertPinBypass)
