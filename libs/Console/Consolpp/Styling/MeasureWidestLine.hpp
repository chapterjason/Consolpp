#pragma once

#include "Consolpp/Styling/StripMarkup.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"
#include "Consolpp/Text/Splitter.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Styling
{
    // The display width of the widest line of the markup; the text receives its visible text. Throws like
    // parseMarkup() for invalid markup.
    inline std::size_t measureWidestLine(const std::string_view markup, std::string& text)
    {
        std::size_t width = 0;

        text.clear();
        Styling::stripMarkup(markup, std::back_inserter(text));

        for (Text::Splitter lines{text, '\n'}; lines.hasNext();) {
            width = std::max(width, Text::measureDisplayWidth(lines.takeNext()));
        }

        return width;
    }
} // namespace Consolpp::Styling
