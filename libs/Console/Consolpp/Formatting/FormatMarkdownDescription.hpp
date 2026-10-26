#pragma once

#include "Consolpp/Formatting/EncodeMarkdown.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"
#include "Consolpp/Text/Splitter.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // Ends a list item with ": " and the stripped and escaped description markup; its further lines are indented to
    // stay in the item.
    template <std::output_iterator<char> Output>
    Output formatMarkdownDescription(const std::string_view description, Output output)
    {
        constexpr std::string_view whitespace = " \t\r";
        std::string text;

        Styling::stripMarkup(description, std::back_inserter(text));

        Text::Splitter lines{text, '\n'};

        if (!text.empty()) {
            output = std::ranges::copy(std::string_view{": "}, output).out;
        }

        for (bool first = true; lines.hasNext(); first = false) {
            std::string_view line = lines.takeNext();

            line.remove_prefix(std::min(line.find_first_not_of(whitespace), line.size()));
            line = line.substr(0, line.find_last_not_of(whitespace) + 1);
            output = std::ranges::copy(std::string_view{first ? "" : "\n  "}, output).out;
            output = Formatting::encodeMarkdown(line, output);
        }

        *output = '\n';
        ++output;

        return output;
    }
} // namespace Consolpp::Formatting
