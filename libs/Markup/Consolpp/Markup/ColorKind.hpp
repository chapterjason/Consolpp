#pragma once

#include <cstdint>

namespace Consolpp::Markup
{
    enum class ColorKind : std::uint8_t
    {
        Default,
        Basic,
        Bright,
        Rgb,
    };
} // namespace Consolpp::Markup
