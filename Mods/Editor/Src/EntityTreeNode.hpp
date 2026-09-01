#pragma once

#include <map>
#include <string>

#include <Glacier/ZResourceID.hpp>
#include <Glacier/ZEntity.hpp>

#include "EntityNameCompare.hpp"

struct EntityTreeNode {
    std::string m_Name;
    std::string m_EntityType;
    uint64_t m_EntityID;
    ZRuntimeResourceID m_BlueprintFactory;
    std::string m_BlueprintFactoryType;
    ZRuntimeResourceID m_ReferencedBlueprintFactory;
    std::string m_ReferencedBlueprintFactoryType;
    std::string m_BlueprintFactorySourceResourceID;
    std::string m_ReferencedBlueprintFactorySourceResourceID;
    ZEntityRef m_Entity;
    std::multimap<std::string, std::shared_ptr<EntityTreeNode>, EntityNameCompare> m_Children;
    std::weak_ptr<EntityTreeNode> m_Parent;
    bool m_IsRootEntity;
    bool m_IsDynamicEntity;
    std::atomic<bool> m_IsPendingDeletion{false};
    bool m_IsDirectMatch = false;

    EntityTreeNode(
        const std::string& p_Name, const std::string& p_type, uint64_t p_EntityID, ZRuntimeResourceID p_BlueprintFactory,
        const std::string& p_BlueprintFactoryType, ZRuntimeResourceID p_ReferencedBlueprintFactory,
        const std::string& p_ReferencedBlueprintFactoryType, const std::string& p_BlueprintFactorySourceResourceID,
        const std::string& p_ReferencedBlueprintFactorySourceResourceID, ZEntityRef p_Ref, bool p_IsRootEntity, bool p_IsDynamicEntity = false
    )
        : m_Name(p_Name)
        , m_EntityType(p_type)
        , m_EntityID(p_EntityID)
        , m_BlueprintFactory(p_BlueprintFactory)
        , m_BlueprintFactoryType(p_BlueprintFactoryType)
        , m_ReferencedBlueprintFactory(p_ReferencedBlueprintFactory)
        , m_ReferencedBlueprintFactoryType(p_ReferencedBlueprintFactoryType)
        , m_BlueprintFactorySourceResourceID(p_ReferencedBlueprintFactoryType)
        , m_ReferencedBlueprintFactorySourceResourceID(p_ReferencedBlueprintFactorySourceResourceID)
        , m_Entity(p_Ref)
        , m_IsRootEntity(p_IsRootEntity)
        , m_IsDynamicEntity(p_IsDynamicEntity) {}

    std::string_view GetEntityName() const {
        const std::string_view s_Name = m_Name;
        const size_t s_Position = s_Name.rfind(" (");

        return s_Position != std::string_view::npos ? s_Name.substr(0, s_Position) : s_Name;
    }
};
