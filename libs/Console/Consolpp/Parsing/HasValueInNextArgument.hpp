#pragma once

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Token.hpp"

#include <span>

namespace Consolpp::Parsing
{
    bool hasValueInNextArgument(std::span<const Option> options, const Parsing::Token& token);
} // namespace Consolpp::Parsing
