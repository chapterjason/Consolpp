#pragma once

#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/FormatSignatureAsText.hpp"
#include "Consolpp/Formatting/FormatSynopsis.hpp"
#include "Consolpp/Formatting/SynopsisForm.hpp"
#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Text/Splitter.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Formatting
{
    // Markup; the description and help are markup themselves, every other text is encoded.
    template <std::output_iterator<char> Output>
    Output formatDefinitionAsText(const DefinitionView& definition, Output output)
    {
        std::string synopsis;
        const auto copy = [&output](const std::string_view text) {
            output = std::ranges::copy(text, output).out;
        };
        const auto encode = [&output](const std::string_view text) {
            output = Markup::encode(text, output);
        };

        Formatting::formatSynopsis(definition, SynopsisForm::Short, std::back_inserter(synopsis));

        if (!definition.description.empty()) {
            copy("<comment>Description:</comment>\n  ");
            copy(definition.description);
            copy("\n\n");
        }

        copy("<comment>Usage:</comment>\n  ");
        encode(synopsis);

        for (const std::string_view alias : definition.aliases) {
            copy("\n  ");
            encode(alias);
        }

        for (const std::string_view usage : definition.usages) {
            copy("\n  ");
            encode(usage);
        }

        copy("\n\n");
        output = Formatting::formatSignatureAsText(definition.options, definition.arguments, output);
        copy("\n");

        if (!definition.help.empty() && definition.help != definition.description) {
            Text::Splitter lines{definition.help, '\n'};

            copy("\n<comment>Help:</comment>");

            while (lines.hasNext()) {
                copy("\n  ");
                copy(lines.takeNext());
            }

            copy("\n");
        }

        return output;
    }
} // namespace Consolpp::Formatting
