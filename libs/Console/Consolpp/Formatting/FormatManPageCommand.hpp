#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/EncodeRoff.hpp"
#include "Consolpp/Formatting/FormatArgumentAsManPage.hpp"
#include "Consolpp/Formatting/FormatOptionAsManPage.hpp"
#include "Consolpp/Formatting/FormatSynopsis.hpp"
#include "Consolpp/Formatting/SynopsisForm.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // The subsection of one command in the man page of the program; the help or description markup is stripped.
    template <std::output_iterator<char> Output>
    Output formatManPageCommand(const Program& program, const DefinitionView& command, Output output)
    {
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto roff = [&output](const std::string_view text) {
            output = Formatting::encodeRoff(text, output);
        };
        std::string synopsis;
        std::string description;
        std::string help;

        Formatting::formatSynopsis(command, SynopsisForm::Long, std::back_inserter(synopsis));
        Styling::stripMarkup(command.description, std::back_inserter(description));
        Styling::stripMarkup(command.help, std::back_inserter(help));
        copy(".SS ");
        roff(command.name);
        copy("\n\\fB");
        roff(program.name);
        copy(program.name.empty() ? "" : " ");
        roff(command.name);
        copy("\\fR");
        roff(std::string_view{synopsis}.substr(command.name.size()));
        copy("\n");

        if (!help.empty() || !description.empty()) {
            copy(".PP\n");
            roff(help.empty() ? description : help);
            copy("\n");
        }

        for (bool first = true; const std::string_view alias : command.aliases) {
            copy(first ? ".PP\nAliases: \\fB" : "\\fR, \\fB");
            roff(alias);
            first = false;
        }

        copy(command.aliases.empty() ? "" : "\\fR\n");

        for (const Argument& argument : command.arguments) {
            output = Formatting::formatArgumentAsManPage(argument, output);
        }

        for (const Option& option : command.options) {
            output = Formatting::formatOptionAsManPage(option, true, output);
        }

        copy(command.usages.empty() ? "" : ".PP\nExamples:\n.nf\n");

        for (const std::string_view usage : command.usages) {
            roff(usage);
            copy("\n");
        }

        copy(command.usages.empty() ? "" : ".fi\n");
        return output;
    }
} // namespace Consolpp::Formatting
