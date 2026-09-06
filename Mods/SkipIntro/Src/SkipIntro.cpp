#include "SkipIntro.hpp"

#include <Glacier/ZModule.hpp>
#include <Glacier/ZKntStartingCheckpoint.hpp>

void SkipIntro::Init() {
    SDK()->Hooks()->ZKntStartingCheckpoint_Start->AddDetour(this, &SkipIntro::ZKntStartingCheckpoint_Start);
}

DEFINE_PLUGIN_DETOUR(SkipIntro, void, ZKntStartingCheckpoint_Start, ZKntStartingCheckpoint* th) {
    ZEntitySceneContext* s_EntitySceneContext = SDK()->Globals()->GameSceneflowModule->m_pEntitySceneContext;

    for (const auto& brick : s_EntitySceneContext->m_SceneConfig->m_aMainBricks) {
        if (brick.m_RuntimeResourceID.GetID() != ResId<"[assembly:/_knt/scenes/globalbricks/global_streaming.brick].entitytype">) {
            continue;
        }

        const auto s_SubEntityCount = brick.m_BrickFactory->GetBlueprint()->GetSubEntitiesCount();

        for (uint64_t i = 0; i < s_SubEntityCount; ++i) {
            const ZEntityRef s_SubEntity = brick.m_BrickFactory->GetBlueprint()->GetSubEntity(brick.m_EntityType, i);

            if (!s_SubEntity.GetEntity() || !s_SubEntity->GetType()) {
                continue;
            }

            constexpr uint64_t s_MainMenuEntityID = 0xc3215683b280232e;

            if (s_SubEntity->GetType()->m_nEntityID != s_MainMenuEntityID) {
                continue;
            }

            ZEntityRef s_EntityRef;
            th->GetID(s_EntityRef);

            s_EntityRef.SetProperty<TEntityRef<ZKntCheckpointEntity>>("m_startupCheckpoint", TEntityRef<ZKntCheckpointEntity>(s_SubEntity));

            return {HookAction::Continue()};
        }
    }

    return {HookAction::Continue()};
}

DEFINE_ZKNT_PLUGIN(SkipIntro)
