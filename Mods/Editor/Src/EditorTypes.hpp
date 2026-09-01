#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <Glacier/ZEntity.hpp>
#include <Glacier/ZMath.hpp>

struct EntitySelector {
    uint64_t EntityId;
    std::optional<ZRuntimeResourceID> TbluHash;
    std::optional<std::string> PrimHash;
};

struct NavKitMatiTextures {
    std::string m_DiffuseTextureHash;
    std::string m_NormalTextureHash;
    std::string m_SpecularTextureHash;
};

struct NavKitMeshEntity {
    NavKitMeshEntity(
        std::string p_AlocHash, std::string p_PrimHash, const Quat p_Quat, std::string p_FolderName, std::string p_RoomName, const ZEntityRef p_Entity
    )
        : m_AlocHash(std::move(p_AlocHash))
        , m_PrimHash(std::move(p_PrimHash))
        , m_Quat(p_Quat)
        , m_RoomName(std::move(p_RoomName))
        , m_FolderName(std::move(p_FolderName))
        , m_Entity(p_Entity) {}

    std::string m_AlocHash;
    std::string m_PrimHash;
    Quat m_Quat;
    std::string m_RoomName;
    std::string m_FolderName;
    ZEntityRef m_Entity;
};
