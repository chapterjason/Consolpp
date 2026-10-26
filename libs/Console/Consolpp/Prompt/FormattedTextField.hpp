#pragma once

#include <cstddef>
#include <string>

namespace Consolpp::Prompt
{
    // The markup lines of a text field and where its cursor stands.
    struct FormattedTextField
    {
        public:
            std::string question;
            std::string input;
            // In the input line, counted in columns from its start.
            std::size_t cursorColumn{0};
    };
} // namespace Consolpp::Prompt
