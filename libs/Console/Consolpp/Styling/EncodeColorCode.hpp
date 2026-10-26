#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/ColorKind.hpp"
#include "Consolpp/Styling/ColorLayer.hpp"
#include "Consolpp/Styling/ReduceTo3BitColor.hpp"
#include "Consolpp/Styling/ReduceTo8BitColor.hpp"
#include "Consolpp/Terminal/ColorMode.hpp"
#include "Consolpp/Text/FormatDecimal.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string_view>

namespace Consolpp::Styling
{
    template <std::output_iterator<char> Output>
    constexpr Output encodeColorCode(const Markup::Color& color,
                                     const ColorLayer layer,
                                     const Terminal::ColorMode mode,
                                     Output output)
    {
        constexpr std::uint32_t basicForeground = 30;
        constexpr std::uint32_t basicBackground = 40;
        constexpr std::uint32_t brightForeground = 90;
        constexpr std::uint32_t brightBackground = 100;
        constexpr std::uint32_t defaultIndex = 9;
        const bool foreground = layer == ColorLayer::Foreground;
        const std::uint32_t basicBase = foreground ? basicForeground : basicBackground;

        switch (color.kind) {
            case Markup::ColorKind::Default:
                return Text::formatDecimal(basicBase + defaultIndex, output);
            case Markup::ColorKind::Basic:
                return Text::formatDecimal(basicBase + color.index, output);
            case Markup::ColorKind::Bright:
                return Text::formatDecimal((foreground ? brightForeground : brightBackground) + color.index, output);
            case Markup::ColorKind::Rgb:
                break;
        }

        if (mode == Terminal::ColorMode::Colors16) {
            return Text::formatDecimal(basicBase + reduceTo3BitColor(color), output);
        }

        output = std::ranges::copy(std::string_view{foreground ? "38;" : "48;"}, output).out;

        if (mode == Terminal::ColorMode::Colors256) {
            output = std::ranges::copy(std::string_view{"5;"}, output).out;

            return Text::formatDecimal(reduceTo8BitColor(color), output);
        }

        output = std::ranges::copy(std::string_view{"2;"}, output).out;
        output = Text::formatDecimal(color.redChannel, output);
        *output = ';';
        ++output;
        output = Text::formatDecimal(color.greenChannel, output);
        *output = ';';
        ++output;

        return Text::formatDecimal(color.blueChannel, output);
    }
} // namespace Consolpp::Styling
