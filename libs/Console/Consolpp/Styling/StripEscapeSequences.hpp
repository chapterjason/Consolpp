#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Styling
{
    // ECMA-48 control sequences and operating system commands.
    template <std::output_iterator<char> Output>
    constexpr Output stripEscapeSequences(const std::string_view text, Output output)
    {
        const auto isWithin = [](const char character, const char first, const char last) {
            return character >= first && character <= last;
        };
        // "ESC [", parameter bytes, intermediate bytes and one final byte.
        const auto measureControlSequence = [&isWithin](const std::string_view rest) -> std::size_t {
            std::size_t index = 2;

            if (!rest.starts_with("\x1b[")) {
                return 0;
            }

            while (index < rest.size() && isWithin(rest.at(index), '\x30', '\x3F')) {
                ++index;
            }

            while (index < rest.size() && isWithin(rest.at(index), '\x20', '\x2F')) {
                ++index;
            }

            return index < rest.size() && isWithin(rest.at(index), '\x40', '\x7E') ? index + 1 : 0;
        };
        // "ESC ]" up to BEL or the string terminator "ESC \".
        const auto measureOperatingSystemCommand = [](const std::string_view rest) -> std::size_t {
            if (!rest.starts_with("\x1b]")) {
                return 0;
            }

            const std::size_t end = rest.find_first_of("\x07\x1b", 2);

            if (end == std::string_view::npos) {
                return 0;
            }

            if (rest.at(end) == '\x07') {
                return end + 1;
            }

            return rest.substr(end).starts_with("\x1b\\") ? end + 2 : 0;
        };
        std::size_t index = 0;

        while (index < text.size()) {
            const std::size_t escape = text.find('\x1b', index);

            output = std::ranges::copy(text.substr(index, escape - index), output).out;

            if (escape == std::string_view::npos) {
                break;
            }

            const std::string_view rest = text.substr(escape);
            std::size_t length = std::max(measureControlSequence(rest), measureOperatingSystemCommand(rest));

            if (length == 0) {
                *output = '\x1b';
                ++output;
                length = 1;
            }

            index = escape + length;
        }

        return output;
    }
} // namespace Consolpp::Styling
