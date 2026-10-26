#pragma once

#include "Consolpp/Text/Grapheme/Range.hpp"
#include "Consolpp/Text/MeasureGraphemeDisplayWidth.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Screen
{
    constexpr std::size_t countRows(const std::string_view line, const std::size_t width)
    {
        constexpr std::size_t tabStop = 8;
        std::size_t rows = 1;
        std::size_t column = 0;

        if (width == 0) {
            throw std::invalid_argument{"The width must be greater than zero"};
        }

        for (const std::string_view cluster : Text::Grapheme::Range{line}) {
            if (cluster == "\t") {
                column = std::min(((column / tabStop) + 1) * tabStop, width);

                continue;
            }

            const std::size_t columns = Text::measureGraphemeDisplayWidth(cluster);

            if (column > 0 && column + columns > width) {
                ++rows;
                column = 0;
            }

            column += columns;
        }

        return rows;
    }
} // namespace Consolpp::Screen
