#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/ColorKind.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string_view>

namespace Consolpp::Markup
{
    // The attribute value parseColor() reads back as the same color. Throws std::out_of_range for a basic or bright
    // color with an index above 7.
    template <std::output_iterator<char> Output>
    constexpr Output encodeColor(const Color& color, Output output)
    {
        constexpr std::array<std::string_view, 8> basicNames{
            "black",
            "red",
            "green",
            "yellow",
            "blue",
            "magenta",
            "cyan",
            "white",
        };
        constexpr std::array<std::string_view, 8> brightNames{
            "gray",
            "bright-red",
            "bright-green",
            "bright-yellow",
            "bright-blue",
            "bright-magenta",
            "bright-cyan",
            "bright-white",
        };
        constexpr std::string_view hexDigits = "0123456789abcdef";
        constexpr std::size_t hexBase = 16;
        const auto copyChannel = [&output, &hexDigits](const std::uint8_t channel) {
            *output = hexDigits.at(channel / hexBase);
            ++output;
            *output = hexDigits.at(channel % hexBase);
            ++output;
        };

        if (color.kind == ColorKind::Default) {
            return std::ranges::copy(std::string_view{"default"}, output).out;
        }

        if (color.kind == ColorKind::Basic) {
            return std::ranges::copy(basicNames.at(color.index), output).out;
        }

        if (color.kind == ColorKind::Bright) {
            return std::ranges::copy(brightNames.at(color.index), output).out;
        }

        *output = '#';
        ++output;
        copyChannel(color.redChannel);
        copyChannel(color.greenChannel);
        copyChannel(color.blueChannel);

        return output;
    }
} // namespace Consolpp::Markup
