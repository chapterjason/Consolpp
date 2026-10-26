#pragma once

#include "Consolpp/Prompt/ConfirmationField.hpp"

#include <string>

namespace Consolpp::Prompt
{
    // The markup line of the question with the typed answer.
    std::string formatConfirmationField(const ConfirmationField& field);
} // namespace Consolpp::Prompt
