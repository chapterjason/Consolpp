#pragma once

#include <algorithm>
#include <iterator>
#include <string_view>

namespace Consolpp::Styling
{
    // OSC 8.
    template <std::output_iterator<char> Output>
    constexpr Output encodeLinkEnd(Output output)
    {
        return std::ranges::copy(std::string_view{"\x1b]8;;\x1b\\"}, output).out;
    }
} // namespace Consolpp::Styling
