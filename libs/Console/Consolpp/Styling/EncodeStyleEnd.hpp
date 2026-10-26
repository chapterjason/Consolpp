#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Styling/ColorLayer.hpp"
#include "Consolpp/Styling/EncodeStyleSequence.hpp"
#include "Consolpp/Text/FormatDecimal.hpp"

#include <array>
#include <cstdint>
#include <iterator>
#include <utility>

namespace Consolpp::Styling
{
    template <std::output_iterator<char> Output>
    constexpr Output encodeStyleEnd(const Markup::Style& style, Output output)
    {
        constexpr std::array<std::pair<bool Markup::Style::*, std::uint32_t>, 5> options{
            {
                {&Markup::Style::bold, 22},
                {&Markup::Style::underline, 24},
                {&Markup::Style::blink, 25},
                {&Markup::Style::reverse, 27},
                {&Markup::Style::conceal, 28},
            },
        };

        return Styling::encodeStyleSequence(
            style,
            options,
            [](const Markup::Color& /*color*/, const ColorLayer layer, Output colorOutput) {
                constexpr std::uint32_t defaultForeground = 39;
                constexpr std::uint32_t defaultBackground = 49;

                return Text::formatDecimal(layer == ColorLayer::Foreground ? defaultForeground : defaultBackground,
                                           colorOutput);
            },
            output);
    }
} // namespace Consolpp::Styling
