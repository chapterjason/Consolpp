#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    // A code span with one backtick more than the longest run inside, as CommonMark requires.
    template <std::output_iterator<char> Output>
    constexpr Output encodeMarkdownCode(const std::string_view text, Output output)
    {
        std::size_t longest = 0;
        std::size_t run = 0;

        for (const char character : text) {
            run = character == '`' ? run + 1 : 0;
            longest = std::max(longest, run);
        }

        const auto fence = static_cast<std::ptrdiff_t>(longest + 1);
        const bool padded = text.starts_with('`') || text.ends_with('`');

        output = std::ranges::fill_n(output, fence, '`');
        output = std::ranges::copy(std::string_view{padded ? " " : ""}, output).out;
        output = std::ranges::copy(text, output).out;
        output = std::ranges::copy(std::string_view{padded ? " " : ""}, output).out;

        return std::ranges::fill_n(output, fence, '`');
    }
} // namespace Consolpp::Formatting
