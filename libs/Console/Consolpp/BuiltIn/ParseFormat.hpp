#pragma once

#include "Consolpp/BuiltIn/Format.hpp"

#include <stdexcept>
#include <string_view>

namespace Consolpp::BuiltIn
{
    // Throws std::invalid_argument for a name other than txt, json and md.
    constexpr Format parseFormat(const std::string_view name)
    {
        if (name == "txt") {
            return Format::Text;
        }

        if (name == "json") {
            return Format::Json;
        }

        if (name == "md") {
            return Format::Markdown;
        }

        throw std::invalid_argument{"The format has to be txt, json or md"};
    }
} // namespace Consolpp::BuiltIn
