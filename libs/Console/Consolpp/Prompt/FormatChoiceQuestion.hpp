#pragma once

#include "Consolpp/Prompt/ChoiceField.hpp"

#include <string>

namespace Consolpp::Prompt
{
    // The markup line of the question with the default choices.
    std::string formatChoiceQuestion(const ChoiceField& field);
} // namespace Consolpp::Prompt
