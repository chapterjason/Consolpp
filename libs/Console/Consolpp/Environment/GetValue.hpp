#pragma once

#include <optional>
#include <string_view>

namespace Consolpp::Environment
{
    std::optional<std::string_view> getValue(const char* name);
} // namespace Consolpp::Environment
