#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/ColorKind.hpp"

#include <cstdint>
#include <stdexcept>

namespace Consolpp::Styling
{
    constexpr std::uint8_t reduceTo8BitColor(const Markup::Color& color)
    {
        constexpr std::uint32_t cubeStart = 16;
        constexpr std::uint32_t cubeLevels = 5;
        constexpr std::uint32_t cubeRowSize = 6;
        constexpr std::uint32_t cubeBlack = 16;
        constexpr std::uint32_t cubeWhite = 231;
        constexpr std::uint32_t grayRampStart = 232;
        constexpr std::uint32_t grayRampSteps = 24;
        constexpr std::uint32_t darkestGray = 8;
        constexpr std::uint32_t lightestGray = 248;
        constexpr std::uint32_t grayRange = 247;
        constexpr std::uint32_t channelMaximum = 255;
        const auto roundedRatio = [](const std::uint32_t numerator, const std::uint32_t denominator) {
            return ((2 * numerator) + denominator) / (2 * denominator);
        };
        const auto level = [&roundedRatio](const std::uint32_t channel) {
            return roundedRatio(channel * cubeLevels, channelMaximum);
        };

        if (color.kind != Markup::ColorKind::Rgb) {
            throw std::invalid_argument{"Only an RGB color can be reduced"};
        }

        const std::uint32_t red = color.redChannel;
        const std::uint32_t green = color.greenChannel;
        const std::uint32_t blue = color.blueChannel;

        if (red == green && green == blue) {
            if (red < darkestGray) {
                return cubeBlack;
            }

            if (red > lightestGray) {
                return cubeWhite;
            }

            return static_cast<std::uint8_t>(grayRampStart
                                             + roundedRatio((red - darkestGray) * grayRampSteps, grayRange));
        }

        return static_cast<std::uint8_t>(
            cubeStart + (cubeRowSize * cubeRowSize * level(red)) + (cubeRowSize * level(green)) + level(blue));
    }
} // namespace Consolpp::Styling
