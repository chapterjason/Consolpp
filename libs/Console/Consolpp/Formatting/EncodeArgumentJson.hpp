#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/Formatting/EncodeJsonArray.hpp"
#include "Consolpp/Formatting/EncodeJsonString.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // One field per member of Argument; the description markup is stripped.
    template <std::output_iterator<char> Output>
    Output encodeArgumentJson(const Argument& argument, Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        std::string description;

        Styling::stripMarkup(argument.description, std::back_inserter(description));
        copy(R"({"name":)");
        output = Formatting::encodeJsonString(argument.name, output);
        copy(R"(,"required":)");
        copy(argument.required ? "true" : "false");
        copy(R"(,"variadic":)");
        copy(argument.variadic ? "true" : "false");
        copy(R"(,"description":)");
        output = Formatting::encodeJsonString(std::string_view{description}, output);
        copy(R"(,"defaultValue":)");
        output = Formatting::encodeJsonString(argument.defaultValue, output);
        copy(R"(,"defaultValues":)");
        output = Formatting::encodeJsonArray(argument.defaultValues, output);
        copy("}");

        return output;
    }
} // namespace Consolpp::Formatting
