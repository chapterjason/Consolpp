#pragma once

#include "Consolpp/Text/Grapheme/Range.hpp"
#include "Consolpp/Text/MeasureGraphemeDisplayWidth.hpp"

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace Consolpp::Text
{
    // The text must be valid UTF-8 without escape sequences.
    constexpr std::size_t measureDisplayWidth(const std::string_view text)
    {
        const bool printableAscii = std::ranges::all_of(text, [](const char character) {
            return character >= ' ' && character <= '~';
        });
        std::size_t width = 0;

        if (printableAscii) {
            return text.size();
        }

        for (const std::string_view cluster : Text::Grapheme::Range{text}) {
            width += Text::measureGraphemeDisplayWidth(cluster);
        }

        return width;
    }
} // namespace Consolpp::Text
