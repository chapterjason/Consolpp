#pragma once

#include "Consolpp/Markup/Style.hpp"

#include <array>
#include <string_view>
#include <utility>

namespace Consolpp::Markup
{
    // The flag attributes of the style element and the members of Style they set.
    inline constexpr std::array<std::pair<std::string_view, bool Style::*>, 5> flagAttributes{
        {
            {"bold", &Style::bold},
            {"underline", &Style::underline},
            {"blink", &Style::blink},
            {"reverse", &Style::reverse},
            {"conceal", &Style::conceal},
        },
    };
} // namespace Consolpp::Markup
