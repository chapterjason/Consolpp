#pragma once

#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/CollectListEntries.hpp"
#include "Consolpp/Formatting/EncodeDefinitionJson.hpp"
#include "Consolpp/Formatting/EncodeJsonString.hpp"
#include "Consolpp/Formatting/EncodeOptionJson.hpp"
#include "Consolpp/Formatting/ListEntry.hpp"
#include "Consolpp/GlobalOptions.hpp"

#include <algorithm>
#include <iterator>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace Consolpp::Formatting
{
    // The listed definitions and, in their own section, the options every command takes; hidden ones are left out.
    // Throws CommandError when a non-empty namespace holds no listed name.
    template <std::output_iterator<char> Output>
    Output formatDefinitionsAsJson(const std::span<const DefinitionView> definitions,
                                   const std::string_view namespaceName,
                                   Output output)
    {
        const std::vector<ListEntry> entries = Formatting::collectListEntries(definitions, namespaceName);
        std::optional<std::string_view> filter{};
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };

        if (!namespaceName.empty()) {
            filter = namespaceName;
        }

        copy(R"({"namespace":)");
        output = Formatting::encodeJsonString(filter, output);
        copy(R"(,"definitions":[)");

        for (bool first = true; const ListEntry& entry : entries) {
            if (entry.name != entry.command->name) {
                continue;
            }

            copy(first ? "" : ",");
            output = Formatting::encodeDefinitionJson(*entry.command, output);
            first = false;
        }

        copy(R"(],"globalOptions":[)");
        for (bool first = true; const Option& option : globalOptions) {
            if (!first) {
                copy(",");
            }

            output = Formatting::encodeOptionJson(option, output);
            first = false;
        }
        copy("]}");

        return output;
    }
} // namespace Consolpp::Formatting
