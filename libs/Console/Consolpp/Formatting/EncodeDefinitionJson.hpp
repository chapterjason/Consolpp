#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/EncodeArgumentJson.hpp"
#include "Consolpp/Formatting/EncodeJsonArray.hpp"
#include "Consolpp/Formatting/EncodeJsonString.hpp"
#include "Consolpp/Formatting/EncodeOptionJson.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // One field per member of DefinitionView; the description and help markup is stripped.
    template <std::output_iterator<char> Output>
    Output encodeDefinitionJson(const DefinitionView& definition, Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        std::string description;
        std::string help;

        Styling::stripMarkup(definition.description, std::back_inserter(description));
        Styling::stripMarkup(definition.help, std::back_inserter(help));
        copy(R"({"name":)");
        output = Formatting::encodeJsonString(definition.name, output);
        copy(R"(,"description":)");
        output = Formatting::encodeJsonString(std::string_view{description}, output);
        copy(R"(,"help":)");
        output = Formatting::encodeJsonString(std::string_view{help}, output);
        copy(R"(,"aliases":)");
        output = Formatting::encodeJsonArray(definition.aliases, output);
        copy(R"(,"usages":)");
        output = Formatting::encodeJsonArray(definition.usages, output);
        copy(R"(,"options":[)");

        for (bool first = true; const Option& option : definition.options) {
            copy(first ? "" : ",");
            output = Formatting::encodeOptionJson(option, output);
            first = false;
        }

        copy(R"(],"arguments":[)");

        for (bool first = true; const Argument& argument : definition.arguments) {
            copy(first ? "" : ",");
            output = Formatting::encodeArgumentJson(argument, output);
            first = false;
        }

        copy(R"(],"hidden":)");
        copy(definition.hidden ? "true" : "false");
        copy("}");

        return output;
    }
} // namespace Consolpp::Formatting
