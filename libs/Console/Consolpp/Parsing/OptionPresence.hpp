#pragma once

#include <cstdint>

namespace Consolpp::Parsing
{
    enum class OptionPresence : std::uint8_t
    {
        Absent,
        Given,
        Negated,
    };
} // namespace Consolpp::Parsing
