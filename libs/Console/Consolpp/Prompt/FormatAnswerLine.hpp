#pragma once

#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/Echo.hpp"
#include "Consolpp/Prompt/FormatTemplate.hpp"
#include "Consolpp/Prompt/Replacement.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Text/Grapheme/FindEnd.hpp"

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    // The label with the answer as it was shown: masked text as "*", hidden text not at all.
    template <std::output_iterator<char> Output>
    Output formatAnswerLine(const TextField& field, Output output)
    {
        const std::string_view value = field.getValue();
        std::string shown;

        if (field.getOptions().echo == Echo::Visible) {
            Markup::encode(value, std::back_inserter(shown));
        } else if (field.getOptions().echo == Echo::Masked) {
            for (std::size_t position = 0; position < value.size();
                 position = Text::Grapheme::findEnd(value, position)) {
                shown += '*';
            }
        }

        return Prompt::formatTemplate(TextField::getAnswerTemplate(),
                                      {
                                          Replacement{.name = "label", .value = field.getOptions().label},
                                          Replacement{.name = "answer", .value = shown},
                                      },
                                      output);
    }

    // The label with the chosen choices, separated by commas.
    template <std::output_iterator<char> Output>
    Output formatAnswerLine(const ChoiceField& field, Output output)
    {
        std::string chosen;

        for (const std::size_t index : field.getChosenIndexes()) {
            chosen += chosen.empty() ? "" : ", ";
            Markup::encode(field.getOptions().choices.subspan(index).front(), std::back_inserter(chosen));
        }

        return Prompt::formatTemplate(ChoiceField::getAnswerTemplate(),
                                      {
                                          Replacement{.name = "label", .value = field.getOptions().label},
                                          Replacement{.name = "answer", .value = chosen},
                                      },
                                      output);
    }

    // The label with yes or no.
    template <std::output_iterator<char> Output>
    Output formatAnswerLine(const ConfirmationField& field, Output output)
    {
        return Prompt::formatTemplate(ConfirmationField::getAnswerTemplate(),
                                      {
                                          Replacement{.name = "label", .value = field.getOptions().label},
                                          Replacement{
                                              .name = "answer",
                                              .value = field.getValue() ? ConfirmationField::getYesTemplate()
                                                                        : ConfirmationField::getNoTemplate(),
                                          },
                                      },
                                      output);
    }
} // namespace Consolpp::Prompt
