#pragma once

#include "Consolpp/Markup/Encode.hpp"
#include "Consolpp/Prompt/FormatTemplate.hpp"
#include "Consolpp/Prompt/Replacement.hpp"

#include <iterator>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    // The label is markup.
    template <std::output_iterator<char> Output>
    Output formatQuestionLine(const std::string_view questionTemplate,
                              const std::string_view defaultTemplate,
                              const std::string_view label,
                              const std::string_view defaultText,
                              Output output)
    {
        std::string encoded;
        std::string defaultPart;

        if (!defaultText.empty()) {
            Markup::encode(defaultText, std::back_inserter(encoded));
            Prompt::formatTemplate(defaultTemplate,
                                   {Replacement{.name = "value", .value = encoded}},
                                   std::back_inserter(defaultPart));
        }

        return Prompt::formatTemplate(questionTemplate,
                                      {
                                          Replacement{.name = "label", .value = label},
                                          Replacement{.name = "default", .value = defaultPart},
                                      },
                                      output);
    }
} // namespace Consolpp::Prompt
