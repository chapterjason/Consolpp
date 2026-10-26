#pragma once

#include <algorithm>
#include <iterator>
#include <string_view>

namespace Consolpp::Styling
{
    // OSC 8; the href is written as it is.
    template <std::output_iterator<char> Output>
    constexpr Output encodeLinkStart(const std::string_view href, Output output)
    {
        output = std::ranges::copy(std::string_view{"\x1b]8;;"}, output).out;
        output = std::ranges::copy(href, output).out;

        return std::ranges::copy(std::string_view{"\x1b\\"}, output).out;
    }
} // namespace Consolpp::Styling
