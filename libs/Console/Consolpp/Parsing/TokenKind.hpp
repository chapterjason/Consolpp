#pragma once

#include <cstdint>

namespace Consolpp::Parsing
{
    enum class TokenKind : std::uint8_t
    {
        LongOption,
        ShortOptions,
        Separator,
        Dash,
        Positional,
    };
} // namespace Consolpp::Parsing
