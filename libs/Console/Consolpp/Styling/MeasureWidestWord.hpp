#pragma once

#include "Consolpp/Styling/StripMarkup.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Styling
{
    // The display width of the widest word of the markup; the text receives its visible text. Throws like
    // parseMarkup() for invalid markup.
    inline std::size_t measureWidestWord(const std::string_view markup, std::string& text)
    {
        std::size_t width = 0;
        std::size_t start = 0;

        text.clear();
        Styling::stripMarkup(markup, std::back_inserter(text));

        while (start <= text.size()) {
            const std::size_t end = std::min(text.find_first_of(" \n", start), text.size());

            width = std::max(width, Text::measureDisplayWidth(std::string_view{text}.substr(start, end - start)));
            start = end + 1;
        }

        return width;
    }
} // namespace Consolpp::Styling
