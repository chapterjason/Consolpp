#pragma once

#include <cstdint>

namespace Consolpp::Text::Grapheme
{
    // The Indic_Conjunct_Break property used by rule GB9c of UAX #29.
    enum class IndicConjunctBreak : std::uint8_t
    {
        None,
        Linker,
        Consonant,
        Extend,
    };
} // namespace Consolpp::Text::Grapheme
