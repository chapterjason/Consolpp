#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace Consolpp::Tests::Unit::Text::Grapheme
{
    // One line of GraphemeBreakTest.txt: the text and the byte lengths of its clusters.
    struct BreakCase
    {
        public:
            std::string_view text;
            std::array<std::size_t, 4> lengths;
            std::size_t count;
    };
} // namespace Consolpp::Tests::Unit::Text::Grapheme
