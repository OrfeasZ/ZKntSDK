#include "EditorJson.hpp"

namespace zknt::editor_json {
    EntitySelector ReadEntitySelector(simdjson::ondemand::object p_Selector) {
        const std::string_view s_IdString = p_Selector["id"];
        const auto s_Id64 = std::stoull(std::string(s_IdString), nullptr, 16);

        auto s_TbluField = p_Selector.find_field("tblu");

        if (s_TbluField.error() == simdjson::SUCCESS) {
            const std::string_view s_TbluString = s_TbluField;
            const auto s_Tblu64 = std::stoull(std::string(s_TbluString), nullptr, 16);

            return {
                .EntityId = s_Id64,
                .TbluHash = std::make_optional(s_Tblu64),
                .PrimHash = std::nullopt,
            };
        }

        return {
            .EntityId = s_Id64,
            .TbluHash = std::nullopt,
            .PrimHash = std::nullopt,
        };
    }
}
