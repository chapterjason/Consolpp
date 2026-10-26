#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"

#include <array>
#include <optional>
#include <string_view>
#include <utility>

namespace Consolpp::Markup
{
    // The color attributes of the style element and the members of Style they set.
    inline constexpr std::array<std::pair<std::string_view, std::optional<Color> Style::*>, 2> colorAttributes{
        {
            {"foreground", &Style::foreground},
            {"background", &Style::background},
        },
    };
} // namespace Consolpp::Markup
