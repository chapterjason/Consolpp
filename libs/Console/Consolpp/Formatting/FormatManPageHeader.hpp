#pragma once

#include "Consolpp/Formatting/EncodeRoff.hpp"
#include "Consolpp/Program.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // The title line of a page in section 1, without a date so that the page is the same on every build.
    template <std::output_iterator<char> Output>
    Output formatManPageHeader(const std::string_view page, const Program& program, Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        std::string title;

        for (const char character : page) {
            title += character >= 'a' && character <= 'z' ? static_cast<char>(character - 'a' + 'A') : character;
        }

        copy(".TH \"");
        output = Formatting::encodeRoff(title, output);
        copy(R"(" "1" "" ")");
        output = Formatting::encodeRoff(program.name, output);
        copy(program.name.empty() || program.version.empty() ? "" : " ");
        output = Formatting::encodeRoff(program.version, output);
        copy("\" \"User Commands\"\n");

        return output;
    }
} // namespace Consolpp::Formatting
