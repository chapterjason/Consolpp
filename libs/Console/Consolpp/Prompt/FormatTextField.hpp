#pragma once

#include "Consolpp/Prompt/FormattedTextField.hpp"
#include "Consolpp/Prompt/TextField.hpp"

#include <cstddef>

namespace Consolpp::Prompt
{
    // The input line scrolls with the cursor within the columns.
    FormattedTextField formatTextField(const TextField& field, std::size_t columns);
} // namespace Consolpp::Prompt
