#pragma once

#include <simdjson.h>

#include "EditorTypes.hpp"

namespace zknt::editor_json {
    EntitySelector ReadEntitySelector(simdjson::ondemand::object p_Object);
}
