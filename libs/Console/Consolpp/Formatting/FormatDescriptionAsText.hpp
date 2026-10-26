#pragma once

#include "Consolpp/Text/Splitter.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    // The description is markup; every line after the first is indented and trimmed, trailing whitespace is dropped.
    template <std::output_iterator<char> Output>
    constexpr Output formatDescriptionAsText(const std::string_view description,
                                             const std::size_t indent,
                                             Output output)
    {
        constexpr std::string_view whitespace = " \t\r";
        Text::Splitter lines{description, '\n'};
        bool first = true;

        while (lines.hasNext()) {
            std::string_view line = lines.takeNext();

            line = line.substr(0, line.find_last_not_of(whitespace) + 1);

            if (!first) {
                line.remove_prefix(std::min(line.find_first_not_of(whitespace), line.size()));
                *output = '\n';
                ++output;
                output = std::ranges::fill_n(output, static_cast<std::ptrdiff_t>(indent), ' ');
            }

            first = false;
            output = std::ranges::copy(line, output).out;
        }

        return output;
    }
} // namespace Consolpp::Formatting
