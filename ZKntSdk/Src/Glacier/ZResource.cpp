#include "Glacier/ZResource.hpp"

#include "Functions.hpp"

ZResourcePtr::~ZResourcePtr() {
    /*if (m_nResourceIndex.val < 0) {
        return;
    }

    auto& s_ResourceInfo = (*SDK()->Globals()->ResourceContainer)->m_resources[m_nResourceIndex.val];

    if (InterlockedDecrement(&s_ResourceInfo.refCount) == 0 && s_ResourceInfo.resourceData) {
        Functions::ZResourceManager_UninstallResource->Call(Globals::ResourceManager, m_nResourceIndex);
    }*/
}
