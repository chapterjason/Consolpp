#pragma once

#include <cstdint>

namespace Consolpp::Parsing
{
    enum class ParseErrorKind : std::uint8_t
    {
        UnknownOption,
        MissingValue,
        UnexpectedValue,
        DuplicateOption,
        MissingArgument,
        TooManyArguments,
    };
} // namespace Consolpp::Parsing
