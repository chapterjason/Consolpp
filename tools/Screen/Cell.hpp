#pragma once

#include <cstddef>
#include <string>

namespace Consolpp::Tools::Screen
{
    struct Cell
    {
        public:
            std::string text;
            // The right half of a wide character is a cell of width 0 without text.
            std::size_t width{1};

            bool operator==(const Cell& other) const = default;
    };
} // namespace Consolpp::Tools::Screen
