#pragma once

#include "Consolpp/Markup/Color.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace Consolpp::Markup
{
    constexpr Color parseColor(const std::string_view value)
    {
        constexpr std::array<std::pair<std::string_view, Color>, 17> names{
            {
                {"default", Color::defaultColor},
                {"black", Color::black},
                {"red", Color::red},
                {"green", Color::green},
                {"yellow", Color::yellow},
                {"blue", Color::blue},
                {"magenta", Color::magenta},
                {"cyan", Color::cyan},
                {"white", Color::white},
                {"gray", Color::gray},
                {"bright-red", Color::brightRed},
                {"bright-green", Color::brightGreen},
                {"bright-yellow", Color::brightYellow},
                {"bright-blue", Color::brightBlue},
                {"bright-magenta", Color::brightMagenta},
                {"bright-cyan", Color::brightCyan},
                {"bright-white", Color::brightWhite},
            },
        };
        const auto toLower = [](const char character) {
            return character >= 'A' && character <= 'Z' ? static_cast<char>(character - 'A' + 'a') : character;
        };

        if (value.starts_with('#')) {
            return Color::fromHex(value);
        }

        const auto* const found = std::ranges::find_if(names, [&value, &toLower](const auto& entry) {
            return std::ranges::equal(entry.first, value, {}, {}, toLower);
        });

        if (found == names.end()) {
            throw std::invalid_argument{"The color is unknown"};
        }

        return found->second;
    }
} // namespace Consolpp::Markup
