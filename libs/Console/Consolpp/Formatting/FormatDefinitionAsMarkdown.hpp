#pragma once

#include "Consolpp/Argument.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/EncodeMarkdown.hpp"
#include "Consolpp/Formatting/EncodeMarkdownCode.hpp"
#include "Consolpp/Formatting/FormatArgumentAsMarkdown.hpp"
#include "Consolpp/Formatting/FormatOptionAsMarkdown.hpp"
#include "Consolpp/Formatting/FormatSynopsis.hpp"
#include "Consolpp/Formatting/SynopsisForm.hpp"
#include "Consolpp/GlobalOptions.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Styling/StripMarkup.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // Every text of the definition is escaped, the description and help markup stripped first; names and values are
    // code spans.
    template <std::output_iterator<char> Output>
    Output formatDefinitionAsMarkdown(const DefinitionView& definition, Output output)
    {
        std::string synopsis;
        std::string description;
        std::string help;
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto code = [&output](const std::string_view text) {
            output = Formatting::encodeMarkdownCode(text, output);
        };

        Formatting::formatSynopsis(definition, SynopsisForm::Short, std::back_inserter(synopsis));
        Styling::stripMarkup(definition.description, std::back_inserter(description));
        Styling::stripMarkup(definition.help, std::back_inserter(help));
        copy("## ");
        code(definition.name);
        copy("\n");

        if (!description.empty()) {
            copy("\n");
            output = Formatting::encodeMarkdown(description, output);
            copy("\n");
        }

        copy("\n### Usage\n\n- ");
        code(synopsis);
        copy("\n");

        for (const std::string_view usage : definition.usages) {
            copy("- ");
            code(usage);
            copy("\n");
        }

        for (bool first = true; const std::string_view alias : definition.aliases) {
            copy(first ? "\nAliases: " : ", ");
            code(alias);
            first = false;
        }

        copy(definition.aliases.empty() ? "" : "\n");
        copy(definition.arguments.empty() ? "" : "\n### Arguments\n\n");

        for (const Argument& argument : definition.arguments) {
            output = Formatting::formatArgumentAsMarkdown(argument, output);
        }

        copy("\n### Options\n\n");
        for (const Option& option : globalOptions) {
            output = Formatting::formatOptionAsMarkdown(option, false, output);
        }

        for (const Option& option : definition.options) {
            output = Formatting::formatOptionAsMarkdown(option, true, output);
        }

        if (!help.empty() && help != description) {
            copy("\n### Help\n\n");
            output = Formatting::encodeMarkdown(help, output);
            copy("\n");
        }

        return output;
    }
} // namespace Consolpp::Formatting
