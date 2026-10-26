#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Consolpp::Prompt
{
    inline void validateTemplate(const std::string_view markupTemplate,
                                 const std::initializer_list<std::string_view> allowedNames,
                                 const std::initializer_list<std::string_view> requiredNames = {})
    {
        for (const std::string_view required : requiredNames) {
            const std::string placeholder = "{" + std::string{required} + "}";
            const std::size_t first = markupTemplate.find(placeholder);

            if (first == std::string_view::npos
                || markupTemplate.find(placeholder, first + placeholder.size()) != std::string_view::npos) {
                throw std::invalid_argument{"The template needs " + placeholder + " exactly once"};
            }
        }

        for (std::size_t opening = markupTemplate.find('{'); opening != std::string_view::npos;
             opening = markupTemplate.find('{', opening + 1)) {
            const std::size_t closing = markupTemplate.find('}', opening);

            if (closing == std::string_view::npos) {
                throw std::invalid_argument{"The template has an unclosed placeholder"};
            }

            const std::string_view name = markupTemplate.substr(opening + 1, closing - opening - 1);

            if (std::ranges::find(allowedNames, name) == allowedNames.end()) {
                throw std::invalid_argument{"The template does not allow the placeholder {" + std::string{name} + "}"};
            }
        }
    }
} // namespace Consolpp::Prompt
