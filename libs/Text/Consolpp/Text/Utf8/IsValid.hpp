#pragma once

#include "Consolpp/Text/Utf8/CodePoint.hpp"
#include "Consolpp/Text/Utf8/Decode.hpp"

#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Text::Utf8
{
    constexpr bool isValid(const std::string_view text)
    {
        std::size_t index = 0;

        try {
            while (index < text.size()) {
                index += Text::Utf8::decode(text.substr(index)).length;
            }
        } catch (const std::invalid_argument&) {
            return false;
        }

        return true;
    }
} // namespace Consolpp::Text::Utf8
