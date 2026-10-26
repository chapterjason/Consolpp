#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Styling/ColorLayer.hpp"
#include "Consolpp/Styling/EncodeColorCode.hpp"
#include "Consolpp/Styling/EncodeStyleSequence.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <cstdint>
#include <iterator>
#include <utility>

namespace Consolpp::Styling
{
    template <std::output_iterator<char> Output>
    constexpr Output encodeStyleStart(const Markup::Style& style,
                                      const Terminal::Capabilities& capabilities,
                                      Output output)
    {
        constexpr std::array<std::pair<bool Markup::Style::*, std::uint32_t>, 5> options{
            {
                {&Markup::Style::bold, 1},
                {&Markup::Style::underline, 4},
                {&Markup::Style::blink, 5},
                {&Markup::Style::reverse, 7},
                {&Markup::Style::conceal, 8},
            },
        };

        return Styling::encodeStyleSequence(
            style,
            options,
            [&capabilities](const Markup::Color& color, const ColorLayer layer, Output colorOutput) {
                return encodeColorCode(color, layer, capabilities.colorMode, colorOutput);
            },
            output);
    }
} // namespace Consolpp::Styling
