#pragma once

#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/EncodeDefinitionJson.hpp"
#include "Consolpp/Formatting/EncodeOptionJson.hpp"
#include "Consolpp/GlobalOptions.hpp"

#include <algorithm>
#include <iterator>
#include <string_view>

namespace Consolpp::Formatting
{
    // The definition and, in their own section, the options every command takes.
    template <std::output_iterator<char> Output>
    Output formatDefinitionAsJson(const DefinitionView& definition, Output output)
    {
        output = std::ranges::copy(std::string_view{R"({"definition":)"}, output).out;
        output = Formatting::encodeDefinitionJson(definition, output);
        output = std::ranges::copy(std::string_view{R"(,"globalOptions":[)"}, output).out;
        for (bool first = true; const Option& option : globalOptions) {
            if (!first) {
                *output = ',';
                ++output;
            }

            output = Formatting::encodeOptionJson(option, output);
            first = false;
        }

        return std::ranges::copy(std::string_view{"]}"}, output).out;
    }
} // namespace Consolpp::Formatting
