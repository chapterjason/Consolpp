#pragma once

#include "Consolpp/Text/ContainsCodePoint.hpp"
#include "Consolpp/Text/DefaultIgnorableRanges.hpp"
#include "Consolpp/Text/EastAsianWideRanges.hpp"
#include "Consolpp/Text/EmojiPresentationRanges.hpp"
#include "Consolpp/Text/ExtendedPictographicRanges.hpp"
#include "Consolpp/Text/FindProperty.hpp"
#include "Consolpp/Text/Grapheme/Break.hpp"
#include "Consolpp/Text/Grapheme/BreakRanges.hpp"
#include "Consolpp/Text/Utf8/CodePoint.hpp"
#include "Consolpp/Text/Utf8/Decode.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Consolpp::Text
{
    // Columns of one grapheme cluster: emoji presentation (UTS #51) takes two, otherwise the East Asian Width of
    // the base (UAX #11) decides, with Ambiguous as narrow; controls and default ignorable code points take none.
    constexpr std::size_t measureGraphemeDisplayWidth(const std::string_view cluster)
    {
        constexpr std::uint32_t textSelector = 0xFE0E;
        constexpr std::uint32_t emojiSelector = 0xFE0F;
        constexpr std::uint32_t zeroWidthJoiner = 0x200D;
        constexpr std::uint32_t firstModifier = 0x1F3FB;
        constexpr std::uint32_t lastModifier = 0x1F3FF;
        const Text::Utf8::CodePoint base = Text::Utf8::decode(cluster);
        const auto baseBreak = Text::findProperty<Text::Grapheme::Break>(Text::Grapheme::breakRanges,
                                                                         base.value,
                                                                         Text::Grapheme::Break::Other);
        bool textPresentation = false;
        bool emojiPresentation = Text::containsCodePoint(Text::emojiPresentationRanges, base.value);
        const bool pictographic = Text::containsCodePoint(Text::extendedPictographicRanges, base.value);
        const bool keycapBase = base.value == '#' || base.value == '*' || (base.value >= '0' && base.value <= '9');

        if (baseBreak == Text::Grapheme::Break::Control
            || baseBreak == Text::Grapheme::Break::CarriageReturn
            || baseBreak == Text::Grapheme::Break::LineFeed
            || Text::containsCodePoint(Text::defaultIgnorableRanges, base.value)) {
            return 0;
        }

        for (std::size_t index = base.length; index < cluster.size();) {
            const Text::Utf8::CodePoint next = Text::Utf8::decode(cluster.substr(index));
            const bool modifier = next.value >= firstModifier && next.value <= lastModifier;

            textPresentation = textPresentation || next.value == textSelector;
            emojiPresentation = emojiPresentation
                                || (next.value == emojiSelector && (pictographic || keycapBase))
                                || (pictographic && (modifier || next.value == zeroWidthJoiner));
            index += next.length;
        }

        if (emojiPresentation && !textPresentation) {
            return 2;
        }

        return Text::containsCodePoint(Text::eastAsianWideRanges, base.value) ? 2 : 1;
    }
} // namespace Consolpp::Text
