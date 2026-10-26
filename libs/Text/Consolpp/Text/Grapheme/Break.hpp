#pragma once

#include <cstdint>

namespace Consolpp::Text::Grapheme
{
    // The Grapheme_Cluster_Break property of UAX #29.
    enum class Break : std::uint8_t
    {
        Other,
        CarriageReturn,
        LineFeed,
        Control,
        Extend,
        ZeroWidthJoiner,
        RegionalIndicator,
        Prepend,
        SpacingMark,
        LeadingJamo,
        VowelJamo,
        TrailingJamo,
        LeadingVowelSyllable,
        LeadingVowelTrailingSyllable,
    };
} // namespace Consolpp::Text::Grapheme
