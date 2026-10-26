#pragma once

#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/CollectListEntries.hpp"
#include "Consolpp/Formatting/EncodeRoff.hpp"
#include "Consolpp/Formatting/FormatManPageCommand.hpp"
#include "Consolpp/Formatting/FormatManPageHeader.hpp"
#include "Consolpp/Formatting/FormatOptionAsManPage.hpp"
#include "Consolpp/Formatting/ListEntry.hpp"
#include "Consolpp/GlobalOptions.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Program.hpp"

#include <algorithm>
#include <iterator>
#include <span>
#include <string_view>
#include <vector>

namespace Consolpp::Formatting
{
    // The whole roff page of the program for man(1): every visible command in a subsection, then the global options.
    // Every text is escaped.
    template <std::output_iterator<char> Output>
    Output formatManPage(const Program& program, const std::span<const DefinitionView> definitions, Output output)
    {
        const std::vector<ListEntry> entries = Formatting::collectListEntries(definitions, std::string_view{});
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto roff = [&output](const std::string_view text) {
            output = Formatting::encodeRoff(text, output);
        };
        output = Formatting::formatManPageHeader(program.name, program, output);
        copy(".SH NAME\n");
        roff(program.name);
        copy("\n.SH SYNOPSIS\n\\fB");
        roff(program.name);
        copy("\\fR command [options] [arguments]\n.SH COMMANDS\n");

        for (const ListEntry& entry : entries) {
            const DefinitionView& command = *entry.command;

            if (entry.name != command.name) {
                continue;
            }

            output = Formatting::formatManPageCommand(program, command, output);
        }

        copy(".SH GLOBAL OPTIONS\n");

        for (const Option& option : globalOptions) {
            output = Formatting::formatOptionAsManPage(option, false, output);
        }

        return output;
    }
} // namespace Consolpp::Formatting
