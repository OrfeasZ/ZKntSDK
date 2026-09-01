#include "Editor.hpp"

#include <queue>

#include "IconsMaterialDesign.h"

#include <imgui_stdlib.h>

#include <Glacier/ZModule.hpp>
#include <Glacier/ZPlayer.hpp>

#include <Util/ImGuiUtils.hpp>
#include <Util/StringUtils.hpp>
#include <Logging.hpp>

void Editor::DrawEntityTreeWindow() {
    ImGui::SetNextWindowPos({0, 110}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({615, ImGui::GetIO().DisplaySize.y - 110}, ImGuiCond_FirstUseEver);
    ImGui::Begin(ICON_MD_CATEGORY " Entities", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    const auto s_EntitySceneContext = SDK()->Globals()->GameSceneflowModule->m_pEntitySceneContext;

    if (s_EntitySceneContext && s_EntitySceneContext->m_pScene && s_EntitySceneContext->m_SceneConfig->m_aMainBricks.size() > 0) {
        const ImGuiStyle& s_Style = ImGui::GetStyle();

        const float s_PreviousButtonWidth = ImGui::CalcTextSize(ICON_MD_ARROW_BACK " Previous").x + s_Style.FramePadding.x * 2;

        const std::string s_ResultCount =
            fmt::format("{} / {}", m_TotalMatchCount == 0 ? 0 : m_CurrentEntitySearchResultIndex + 1, m_TotalMatchCount);
        const float s_ResultCountWidth = ImGui::CalcTextSize(s_ResultCount.c_str()).x;

        const float s_NextButtonWidth = ImGui::CalcTextSize("Next " ICON_MD_ARROW_FORWARD).x + s_Style.FramePadding.x * 2;

        const float s_ClearFiltersButtonWidth = ImGui::CalcTextSize(ICON_MD_CLEAR " Clear filters").x + s_Style.FramePadding.x * 2;

        const float s_FilterControlsWidth =
            s_PreviousButtonWidth + s_ResultCountWidth + s_NextButtonWidth + s_ClearFiltersButtonWidth + s_Style.ItemSpacing.x * 3;

        ImGui::SetNextItemWidth(s_FilterControlsWidth);

        if (ImGui::InputText(
                ICON_MD_SEARCH " Search by ID", &m_EntityIDSearchInput,
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsNoBlank
            )) {
            FilterEntityTree();
        }

        ImGui::SetNextItemWidth(s_FilterControlsWidth);

        if (ImGui::InputText(ICON_MD_SEARCH " Search by name", &m_EntityNameSearchInput, ImGuiInputTextFlags_EnterReturnsTrue)) {
            FilterEntityTree();
        }

        ImGui::SetNextItemWidth(s_FilterControlsWidth);

        knt::util::InputWithAutocomplete(
            ICON_MD_SEARCH " Search by type##EntityTypesPopup", m_EntityTypeSearchInput, m_ClassNames,
            [](const std::string& p_ClassName) -> const std::string& { return p_ClassName; },
            [](const std::string& p_ClassName) -> const std::string& { return p_ClassName; },
            [&](const std::string&, const std::string&, const auto&) { FilterEntityTree(); }
        );

        ImGui::SetNextItemWidth(s_FilterControlsWidth);

        if (ImGui::BeginCombo("Entity view mode", m_EntityViewModes[m_EntityViewMode].c_str())) {
            for (int i = 0; i < m_EntityViewModes.size(); ++i) {
                const bool s_IsSelected = m_EntityViewMode == static_cast<EntityViewMode>(i);

                if (ImGui::Selectable(m_EntityViewModes[i].c_str(), s_IsSelected)) {
                    m_EntityViewMode = static_cast<EntityViewMode>(i);

                    FilterEntityTree();
                }

                if (s_IsSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        const bool s_HasSearchResults = m_TotalMatchCount > 0;

        // Clamp index in case matches shrank while searching
        if (s_HasSearchResults && m_CurrentEntitySearchResultIndex >= m_TotalMatchCount) {
            m_CurrentEntitySearchResultIndex = 0;
        }

        ImGui::BeginDisabled(!s_HasSearchResults);

        if (ImGui::Button(ICON_MD_ARROW_BACK " Previous")) {
            if (s_HasSearchResults) {
                if (m_CurrentEntitySearchResultIndex == 0) {
                    m_CurrentEntitySearchResultIndex = m_TotalMatchCount - 1;
                }
                else {
                    --m_CurrentEntitySearchResultIndex;
                }

                size_t s_Counter = 0;
                if (auto s_Node = FindMatchByIndex(m_CachedEntityTree, m_CurrentEntitySearchResultIndex, s_Counter)) {
                    OnSelectEntity(s_Node->m_Entity, true, std::nullopt);
                }
            }
        }

        ImGui::SameLine();

        ImGui::Text("%zu / %zu", s_HasSearchResults ? m_CurrentEntitySearchResultIndex + 1 : 0, m_TotalMatchCount);

        ImGui::SameLine();

        if (ImGui::Button("Next " ICON_MD_ARROW_FORWARD)) {
            if (s_HasSearchResults) {
                m_CurrentEntitySearchResultIndex = (m_CurrentEntitySearchResultIndex + 1) % m_TotalMatchCount;

                size_t s_Counter = 0;
                if (auto s_Node = FindMatchByIndex(m_CachedEntityTree, m_CurrentEntitySearchResultIndex, s_Counter)) {
                    OnSelectEntity(s_Node->m_Entity, true, std::nullopt);
                }
            }
        }

        ImGui::EndDisabled();

        ImGui::SameLine();

        if (ImGui::Button(ICON_MD_CLEAR " Clear filters")) {
            ClearFilters();
        }

        ImGui::Spacing();

        bool s_HasEntityTree;

        {
            std::shared_lock s_Lock(m_CachedEntityTreeMutex);
            s_HasEntityTree = m_CachedEntityTree != nullptr;
        }

        if (ImGui::Button(s_HasEntityTree ? ICON_MD_CONSTRUCTION " Rebuild entity tree" : ICON_MD_CONSTRUCTION " Build entity tree")) {
            UpdateEntities();
        }

        ImGui::SameLine();

        {
            std::shared_lock s_Lock(m_CachedEntityTreeMutex);

            ImGui::BeginDisabled(!m_CachedEntityTree);

            if (ImGui::Button(ICON_MD_UNFOLD_LESS " Collapse tree")) {
                m_OpenEntityTreeNodes.clear();
            }

            ImGui::SameLine();

            if (ImGui::Button(ICON_MD_SAVE_ALT " Export entity tree")) {
                std::ofstream s_FileOut("entity_tree.txt");

                if (s_FileOut.is_open()) {
                    auto s_OutputNode = [&](auto& self, const std::shared_ptr<EntityTreeNode>& p_Node, uint32_t p_Depth) -> void {
                        if (!p_Node || p_Node->m_IsPendingDeletion.load(std::memory_order_acquire)) {
                            return;
                        }

                        if (m_HasActiveFilters && !m_FilteredEntityTreeNodes.contains(p_Node.get())) {
                            return;
                        }

                        const bool s_HasVisibleChildren = HasVisibleChildren(p_Node);

                        const std::string s_Prefix = p_Depth != 0 ? std::string(p_Depth * 2, ' ') + (s_HasVisibleChildren ? "> " : "- ") : "";

                        s_FileOut << s_Prefix << p_Node->m_Name << std::endl;

                        if (!s_HasVisibleChildren) {
                            return;
                        }

                        for (const auto& [_, s_Child] : p_Node->m_Children) {
                            self(self, s_Child, p_Depth + 1);
                        }
                    };

                    if (m_CachedEntityTree) {
                        s_OutputNode(s_OutputNode, m_CachedEntityTree, 0);
                    }
                }
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Outputs the current entity tree as a text file (entity_tree.txt - found in the Retail folder of the game).");
            }

            ImGui::EndDisabled();

            if (ImGui::BeginChild("EntityTree", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
                const bool s_HasNoResults = m_HasActiveSearch && m_TotalMatchCount == 0;

                if (s_HasNoResults) {
                    ImGui::TextColored(ImVec4(1.0f, 0.27f, 0.27f, 1.0f), "No results found.");
                }
                else if (m_CachedEntityTree) {
                    DrawEntity(m_CachedEntityTree);
                }
                else {
                    ImGui::Text("No entities loaded. Build the entity tree to load them.");
                }
            }

            ImGui::EndChild();
        }
    }
    else {
        ImGui::Text("No scene loaded.");
    }

    ImGui::End();

    if (m_ScrollToEntity) {
        m_ScrollToEntity = false;
    }
}

void Editor::DrawEntity(std::shared_ptr<EntityTreeNode> p_Node) {
    if (!p_Node) {
        return;
    }

    const bool s_IsPendingDeletion = p_Node->m_IsPendingDeletion.load(std::memory_order_acquire);

    if (m_HasActiveFilters && !m_FilteredEntityTreeNodes.contains(p_Node.get())) {
        return;
    }

    ImGui::PushID(p_Node.get());

    const auto s_Entity = p_Node->m_Entity;
    const auto s_EntityType = p_Node->m_EntityType;
    const auto s_EntityName = p_Node->m_Name;
    const auto s_IsSelected = s_Entity == m_SelectedEntity;

    ImGuiTreeNodeFlags s_Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth
                                 | ImGuiTreeNodeFlags_DrawLinesToNodes;

    const bool s_HasVisibleChildren = HasVisibleChildren(p_Node);

    if (!s_HasVisibleChildren) {
        s_Flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    if (s_IsSelected) {
        s_Flags |= ImGuiTreeNodeFlags_Selected;

        if (m_ScrollToEntity) {
            ImGui::SetScrollHereY();
            m_ScrollToEntity = false;
        }
    }
    else if (m_ScrollToEntity && m_SelectedEntity) {
        bool s_ShouldExpandNode = false;

        if (IsSpecialEntityTreeNode(s_Entity)) {
            for (const auto& [_, s_Child] : p_Node->m_Children) {
                if (s_Child->m_Entity == m_SelectedEntity || (s_Child->m_Entity && m_SelectedEntity.IsAnyParent(s_Child->m_Entity))) {
                    s_ShouldExpandNode = true;
                    break;
                }
            }
        }
        else if (s_Entity && m_SelectedEntity.IsAnyParent(s_Entity)) {
            s_ShouldExpandNode = true;
        }

        if (s_ShouldExpandNode) {
            m_OpenEntityTreeNodes.insert(p_Node.get());
        }
    }

    if (s_IsPendingDeletion) {
        m_OpenEntityTreeNodes.erase(p_Node.get());

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::BeginDisabled();
    }

    const bool s_IsDirectMatch = m_HasActiveFilters && p_Node->m_IsDirectMatch && !s_IsPendingDeletion;

    if (s_IsDirectMatch) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(0xFF8AF0FE)); // Tailwind yellow-200
    }

    ImGui::SetNextItemOpen(m_OpenEntityTreeNodes.contains(p_Node.get()), ImGuiCond_Always);

    auto s_Open = ImGui::TreeNodeEx(s_EntityName.c_str(), s_Flags);

    if (ImGui::IsItemToggledOpen()) {
        if (s_Open) {
            m_OpenEntityTreeNodes.insert(p_Node.get());
        }
        else {
            m_OpenEntityTreeNodes.erase(p_Node.get());
        }
    }

    if (s_IsDirectMatch) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", s_EntityType.c_str());
    }

    if (!s_IsPendingDeletion) {
        if (ImGui::IsItemFocused() && !s_IsSelected) {
            if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_Space)) {
                OnSelectEntity(s_Entity, false, std::nullopt);
            }
        }

        if (ImGui::IsItemClicked()) {
            OnSelectEntity(s_Entity, false, std::nullopt);
        }
    }

    if (s_Open) {
        if (s_HasVisibleChildren) {
            for (const auto& s_Child : p_Node->m_Children) {
                DrawEntity(s_Child.second);
            }
        }

        if (!(s_Flags & ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
            ImGui::TreePop();
        }
    }

    if (s_IsPendingDeletion) {
        ImGui::EndDisabled();
        ImGui::PopStyleColor();
    }

    ImGui::PopID();
}

bool Editor::HasVisibleChildren(const std::shared_ptr<EntityTreeNode>& p_Node) const {
    if (!p_Node || p_Node->m_Children.empty()) {
        return false;
    }

    if (!m_HasActiveFilters) {
        return true;
    }

    return std::any_of(p_Node->m_Children.begin(), p_Node->m_Children.end(), [this](const auto& s_Child) {
        return m_FilteredEntityTreeNodes.contains(s_Child.second.get());
    });
}

void Editor::FilterEntityTree() {
    m_FilteredEntityTreeNodes.clear();
    m_TotalMatchCount = 0;

    if (!m_CachedEntityTree) {
        m_HasActiveFilters = false;
        m_CurrentEntitySearchResultIndex = 0;
        return;
    }

    m_HasActiveSearch = !m_EntityIDSearchInput.empty() || !m_EntityTypeSearchInput.empty() || !m_EntityNameSearchInput.empty();

    m_HasActiveFilters = m_HasActiveSearch || m_EntityViewMode != EntityViewMode::All;

    if (!m_HasActiveFilters) {
        m_CurrentEntitySearchResultIndex = 0;
        m_LastEntityViewMode = m_EntityViewMode;
        return;
    }

    EntityTreeNode* s_SingleMatchNode = nullptr;

    FilterEntityTree(m_CachedEntityTree.get(), s_SingleMatchNode);

    if (m_FilteredEntityTreeNodes.empty()) {
        m_FilteredEntityTreeNodes.insert(m_CachedEntityTree.get());
    }

    m_CurrentEntitySearchResultIndex = 0;

    if (m_TotalMatchCount == 1 && s_SingleMatchNode) {
        OnSelectEntity(s_SingleMatchNode->m_Entity, true, std::nullopt);
    }

    m_LastEntityViewMode = m_EntityViewMode;
}

bool Editor::FilterEntityTree(EntityTreeNode* p_Node, EntityTreeNode*& p_OutSingleMatchNode) {
    if (!p_Node || p_Node->m_IsPendingDeletion.load(std::memory_order_acquire)) {
        return false;
    }

    if (m_EntityViewMode == EntityViewMode::ScenesAndBricks && (p_Node->m_IsDynamicEntity || p_Node->m_Entity == m_DynamicEntitiesNodeEntityRef)) {
        p_Node->m_IsDirectMatch = false;
        return false;
    }

    const bool s_PassesViewMode = m_EntityViewMode == EntityViewMode::All || m_EntityViewMode == EntityViewMode::ScenesAndBricks
                                  || (m_EntityViewMode == EntityViewMode::DynamicEntities && p_Node->m_IsDynamicEntity);

    bool s_Matches = false;

    if (s_PassesViewMode && m_HasActiveSearch) {
        bool s_MatchesID = true;
        bool s_MatchesType = true;
        bool s_MatchesName = true;

        if (!IsSpecialEntityTreeNode(p_Node->m_Entity)) {
            if (!m_EntityIDSearchInput.empty()) {
                const uint64_t s_EntityID = std::strtoull(m_EntityIDSearchInput.c_str(), nullptr, 16);
                s_MatchesID = p_Node->m_EntityID == s_EntityID;
            }

            if (!m_EntityTypeSearchInput.empty()) {
                s_MatchesType = p_Node->m_Entity.HasInterface(m_EntityTypeSearchInput);
            }
        }

        if (!m_EntityNameSearchInput.empty()) {
            s_MatchesName = knt::util::FindSubstring(p_Node->m_Name.c_str(), m_EntityNameSearchInput);
        }

        s_Matches = s_MatchesID && s_MatchesType && s_MatchesName;

        // Special root containers match only if explicitly queried by name
        if (s_Matches && IsSpecialEntityTreeNode(p_Node->m_Entity)) {
            if (m_EntityNameSearchInput.empty() || !knt::util::FindSubstring(p_Node->m_Name.c_str(), m_EntityNameSearchInput)) {
                s_Matches = false;
            }
        }
    }

    p_Node->m_IsDirectMatch = s_Matches;

    if (s_Matches) {
        m_TotalMatchCount++;
        p_OutSingleMatchNode = p_Node;
    }

    bool s_ChildMatches = false;

    for (auto& [_, child] : p_Node->m_Children) {
        if (FilterEntityTree(child.get(), p_OutSingleMatchNode)) {
            s_ChildMatches = true;
        }
    }

    if ((!m_HasActiveSearch && s_PassesViewMode) || s_Matches || s_ChildMatches) {
        m_FilteredEntityTreeNodes.insert(p_Node);
        return true;
    }

    return false;
}

void Editor::ClearFilters() {
    std::scoped_lock s_Lock(m_CachedEntityTreeMutex);

    m_EntityIDSearchInput.clear();
    m_EntityTypeSearchInput.clear();
    m_EntityNameSearchInput.clear();

    m_EntityViewMode = EntityViewMode::All;
    m_LastEntityViewMode = EntityViewMode::All;

    m_FilteredEntityTreeNodes.clear();
    m_TotalMatchCount = 0;
    m_CurrentEntitySearchResultIndex = 0;
    m_HasActiveSearch = false;
    m_HasActiveFilters = false;
}

std::shared_ptr<EntityTreeNode>
Editor::FindMatchByIndex(const std::shared_ptr<EntityTreeNode>& p_Node, size_t p_TargetIndex, size_t& p_CurrentCounter) {
    if (!p_Node || p_Node->m_IsPendingDeletion.load(std::memory_order_acquire)) {
        return nullptr;
    }

    if (p_Node->m_IsDirectMatch) {
        if (p_CurrentCounter == p_TargetIndex) {
            return p_Node;
        }

        p_CurrentCounter++;
    }

    for (const auto& [_, s_Child] : p_Node->m_Children) {
        if (auto s_Match = FindMatchByIndex(s_Child, p_TargetIndex, p_CurrentCounter)) {
            return s_Match;
        }
    }

    return nullptr;
}

void Editor::UpdateEntities() {
    const auto s_EntitySceneContext = SDK()->Globals()->GameSceneflowModule->m_pEntitySceneContext;

    if (!s_EntitySceneContext || !s_EntitySceneContext->m_pScene || !s_EntitySceneContext->m_pScene.m_entityRef
        || s_EntitySceneContext->m_SceneConfig->m_aMainBricks.size() == 0) {
        return;
    }

    const auto s_SceneEntity = s_EntitySceneContext->m_pScene.m_entityRef;

    std::vector<std::pair<ZEntityRef, IEntityBlueprintFactory*>> s_EntitiesToProcess;

    // Add all the brick nodes to the queue.
    for (const auto& s_Brick : s_EntitySceneContext->m_SceneConfig->m_aMainBricks) {
        auto s_BrickEntity = s_Brick.m_EntityType;

        if (!s_BrickEntity) {
            continue;
        }

        s_EntitiesToProcess.push_back({s_BrickEntity, s_Brick.m_BrickFactory->GetBlueprint()});
    }

    auto s_SceneBlueprintFactory =
        reinterpret_cast<ZTemplateEntityBlueprintFactory*>(s_EntitySceneContext->m_SceneConfig->m_sceneFactoryResource.GetResource()->GetBlueprint());

    // Create the root scene node.
    auto s_SceneNode = std::make_shared<EntityTreeNode>(
        "Scene Root", (*s_SceneEntity->GetType()->m_pInterfaceData)[0].m_Type->GetTypeInfo()->pszTypeName, s_SceneEntity->GetType()->m_nEntityID,
        s_SceneBlueprintFactory->m_ridResource, "TBLU", ResId<"[modules:/zsceneentity.class].pc_entityblueprint">, "CBLU", "", "", s_SceneEntity, true
    );

    auto s_DynamicBricksNode =
        std::make_shared<EntityTreeNode>("Dynamic bricks", "", -1, -1, "", -1, "", "", "", m_DynamicBricksNodeEntityRef, false);

    s_SceneNode->m_Children.insert(std::make_pair(s_DynamicBricksNode->m_Name, s_DynamicBricksNode));
    s_DynamicBricksNode->m_Parent = s_SceneNode;

    auto s_UnparentedEntitiesNode =
        std::make_shared<EntityTreeNode>("Unparented entities", "", -1, -1, "", -1, "", "", "", m_UnparentedEntitiesNodeEntityRef, false);

    s_SceneNode->m_Children.insert(std::make_pair(s_UnparentedEntitiesNode->m_Name, s_UnparentedEntitiesNode));
    s_UnparentedEntitiesNode->m_Parent = s_SceneNode;

    std::unordered_map<ZEntityRef, std::shared_ptr<EntityTreeNode>> s_NodeMap;
    s_NodeMap.emplace(s_SceneEntity, s_SceneNode);

    UpdateEntityTree(s_NodeMap, s_EntitiesToProcess, false, false);

    s_EntitiesToProcess.clear();

    for (const auto& s_Brick : s_EntitySceneContext->m_aDynamicBrickEntities) {
        auto s_BrickEntity = s_Brick.second;

        if (!s_BrickEntity) {
            continue;
        }

        s_EntitiesToProcess.push_back({s_BrickEntity, s_EntitySceneContext->m_aDynamicBrickBlueprintFactories[s_Brick.first]});
    }

    UpdateEntityTree(s_NodeMap, s_EntitiesToProcess, true, false);

    AddDynamicEntitiesToEntityTree(s_SceneNode, s_NodeMap);

    // Update the cached tree.
    m_CachedEntityTreeMutex.lock();
    m_CachedEntityTree = std::move(s_SceneNode);
    m_CachedEntityTreeMap = std::move(s_NodeMap);
    m_CachedEntityTreeMutex.unlock();
}

void Editor::UpdateEntityTree(
    std::unordered_map<ZEntityRef, std::shared_ptr<EntityTreeNode>>& p_NodeMap,
    const std::vector<std::pair<ZEntityRef, IEntityBlueprintFactory*>>& p_Entities, bool p_AreBricksDynamic, bool p_AreEntitiesDynamic
) {
    if (m_IsBuildingEntityTree.exchange(true)) {
        return;
    }

    // Go through a first pass by creating all the nodes of the tree using a BFS
    // approach. We'll also opportunistically assign children nodes to parents we've
    // seen before. Then, as a second pass we'll go through and assign the remaining
    // children nodes to their parents.

    std::queue<std::pair<ZEntityBlueprintFactoryBase*, ZEntityRef>> s_NodeQueue;
    std::queue<std::shared_ptr<EntityTreeNode>> s_ParentlessNodes;

    for (const auto& [s_Entity, s_EntityFactory] : p_Entities) {
        if (!s_Entity || !s_EntityFactory) {
            continue;
        }

        if (p_AreEntitiesDynamic && p_NodeMap.contains(s_Entity)) {
            continue;
        }

        s_NodeQueue.emplace(static_cast<ZEntityBlueprintFactoryBase*>(s_EntityFactory), s_Entity);
    }

    const auto s_EntitySceneContext = SDK()->Globals()->GameSceneflowModule->m_pEntitySceneContext;
    const auto s_SceneEntity = s_EntitySceneContext->m_pScene.m_entityRef;
    const std::shared_ptr<EntityTreeNode> s_SceneNode = p_NodeMap[s_SceneEntity];
    const std::shared_ptr<EntityTreeNode> s_UnparentedEntitiesNode = s_SceneNode->m_Children.find("Unparented entities")->second;
    std::shared_ptr<EntityTreeNode> s_DynamicBricksNode;
    std::shared_ptr<EntityTreeNode> s_DynamicEntitiesNode;

    if (p_AreBricksDynamic) {
        s_DynamicBricksNode = s_SceneNode->m_Children.find("Dynamic bricks")->second;
    }
    else if (p_AreEntitiesDynamic) {
        s_DynamicEntitiesNode = s_SceneNode->m_Children.find("Dynamic entities")->second;
    }

    while (!s_NodeQueue.empty()) {
        // Pop the next factory and its root entity off the queue.
        auto [s_CurrentFactory, s_CurrentRoot] = s_NodeQueue.front();
        s_NodeQueue.pop();

        if (!s_CurrentRoot) {
            continue;
        }

        const auto s_SubEntityCount = s_CurrentFactory->GetSubEntitiesCount();
        const bool s_IsTemplateEntityBlueprintFactory = s_CurrentFactory->IsTemplateEntityBlueprintFactory();
        const bool s_IsAspectEntityBlueprintFactory = s_CurrentFactory->IsAspectEntityBlueprintFactory();

        // Go through each of its sub-entities and create nodes for them.
        if (s_SubEntityCount > 0) {
            for (uint64_t i = 0; i < s_SubEntityCount; ++i) {
                ZEntityRef s_SubEntity = s_CurrentFactory->GetSubEntity(s_CurrentRoot.m_pObj, i);
                const auto s_SubEntityFactory = s_CurrentFactory->GetSubEntityBlueprint(i);

                if (!s_SubEntity) {
                    if (!s_SubEntity.m_pObj || !s_SubEntity.GetEntity()) {
                        // This entity may not exist because the current platform is excluded in excludedPlatforms.
                        continue;
                    }

                    Logger::Error(
                        "[Editor] Entity with index {} in blueprint factory {:016X} isn't valid!", i, s_CurrentFactory->m_ridResource.GetID()
                    );

                    continue;
                }

                if (s_SubEntity && reinterpret_cast<int64_t>(*s_SubEntity.m_pObj) < 0) {
                    continue;
                }

                if (!s_SubEntity.GetEntity() || !s_SubEntity->GetType()) {
                    continue;
                }

                // Skip the root entity of the referenced factory
                if (p_NodeMap.contains(s_SubEntity)) {
                    p_NodeMap[s_SubEntity]->m_IsRootEntity = true;

                    /**
                     * Enqueue sub-entities of the referenced factory to ensure they are processed
                     * even when the root entity is skipped
                     */
                    if (s_SubEntityFactory && s_SubEntityFactory->GetSubEntitiesCount() > 0) {
                        s_NodeQueue.emplace(s_SubEntityFactory, s_SubEntity);
                    }

                    continue;
                }

                /*
                 * For brick entities, m_nEntityID is the FNV1a-32 hash of the source resource ID
                 * (e.g. assembly:/_knt/scenes/globalbricks/gameessentials.brick).
                 */
                const auto s_SubEntityID = s_SubEntity->GetType()->m_nEntityID;
                std::string s_EntityName = "<noname>";
                std::string s_BlueprintFactorySourceResourceID;
                bool s_IsRootEntity = false;

                // If our current factory is a template factory, we can get the name of the entity from it.
                if (s_IsTemplateEntityBlueprintFactory) {
                    const auto s_TemplateBpFactory = reinterpret_cast<ZTemplateEntityBlueprintFactory*>(s_CurrentFactory);

                    if (s_TemplateBpFactory->m_pTemplateEntityBlueprint) {
                        s_EntityName = s_TemplateBpFactory->m_pTemplateEntityBlueprint->subEntities[i].entityName;
                        s_BlueprintFactorySourceResourceID = s_TemplateBpFactory->m_pTemplateEntityBlueprint->sourceResourceID;
                        s_IsRootEntity = i == s_TemplateBpFactory->m_rootEntityIndex;
                    }
                }
                else if (s_IsAspectEntityBlueprintFactory) {
                    const auto s_AspectEntityBlueprintFactory = reinterpret_cast<ZAspectEntityBlueprintFactory*>(s_CurrentFactory);
                    const uint32_t s_AspectIndex = s_AspectEntityBlueprintFactory->m_aSubEntitiesLookUp[i].m_nAspectIdx;
                    const uint32_t s_SubEntityIndex = s_AspectEntityBlueprintFactory->m_aSubEntitiesLookUp[i].m_nSubentityIdx;
                    const auto s_TemplateBpFactory =
                        reinterpret_cast<ZTemplateEntityBlueprintFactory*>(s_AspectEntityBlueprintFactory->m_blueprintResources[s_AspectIndex]);

                    if (s_TemplateBpFactory->m_pTemplateEntityBlueprint) {
                        s_EntityName = s_TemplateBpFactory->m_pTemplateEntityBlueprint->subEntities[s_SubEntityIndex].entityName;
                        s_IsRootEntity = s_SubEntityIndex == s_TemplateBpFactory->m_rootEntityIndex;
                    }
                }

                std::string s_ReferencedBlueprintFactorySourceResourceID;

                if (s_SubEntityFactory->IsTemplateEntityBlueprintFactory()) {
                    const auto s_TemplateBpFactory = reinterpret_cast<ZTemplateEntityBlueprintFactory*>(s_SubEntityFactory);

                    if (s_TemplateBpFactory->m_pTemplateEntityBlueprint) {
                        s_ReferencedBlueprintFactorySourceResourceID = s_TemplateBpFactory->m_pTemplateEntityBlueprint->sourceResourceID;
                    }
                }

                if (const auto s_Name = m_EntityNames.find(s_SubEntity); s_Name != m_EntityNames.end()) {
                    s_EntityName = s_Name->second;
                }

                const bool s_IsEntityIDGenerated = p_AreEntitiesDynamic && m_EntityIDToUUID.contains(s_SubEntityID);

                // Format a human-readable name for the entity.
                const auto s_EntityTypeName = (*s_SubEntity->GetType()->m_pInterfaceData)[0].m_Type->GetTypeInfo()->pszTypeName;
                const auto s_EntityHumanName = fmt::format(
                    "{} ({:016x}{}){}", s_EntityName, s_SubEntityID,
                    (s_SubEntityID >> 32) == 0 ? fmt::format(" - {}", s_BlueprintFactorySourceResourceID) : "",
                    p_AreEntitiesDynamic ? (s_IsEntityIDGenerated ? " **" : " *") : ""
                );

                std::string s_ReferencedBlueprintFactoryType;

                if (s_SubEntityFactory->IsTemplateEntityBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "TBLU";
                }
                else if (s_SubEntityFactory->IsAspectEntityBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "ASEB";
                }
                else if (s_SubEntityFactory->IsCppEntityBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "CBLU";
                }
                else if (s_SubEntityFactory->IsExtendedCppEntityBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "ECPB";
                }
                else if (s_SubEntityFactory->IsUIControlBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "UICB";
                }
                else if (s_SubEntityFactory->IsRenderMaterialEntityBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "MATB";
                }
                else if (s_SubEntityFactory->IsAudioSwitchBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "WSWB";
                }
                else if (s_SubEntityFactory->IsAudioStateBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "WSGB";
                }
                else if (s_SubEntityFactory->IsPadEntityBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "PEBL";
                }
                else if (s_SubEntityFactory->IsShadernodeEntityBlueprintFactory()) {
                    s_ReferencedBlueprintFactoryType = "SNEB";
                }

                // Add the node to the map.
                const auto s_SubEntityNode = std::make_shared<EntityTreeNode>(
                    s_EntityHumanName, s_EntityTypeName, s_SubEntityID, s_CurrentFactory->m_ridResource,
                    s_IsTemplateEntityBlueprintFactory ? "TBLU" : "ASEB", s_SubEntityFactory->m_ridResource, s_ReferencedBlueprintFactoryType,
                    s_BlueprintFactorySourceResourceID, s_ReferencedBlueprintFactorySourceResourceID, s_SubEntity, s_IsRootEntity,
                    p_AreEntitiesDynamic
                );

                const auto s_LogicalParent = s_SubEntity.GetLogicalParent();

                if (s_LogicalParent) {
                    auto s_ParentNode = p_NodeMap.find(s_LogicalParent);

                    if (s_ParentNode != p_NodeMap.end()) {
                        // If we have already seen the logical parent of this sub-entity, add it to the parent's children.
                        if (p_AreBricksDynamic && s_ParentNode->second == s_SceneNode) {
                            s_DynamicBricksNode->m_Children.insert({s_EntityHumanName, s_SubEntityNode});
                            s_SubEntityNode->m_Parent = s_DynamicBricksNode;
                        }
                        else if (p_AreEntitiesDynamic && s_ParentNode->second == s_SceneNode) {
                            s_DynamicEntitiesNode->m_Children.insert({s_EntityHumanName, s_SubEntityNode});
                            s_SubEntityNode->m_Parent = s_DynamicEntitiesNode;
                        }
                        else {
                            s_ParentNode->second->m_Children.insert({s_EntityHumanName, s_SubEntityNode});
                            s_SubEntityNode->m_Parent = s_ParentNode->second;
                        }
                    }
                    else {
                        // Otherwise, add it to the parentless nodes queue.
                        s_ParentlessNodes.push(s_SubEntityNode);
                    }
                }
                else {
                    // If it has no logical parent, add it to the parentless nodes queue.
                    s_ParentlessNodes.push(s_SubEntityNode);
                }

                // If the sub-entity has a factory with more sub-entities, add it to the queue.
                if (s_SubEntityFactory && s_SubEntityFactory->GetSubEntitiesCount() > 0) {
                    s_NodeQueue.emplace(s_SubEntityFactory, s_SubEntity);
                }

                p_NodeMap[s_SubEntity] = s_SubEntityNode;
            }
        }
        else if (p_AreEntitiesDynamic) {
            if (p_NodeMap.contains(s_CurrentRoot)) {
                continue;
            }

            const auto s_SubEntityID = s_CurrentRoot->GetType()->m_nEntityID;
            const bool s_IsEntityIDGenerated = p_AreEntitiesDynamic && m_EntityIDToUUID.contains(s_SubEntityID);

            // Format a human-readable name for the entity.
            const auto s_EntityTypeName = (*s_CurrentRoot->GetType()->m_pInterfaceData)[0].m_Type->GetTypeInfo()->pszTypeName;
            const auto s_EntityHumanName = fmt::format(
                "{} ({:016x}{}){}", s_EntityTypeName, s_SubEntityID, "", p_AreEntitiesDynamic ? (s_IsEntityIDGenerated ? " **" : " *") : ""
            );

            std::string s_BlueprintFactoryType;

            if (s_CurrentFactory->IsTemplateEntityBlueprintFactory()) {
                s_BlueprintFactoryType = "TBLU";
            }
            else if (s_CurrentFactory->IsCppEntityBlueprintFactory()) {
                s_BlueprintFactoryType = "CBLU";
            }
            else if (s_CurrentFactory->IsExtendedCppEntityBlueprintFactory()) {
                s_BlueprintFactoryType = "ECPB";
            }
            else if (s_CurrentFactory->IsUIControlBlueprintFactory()) {
                s_BlueprintFactoryType = "UICB";
            }
            else if (s_CurrentFactory->IsRenderMaterialEntityBlueprintFactory()) {
                s_BlueprintFactoryType = "MATB";
            }
            else if (s_CurrentFactory->IsAudioSwitchBlueprintFactory()) {
                s_BlueprintFactoryType = "WSWB";
            }
            else if (s_CurrentFactory->IsAudioStateBlueprintFactory()) {
                s_BlueprintFactoryType = "WSGB";
            }
            else if (s_CurrentFactory->IsPadEntityBlueprintFactory()) {
                s_BlueprintFactoryType = "PEBL";
            }
            else if (s_CurrentFactory->IsShadernodeEntityBlueprintFactory()) {
                s_BlueprintFactoryType = "SNEB";
            }

            // Add the node to the map.
            const auto s_SubEntityNode = std::make_shared<EntityTreeNode>(
                s_EntityHumanName, s_EntityTypeName, s_SubEntityID, s_CurrentFactory->m_ridResource, s_BlueprintFactoryType, -1, "", "", "",
                s_CurrentRoot, true, p_AreEntitiesDynamic
            );

            const auto s_LogicalParent = s_CurrentRoot.GetLogicalParent();

            if (s_LogicalParent) {
                auto s_ParentNode = p_NodeMap.find(s_LogicalParent);

                if (s_ParentNode != p_NodeMap.end()) {
                    // If we have already seen the logical parent of this sub-entity, add it to the parent's children.
                    if (p_AreBricksDynamic && s_ParentNode->second == s_SceneNode) {
                        s_DynamicBricksNode->m_Children.insert({s_EntityHumanName, s_SubEntityNode});
                        s_SubEntityNode->m_Parent = s_DynamicBricksNode;
                    }
                    else if (p_AreEntitiesDynamic && s_ParentNode->second == s_SceneNode) {
                        s_DynamicEntitiesNode->m_Children.insert({s_EntityHumanName, s_SubEntityNode});
                        s_SubEntityNode->m_Parent = s_DynamicEntitiesNode;
                    }
                    else {
                        s_ParentNode->second->m_Children.insert({s_EntityHumanName, s_SubEntityNode});
                        s_SubEntityNode->m_Parent = s_ParentNode->second;
                    }
                }
                else {
                    // Otherwise, add it to the parentless nodes queue.
                    s_ParentlessNodes.push(s_SubEntityNode);
                }
            }
            else {
                // If it has no logical parent, add it to the parentless nodes queue.
                s_ParentlessNodes.push(s_SubEntityNode);
            }

            p_NodeMap[s_CurrentRoot] = s_SubEntityNode;
        }
    }

    // Go through the nodes and assign any remaining children to their parents.
    while (!s_ParentlessNodes.empty()) {
        const auto s_Node = s_ParentlessNodes.front();
        s_ParentlessNodes.pop();

        // Skip entities from second and later factories referenced by aspect factories
        if (s_Node->m_Entity && *s_Node->m_Entity.m_pObj && reinterpret_cast<intptr_t>(*s_Node->m_Entity.m_pObj) & 1) {
            continue;
        }

        const auto s_LogicalParent = s_Node->m_Entity.GetLogicalParent();

        // If it has a logical parent and that parent is in the map, add it to the parent's children.
        if (s_LogicalParent) {
            auto s_ParentNode = p_NodeMap.find(s_LogicalParent);

            if (s_ParentNode != p_NodeMap.end()) {
                if (p_AreBricksDynamic && s_ParentNode->second == s_SceneNode) {
                    s_DynamicBricksNode->m_Children.insert({s_Node->m_Name, s_Node});
                    s_Node->m_Parent = s_DynamicBricksNode;
                }
                if (p_AreEntitiesDynamic && s_ParentNode->second == s_SceneNode) {
                    s_DynamicEntitiesNode->m_Children.insert({s_Node->m_Name, s_Node});
                    s_Node->m_Parent = s_DynamicEntitiesNode;
                }
                else {
                    s_ParentNode->second->m_Children.insert({s_Node->m_Name, s_Node});
                    s_Node->m_Parent = s_ParentNode->second;
                }

                continue;
            }
        }

        if (p_AreBricksDynamic) {
            s_DynamicBricksNode->m_Children.insert({s_Node->m_Name, s_Node});
            s_Node->m_Parent = s_DynamicBricksNode;
        }
        else if (p_AreEntitiesDynamic) {
            s_DynamicEntitiesNode->m_Children.insert({s_Node->m_Name, s_Node});
            s_Node->m_Parent = s_DynamicEntitiesNode;
        }
        else {
            // Otherwise, add it to the "Unparented entities" node.
            s_UnparentedEntitiesNode->m_Children.insert({s_Node->m_Name, s_Node});
            s_Node->m_Parent = s_UnparentedEntitiesNode;
        }
    }

    m_IsBuildingEntityTree = false;
}

void Editor::AddDynamicEntitiesToEntityTree(
    const std::shared_ptr<EntityTreeNode>& p_SceneNode, std::unordered_map<ZEntityRef, std::shared_ptr<EntityTreeNode>>& p_NodeMap
) {
    auto s_DynamicEntitiesNode =
        std::make_shared<EntityTreeNode>("Dynamic entities", "", -1, -1, "", -1, "", "", "", m_DynamicEntitiesNodeEntityRef, false);

    p_SceneNode->m_Children.insert(std::make_pair(s_DynamicEntitiesNode->m_Name, s_DynamicEntitiesNode));
    s_DynamicEntitiesNode->m_Parent = p_SceneNode;

    std::vector<std::pair<ZEntityRef, IEntityBlueprintFactory*>> s_DynamicEntities;
    {
        s_DynamicEntities.reserve(SDK()->Globals()->EntityManager->m_DynamicEntities.size());

        for (const auto& s_Pair : SDK()->Globals()->EntityManager->m_DynamicEntities) {
            if (s_Pair.first == 0x8ac84c75461e10c0 || s_Pair.second.first->GetType()->m_nEntityID == 0x8ac84c75461e10c0) {
                int a = 2;
            }
            s_DynamicEntities.push_back({s_Pair.second.first, s_Pair.second.second.GetResource()->GetBlueprint()});
        }

        m_PendingDynamicEntities.clear();
    }

    if (!s_DynamicEntities.empty()) {
        UpdateEntityTree(p_NodeMap, s_DynamicEntities, false, true);
    }
}

void Editor::OnSelectEntity(ZEntityRef p_Entity, bool p_ShouldScrollToEntity, const std::optional<std::string> p_ClientId) {
    if (m_SelectedEntity.m_pObj == p_Entity.m_pObj) {
        return;
    }

    if (!IsSpecialEntityTreeNode(p_Entity)) {
        m_ScrollToEntity = p_ShouldScrollToEntity && p_Entity;

        m_Server.OnEntitySelected(p_Entity, std::move(p_ClientId));

        if (SpawnEntities()) {
            m_SelectionForFreeCameraEditorStyleEntity->m_selection.clear();

            if (p_Entity) {
                m_SelectionForFreeCameraEditorStyleEntity->m_selection.push_back(p_Entity);
            }
        }
    }

    m_SelectedEntity = p_Entity;
}

void Editor::OnDestroyEntity(ZEntityRef p_Entity, std::optional<std::string> p_ClientId) {
    m_EntityDestructionMutex.lock();
    m_EntitiesToDestroy.push_back({p_Entity, std::move(p_ClientId)});
    m_EntityDestructionMutex.unlock();
}

void Editor::OnEntityNameChange(ZEntityRef p_Entity, const std::string& p_Name, std::optional<std::string> p_ClientId) {
    m_CachedEntityTreeMutex.lock();
    m_EntityNames[p_Entity] = p_Name;
    m_CachedEntityTreeMutex.unlock();

    m_Server.OnEntityNameChanged(p_Entity, std::move(p_ClientId));
}

void Editor::DestroyEntityInternal(ZEntityRef p_Entity, std::optional<std::string> p_ClientId) {
    m_CachedEntityTreeMutex.lock();

    m_EntityNames.erase(p_Entity);
    m_SpawnedEntities.erase(p_Entity->GetType()->m_nEntityID);

    if (m_SelectedEntity == p_Entity) {
        m_SelectedEntity = {};
    }

    if (m_SelectedDebugEntity == p_Entity) {
        m_SelectedDebugEntity = {};
    }

    // Remove from the tree.
    const auto s_EntityIter = m_CachedEntityTreeMap.find(p_Entity);

    if (s_EntityIter != m_CachedEntityTreeMap.end()) {
        const auto s_NodeToRemove = s_EntityIter->second;
        m_CachedEntityTreeMap.erase(s_EntityIter);

        // If a child of this node is selected, deselect it (non-recursive).
        std::queue<std::shared_ptr<EntityTreeNode>> s_ChildrenQueue;

        for (auto& s_Child : s_NodeToRemove->m_Children) {
            s_ChildrenQueue.push(s_Child.second);
        }

        m_OpenEntityTreeNodes.erase(s_NodeToRemove.get());
        m_FilteredEntityTreeNodes.erase(s_NodeToRemove.get());

        while (!s_ChildrenQueue.empty()) {
            auto s_CurrentChild = s_ChildrenQueue.front();

            m_OpenEntityTreeNodes.erase(s_CurrentChild.get());
            m_FilteredEntityTreeNodes.erase(s_CurrentChild.get());

            if (m_SelectedEntity == s_CurrentChild->m_Entity) {
                m_SelectedEntity = {};
            }

            if (m_SelectedDebugEntity == s_CurrentChild->m_Entity) {
                m_SelectedDebugEntity = {};
            }

            for (auto& s_Child : s_CurrentChild->m_Children) {
                s_ChildrenQueue.push(s_Child.second);
            }

            s_ChildrenQueue.pop();
        }

        // Remove it from the children of it's parent.
        if (auto s_Parent = s_NodeToRemove->m_Parent.lock()) {
            for (auto it = s_Parent->m_Children.begin(); it != s_Parent->m_Children.end(); ++it) {
                if (it->second == s_NodeToRemove) {
                    s_Parent->m_Children.erase(it);
                    break;
                }
            }
        }
    }

    m_CachedEntityTreeMutex.unlock();

    m_Server.OnEntityDestroying(p_Entity->GetType()->m_nEntityID, std::move(p_ClientId));
    SDK()->Functions()->ZEntityManager_DeleteEntity->Call(SDK()->Globals()->EntityManager, p_Entity);
}

void Editor::DestroyEntityNodeInternal(const std::shared_ptr<EntityTreeNode>& p_NodeToRemove, std::optional<std::string> p_ClientId) {
    if (!p_NodeToRemove) {
        return;
    }

    const uint64_t s_EntityID = p_NodeToRemove->m_EntityID;

    std::scoped_lock s_Lock(m_CachedEntityTreeMutex);

    m_OpenEntityTreeNodes.erase(p_NodeToRemove.get());
    m_FilteredEntityTreeNodes.erase(p_NodeToRemove.get());

    if (p_NodeToRemove->m_Entity) {
        m_CachedEntityTreeMap.erase(p_NodeToRemove->m_Entity);
        m_EntityNames.erase(p_NodeToRemove->m_Entity);

        if (const auto p_Type = p_NodeToRemove->m_Entity->GetType()) {
            m_SpawnedEntities.erase(p_Type->m_nEntityID);
        }

        if (m_SelectedEntity == p_NodeToRemove->m_Entity) {
            m_SelectedEntity = {};
        }

        if (m_SelectedDebugEntity == p_NodeToRemove->m_Entity) {
            m_SelectedDebugEntity = {};
        }
    }

    // If a child of this node is selected, deselect it (non-recursive).
    std::queue<std::shared_ptr<EntityTreeNode>> s_ChildrenQueue;

    for (auto& s_Child : p_NodeToRemove->m_Children) {
        s_ChildrenQueue.push(s_Child.second);
    }

    while (!s_ChildrenQueue.empty()) {
        auto s_CurrentChild = s_ChildrenQueue.front();
        s_ChildrenQueue.pop();

        m_OpenEntityTreeNodes.erase(s_CurrentChild.get());
        m_FilteredEntityTreeNodes.erase(s_CurrentChild.get());

        if (m_SelectedEntity == s_CurrentChild->m_Entity) {
            m_SelectedEntity = {};
        }

        if (m_SelectedDebugEntity == s_CurrentChild->m_Entity) {
            m_SelectedDebugEntity = {};
        }

        for (auto& s_Child : s_CurrentChild->m_Children) {
            s_ChildrenQueue.push(s_Child.second);
        }
    }

    if (auto s_Parent = p_NodeToRemove->m_Parent.lock()) {
        for (auto it = s_Parent->m_Children.begin(); it != s_Parent->m_Children.end(); ++it) {
            if (it->second == p_NodeToRemove) {
                s_Parent->m_Children.erase(it);
                break;
            }
        }
    }

    m_Server.OnEntityDestroying(s_EntityID, std::move(p_ClientId));
}

bool Editor::IsSpecialEntityTreeNode(ZEntityRef p_Entity) const {
    return p_Entity == m_DynamicBricksNodeEntityRef || p_Entity == m_DynamicEntitiesNodeEntityRef || p_Entity == m_UnparentedEntitiesNodeEntityRef;
}

std::shared_ptr<EntityTreeNode> Editor::GetRootEntity(std::shared_ptr<EntityTreeNode> p_Node) {
    while (p_Node) {
        const auto s_ParentNode = p_Node->m_Parent.lock();

        if (!s_ParentNode) {
            return p_Node;
        }

        if (s_ParentNode->m_IsRootEntity) {
            return s_ParentNode;
        }

        p_Node = s_ParentNode;
    }

    return nullptr;
}

bool Editor::IsEditorEntity(const ZEntityRef& p_Entity) const {
    return m_EntityNames.contains(p_Entity);
}

DEFINE_PLUGIN_DETOUR(
    Editor, void, ZEntitySceneContext_LoadDynamicBrick, ZEntitySceneContext* th, const ZRuntimeResourceID& runtimeResourceID, ZEntityRef entityRef,
    IEntityFactory* entityFactory
) {
    if (m_CachedEntityTree && !m_IsBuildingEntityTree.load()) {
        std::scoped_lock lock(m_PendingDynamicBricksMutex);
        m_PendingDynamicBricks.insert({entityRef, entityFactory->GetBlueprint()});
    }

    m_Server.OnDynamicBrickLoaded(runtimeResourceID);

    return {HookAction::Continue()};
}

DEFINE_PLUGIN_DETOUR(Editor, void, ZEntitySceneContext_UnloadDynamicBrick, ZEntitySceneContext* th, const ZRuntimeResourceID& runtimeResourceID) {
    m_Server.OnDynamicBrickUnloaded(runtimeResourceID);

    auto s_DynamicBrickIt = th->m_aDynamicBrickEntities.find(runtimeResourceID);

    if (s_DynamicBrickIt == th->m_aDynamicBrickEntities.end()) {
        return {HookAction::Continue()};
    }

    ZEntityRef s_EntityRef = s_DynamicBrickIt->second;

    if (m_SelectedEntity == s_EntityRef) {
        m_SelectedEntity = nullptr;
    }

    if (m_SelectedDebugEntity == s_EntityRef) {
        m_SelectedDebugEntity = {};
    }

    std::shared_ptr<EntityTreeNode> s_NodeToRemove;

    {
        std::shared_lock s_Lock(m_CachedEntityTreeMutex);

        if (m_CachedEntityTree && !m_IsBuildingEntityTree.load()) {
            auto it = m_CachedEntityTreeMap.find(s_EntityRef);

            if (it != m_CachedEntityTreeMap.end()) {
                s_NodeToRemove = it->second;
            }
        }
    }

    if (s_NodeToRemove) {
        s_NodeToRemove->m_IsPendingDeletion = true;

        // DeleteDebugEntity(s_NodeToRemove->Entity);
        DestroyEntityNodeInternal(s_NodeToRemove, std::nullopt);
    }

    {
        std::scoped_lock lock(m_PendingDynamicBricksMutex);
        m_PendingDynamicBricks.erase(s_EntityRef);
    }

    m_EntityRefToFactoryRuntimeResourceIDs.erase(s_EntityRef);

    return {HookAction::Continue()};
}

DEFINE_PLUGIN_DETOUR(
    Editor, ZEntityRef*, ZEntityManager_NewUninitializedEntity, ZEntityManager* th, ZEntityRef& result, const ZString& sDebugName,
    TResourcePtr<IEntityFactory>& entityFactory, const ZEntityRef& logicalParent, uint64_t entityID, void* unk0, bool unk1
) {
    ZEntityRef* s_EntityRef = p_Hook->CallOriginal(th, result, sDebugName, entityFactory, logicalParent, entityID, unk0, unk1);

    if (m_CachedEntityTree && !m_IsBuildingEntityTree.load()) {
        std::scoped_lock lock(m_PendingDynamicEntitiesMutex);
        m_PendingDynamicEntities.insert({result, entityFactory.GetResource()->GetBlueprint()});
    }

    return HookResult<ZEntityRef*>(HookAction::Return(), s_EntityRef);
}

DEFINE_PLUGIN_DETOUR(Editor, uint64_t, GenerateEntityIdentifier, SEntityIdentifier* p_Identifier) {
    auto s_EntityID = p_Hook->CallOriginal(p_Identifier);

    m_EntityIDToUUID.insert({p_Identifier->m_EntityID, p_Identifier->m_UUID.c_str()});

    return {HookAction::Return(), s_EntityID};
}

DEFINE_PLUGIN_DETOUR(Editor, void, ZEntityManager_DeleteEntity, ZEntityManager* th, const ZEntityRef& entityRef) {
    if (m_SelectedEntity == entityRef) {
        m_SelectedEntity = nullptr;
    }

    if (m_SelectedDebugEntity == entityRef) {
        m_SelectedDebugEntity = {};
    }

    std::shared_ptr<EntityTreeNode> s_NodeToRemove;

    {
        std::shared_lock s_Lock(m_CachedEntityTreeMutex);

        if (m_CachedEntityTree && !m_IsBuildingEntityTree.load()) {
            auto it = m_CachedEntityTreeMap.find(entityRef);

            if (it != m_CachedEntityTreeMap.end()) {
                s_NodeToRemove = it->second;
            }
        }
    }

    if (s_NodeToRemove) {
        s_NodeToRemove->m_IsPendingDeletion = true;

        // DeleteDebugEntity(s_NodeToRemove->Entity);
        DestroyEntityNodeInternal(s_NodeToRemove, std::nullopt);
    }

    {
        std::scoped_lock lock(m_PendingDynamicEntitiesMutex);
        m_PendingDynamicEntities.erase(entityRef);
    }

    m_EntityRefToFactoryRuntimeResourceIDs.erase(entityRef);

    return {HookAction::Continue()};
}
