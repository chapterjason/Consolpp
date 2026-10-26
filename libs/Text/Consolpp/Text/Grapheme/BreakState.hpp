#pragma once

#include "Consolpp/Text/ContainsCodePoint.hpp"
#include "Consolpp/Text/ExtendedPictographicRanges.hpp"
#include "Consolpp/Text/FindProperty.hpp"
#include "Consolpp/Text/Grapheme/Break.hpp"
#include "Consolpp/Text/Grapheme/BreakRanges.hpp"
#include "Consolpp/Text/Grapheme/IndicConjunctBreak.hpp"
#include "Consolpp/Text/Grapheme/IndicConjunctBreakRanges.hpp"

#include <cstddef>
#include <cstdint>

namespace Consolpp::Text::Grapheme
{
    // What the extended grapheme cluster rules of UAX #29 need to know about the code points before a boundary.
    class BreakState
    {
        private:
            Break previous{Break::Other};
            std::size_t regionalIndicators{0};
            // Extended_Pictographic followed by Extend characters and at most one zero width joiner (GB11).
            bool pictographic{false};
            // An InCB Linker followed by InCB Extend characters (GB9c).
            bool linker{false};

            static constexpr Break getBreak(const std::uint32_t codePoint)
            {
                return Text::findProperty<Break>(breakRanges, codePoint, Break::Other);
            }

            static constexpr IndicConjunctBreak getConjunctBreak(const std::uint32_t codePoint)
            {
                return Text::findProperty<IndicConjunctBreak>(indicConjunctBreakRanges,
                                                              codePoint,
                                                              IndicConjunctBreak::None);
            }

            static constexpr bool isControl(const Break value)
            {
                return value == Break::Control || value == Break::CarriageReturn || value == Break::LineFeed;
            }

            // GB6 to GB8: Hangul syllable sequences.
            [[nodiscard]]
            constexpr bool joinsHangul(const Break next) const
            {
                switch (this->previous) {
                    case Break::LeadingJamo:
                        return next == Break::LeadingJamo
                               || next == Break::VowelJamo
                               || next == Break::LeadingVowelSyllable
                               || next == Break::LeadingVowelTrailingSyllable;
                    case Break::LeadingVowelSyllable:
                    case Break::VowelJamo:
                        return next == Break::VowelJamo || next == Break::TrailingJamo;
                    case Break::LeadingVowelTrailingSyllable:
                    case Break::TrailingJamo:
                        return next == Break::TrailingJamo;
                    default:
                        return false;
                }
            }

        public:
            explicit constexpr BreakState(const std::uint32_t first)
            {
                this->append(first);
            }

            // Whether the code point continues the cluster, by the rules GB3 to GB999.
            [[nodiscard]]
            constexpr bool joins(const std::uint32_t codePoint) const
            {
                const Break next = BreakState::getBreak(codePoint);

                if (this->previous == Break::CarriageReturn && next == Break::LineFeed) {
                    return true;
                }

                if (BreakState::isControl(this->previous) || BreakState::isControl(next)) {
                    return false;
                }

                if (this->joinsHangul(next)
                    || next == Break::Extend
                    || next == Break::ZeroWidthJoiner
                    || next == Break::SpacingMark
                    || this->previous == Break::Prepend) {
                    return true;
                }

                if (this->linker && BreakState::getConjunctBreak(codePoint) == IndicConjunctBreak::Consonant) {
                    return true;
                }

                if (this->previous == Break::ZeroWidthJoiner && this->pictographic) {
                    return Text::containsCodePoint(extendedPictographicRanges, codePoint);
                }

                return this->previous == Break::RegionalIndicator
                       && next == Break::RegionalIndicator
                       && this->regionalIndicators % 2 == 1;
            }

            constexpr void append(const std::uint32_t codePoint)
            {
                const Break next = BreakState::getBreak(codePoint);
                const IndicConjunctBreak conjunct = BreakState::getConjunctBreak(codePoint);
                const bool extendsPictograph = this->previous != Break::ZeroWidthJoiner
                                               && (next == Break::Extend || next == Break::ZeroWidthJoiner);

                this->pictographic = Text::containsCodePoint(extendedPictographicRanges, codePoint)
                                     || (this->pictographic && extendsPictograph);
                this->regionalIndicators = next == Break::RegionalIndicator ? this->regionalIndicators + 1 : 0;
                this->linker = conjunct == IndicConjunctBreak::Linker
                               || (this->linker && conjunct == IndicConjunctBreak::Extend);
                this->previous = next;
            }
    };
} // namespace Consolpp::Text::Grapheme
