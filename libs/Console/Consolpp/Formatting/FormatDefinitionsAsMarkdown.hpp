#pragma once

#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/CollectListEntries.hpp"
#include "Consolpp/Formatting/EncodeMarkdown.hpp"
#include "Consolpp/Formatting/EncodeMarkdownCode.hpp"
#include "Consolpp/Formatting/GetNamespace.hpp"
#include "Consolpp/Formatting/ListEntry.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"

#include <algorithm>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Formatting
{
    // One list item per visible definition with its stripped description markup, the ones of each namespace under its
    // own heading; hidden ones are left out. Throws CommandError when a non-empty namespace holds no listed name.
    template <std::output_iterator<char> Output>
    Output formatDefinitionsAsMarkdown(const std::span<const DefinitionView> definitions,
                                       const std::string_view namespaceName,
                                       Output output)
    {
        const std::vector<ListEntry> entries = Formatting::collectListEntries(definitions, namespaceName);
        std::string_view currentNamespace{};
        std::string description;
        bool first = true;
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto code = [&output](const std::string_view text) {
            output = Formatting::encodeMarkdownCode(text, output);
        };

        for (const ListEntry& entry : entries) {
            const std::string_view entryNamespace = Formatting::getNamespace(entry.name);

            if (entry.name != entry.command->name) {
                continue;
            }

            description.clear();
            Styling::stripMarkup(entry.command->description, std::back_inserter(description));

            if (namespaceName.empty() && !entryNamespace.empty() && entryNamespace != currentNamespace) {
                copy(first ? "### " : "\n### ");
                output = Formatting::encodeMarkdown(entryNamespace, output);
                copy("\n\n");
            }

            currentNamespace = entryNamespace;
            copy("- ");
            code(entry.name);

            if (!entry.command->aliases.empty()) {
                copy(entry.command->aliases.size() == 1 ? " (alias " : " (aliases ");
            }

            for (bool firstAlias = true; const std::string_view alias : entry.command->aliases) {
                copy(firstAlias ? "" : ", ");
                code(alias);
                firstAlias = false;
            }

            copy(entry.command->aliases.empty() ? "" : ")");
            copy(description.empty() ? "" : ": ");
            output = Formatting::encodeMarkdown(description, output);
            copy("\n");
            first = false;
        }

        return output;
    }
} // namespace Consolpp::Formatting
