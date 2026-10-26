#pragma once

#include <array>
#include <string_view>

namespace Consolpp::BuiltIn
{
    inline constexpr std::array<std::string_view, 3> formatNames{"txt", "json", "md"};
} // namespace Consolpp::BuiltIn
