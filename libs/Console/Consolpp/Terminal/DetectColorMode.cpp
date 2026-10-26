#include "Consolpp/Terminal/DetectColorMode.hpp"

#include "Consolpp/Environment/GetValue.hpp"
#include "Consolpp/Terminal/ColorMode.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <string_view>

namespace Consolpp::Terminal
{
    ColorMode detectColorMode()
    {
        constexpr std::array<const char*, 2> names{"COLORTERM", "TERM"};
        const auto toLower = [](const char character) {
            return character >= 'A' && character <= 'Z' ? static_cast<char>(character - 'A' + 'a') : character;
        };
        const auto contains = [&toLower](const std::string_view text, const std::string_view lowercaseNeedle) {
            return !std::ranges::search(text, lowercaseNeedle, std::ranges::equal_to{}, toLower).empty();
        };

        for (const char* name : names) {
            const std::optional<std::string_view> value = Environment::getValue(name);

            if (!value.has_value()) {
                continue;
            }

            if (contains(*value, "truecolor")) {
                return ColorMode::TrueColor;
            }

            if (contains(*value, "256color")) {
                return ColorMode::Colors256;
            }
        }

        return ColorMode::Colors16;
    }
} // namespace Consolpp::Terminal
