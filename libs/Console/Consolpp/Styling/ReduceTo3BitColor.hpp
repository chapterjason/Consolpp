#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/ColorKind.hpp"

#include <cstdint>
#include <stdexcept>

namespace Consolpp::Styling
{
    constexpr std::uint8_t reduceTo3BitColor(const Markup::Color& color)
    {
        constexpr std::uint8_t halfChannel = 128;
        const auto isOn = [](const std::uint8_t channel) {
            return channel >= halfChannel ? 1U : 0U;
        };

        if (color.kind != Markup::ColorKind::Rgb) {
            throw std::invalid_argument{"Only an RGB color can be reduced"};
        }

        return static_cast<std::uint8_t>(
            (isOn(color.blueChannel) << 2U) | (isOn(color.greenChannel) << 1U) | isOn(color.redChannel));
    }
} // namespace Consolpp::Styling
