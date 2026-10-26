#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/CollectListEntries.hpp"
#include "Consolpp/Formatting/FormatSignatureAsText.hpp"
#include "Consolpp/Formatting/GetNamespace.hpp"
#include "Consolpp/Formatting/ListEntry.hpp"
#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <span>
#include <string_view>
#include <vector>

namespace Consolpp::Formatting
{
    // Markup; the descriptions are markup themselves, every other text is encoded. Hidden definitions are left out.
    // Throws CommandError when a non-empty namespace holds no listed name.
    template <std::output_iterator<char> Output>
    Output formatDefinitionsAsText(const std::span<const DefinitionView> definitions,
                                   const std::string_view namespaceName,
                                   Output output)
    {
        const std::vector<ListEntry> entries = Formatting::collectListEntries(definitions, namespaceName);
        std::size_t columns = 0;
        std::string_view currentNamespace{};
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto encode = [&output](const std::string_view text) {
            output = Markup::encode(text, output);
        };

        for (const ListEntry& entry : entries) {
            columns = std::max(columns, Text::measureDisplayWidth(entry.name));
        }

        columns += 2;
        copy("<comment>Usage:</comment>\n  command [options] [arguments]\n\n");
        output = Formatting::formatSignatureAsText(std::span<const Option>{}, std::span<const Argument>{}, output);
        copy("\n\n");

        if (namespaceName.empty()) {
            copy("<comment>Available commands:</comment>");
        } else {
            copy("<comment>Available commands for the \"");
            encode(namespaceName);
            copy("\" namespace:</comment>");
        }

        for (const ListEntry& entry : entries) {
            const std::string_view entryNamespace = Formatting::getNamespace(entry.name);

            if (namespaceName.empty() && !entryNamespace.empty() && entryNamespace != currentNamespace) {
                copy("\n <comment>");
                encode(entryNamespace);
                copy("</comment>");
            }

            currentNamespace = entryNamespace;
            copy("\n  <info>");
            encode(entry.name);
            copy("</info>");
            output = std::ranges::fill_n(output,
                                         static_cast<std::ptrdiff_t>(columns - Text::measureDisplayWidth(entry.name)),
                                         ' ');

            if (entry.name == entry.command->name && !entry.command->aliases.empty()) {
                copy("[");

                for (std::size_t index = 0; const std::string_view alias : entry.command->aliases) {
                    copy(index++ == 0 ? "" : "|");
                    encode(alias);
                }

                copy("] ");
            }

            copy(entry.command->description);
        }

        copy("\n");

        return output;
    }
} // namespace Consolpp::Formatting
