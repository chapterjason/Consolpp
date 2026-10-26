#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

namespace Consolpp::Prompt
{
    // The answer of a text, choice or confirmation field.
    using FormAnswer = std::variant<std::string, std::vector<std::size_t>, bool>;
} // namespace Consolpp::Prompt
