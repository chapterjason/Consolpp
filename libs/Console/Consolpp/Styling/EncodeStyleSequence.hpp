#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Styling/ColorLayer.hpp"
#include "Consolpp/Text/FormatDecimal.hpp"

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <span>
#include <string_view>
#include <utility>

namespace Consolpp::Styling
{
    // One SGR sequence with the codes of the colors and flags the style sets, nothing for a style without any.
    template <std::output_iterator<char> Output, std::invocable<const Markup::Color&, ColorLayer, Output> EncodeColor>
    constexpr Output encodeStyleSequence(
        const Markup::Style& style,
        const std::span<const std::pair<bool Markup::Style::*, std::uint32_t>> flagCodes,
        const EncodeColor& encodeColor,
        Output output)
    {
        bool first = true;
        const auto separate = [&output, &first] {
            if (first) {
                output = std::ranges::copy(std::string_view{"\x1b["}, output).out;
                first = false;

                return;
            }

            *output = ';';
            ++output;
        };

        if (style.foreground.has_value()) {
            separate();
            output = encodeColor(*style.foreground, ColorLayer::Foreground, output);
        }

        if (style.background.has_value()) {
            separate();
            output = encodeColor(*style.background, ColorLayer::Background, output);
        }

        for (const auto& [flag, code] : flagCodes) {
            if (style.*flag) {
                separate();
                output = Text::formatDecimal(code, output);
            }
        }

        if (!first) {
            *output = 'm';
            ++output;
        }

        return output;
    }
} // namespace Consolpp::Styling
