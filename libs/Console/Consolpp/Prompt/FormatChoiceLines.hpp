#pragma once

#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/FormatTemplate.hpp"
#include "Consolpp/Prompt/Replacement.hpp"
#include "Consolpp/Prompt/Selection.hpp"

#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    // One markup line per choice, the highlighted one and the others by their templates, with the choices encoded.
    template <std::output_iterator<char> Output>
    Output formatChoiceLines(const ChoiceField& field, Output output)
    {
        std::string encoded;

        for (std::size_t index = 0; const std::string_view choice : field.getOptions().choices) {
            std::string_view checkbox;

            if (field.getOptions().selection == Selection::Multiple) {
                checkbox = field.isSelected(index) ? ChoiceField::getSelectedTemplate()
                                                   : ChoiceField::getUnselectedTemplate();
            }

            encoded.clear();
            Markup::encode(choice, std::back_inserter(encoded));

            if (index > 0) {
                *output = '\n';
                ++output;
            }

            output = Prompt::formatTemplate(index == field.getHighlighted() ? ChoiceField::getHighlightedTemplate()
                                                                            : ChoiceField::getChoiceTemplate(),
                                            {
                                                Replacement{.name = "checkbox", .value = checkbox},
                                                Replacement{.name = "choice", .value = encoded},
                                            },
                                            output);
            ++index;
        }

        return output;
    }
} // namespace Consolpp::Prompt
