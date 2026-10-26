#pragma once

#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Prompt/CreateInputLineText.hpp"
#include "Consolpp/Prompt/Echo.hpp"
#include "Consolpp/Prompt/FormatTemplate.hpp"
#include "Consolpp/Prompt/InputLineText.hpp"
#include "Consolpp/Prompt/Replacement.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Text/Grapheme/FindEnd.hpp"
#include "Consolpp/Text/MeasureDisplayWidth.hpp"
#include "Consolpp/Text/ReplaceControlCharacters.hpp"

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    // The typed text as markup, split at the cursor; the text scrolls with the cursor within the columns, and one
    // column stays free for the cursor. Hidden text is never written.
    template <std::output_iterator<char> BeforeOutput, std::output_iterator<char> AfterOutput>
    void formatInputLine(const TextField& field,
                         const std::size_t columns,
                         BeforeOutput beforeCursor,
                         AfterOutput afterCursor)
    {
        const std::size_t available = columns > 0 ? columns - 1 : 0;
        const InputLineText inputLineText = Prompt::createInputLineText(field);
        const std::string_view text = inputLineText.text;
        const std::string suggestion = field.getOptions().echo == Echo::Visible
                                           ? Text::replaceControlCharacters(field.getSuggestion())
                                           : std::string{};
        std::size_t start = 0;

        if (available == 0) {
            return;
        }

        while (Text::measureDisplayWidth(text.substr(start, inputLineText.cursor - start)) > available) {
            start = Text::Grapheme::findEnd(text, start);
        }

        std::size_t end = start;

        while (end < text.size()
               && Text::measureDisplayWidth(text.substr(start, Text::Grapheme::findEnd(text, end) - start))
                      <= available) {
            end = Text::Grapheme::findEnd(text, end);
        }

        const std::size_t used = Text::measureDisplayWidth(text.substr(start, end - start));
        const bool suggested = !suggestion.empty()
                               && end == text.size()
                               && used + Text::measureDisplayWidth(suggestion) <= available;
        std::string encodedSuggestion;

        Markup::encode(text.substr(start, inputLineText.cursor - start), beforeCursor);
        afterCursor = Markup::encode(text.substr(inputLineText.cursor, end - inputLineText.cursor), afterCursor);

        if (suggested) {
            Markup::encode(suggestion, std::back_inserter(encodedSuggestion));
            Prompt::formatTemplate(TextField::getSuggestionTemplate(),
                                   {Replacement{.name = "suggestion", .value = encodedSuggestion}},
                                   afterCursor);
        }
    }
} // namespace Consolpp::Prompt
