#pragma once

#include "Consolpp/Prompt/Replacement.hpp"

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    template <std::output_iterator<char> Output>
    constexpr Output formatTemplate(const std::string_view markupTemplate,
                                    const std::initializer_list<Replacement> replacements,
                                    Output output)
    {
        std::size_t start = 0;

        while (true) {
            const std::size_t opening = markupTemplate.find('{', start);

            output = std::ranges::copy(markupTemplate.substr(start, opening - start), output).out;

            if (opening == std::string_view::npos) {
                return output;
            }

            const std::size_t closing = markupTemplate.find('}', opening);

            if (closing == std::string_view::npos) {
                throw std::invalid_argument{"The template has an unclosed placeholder"};
            }

            const std::string_view name = markupTemplate.substr(opening + 1, closing - opening - 1);
            const auto* const replacement = std::ranges::find(replacements, name, &Replacement::name);

            if (replacement == replacements.end()) {
                throw std::invalid_argument{
                    std::string{"The template has the unknown placeholder {"} + std::string{name} + "}"};
            }

            output = std::ranges::copy(replacement->value, output).out;
            start = closing + 1;
        }
    }
} // namespace Consolpp::Prompt
