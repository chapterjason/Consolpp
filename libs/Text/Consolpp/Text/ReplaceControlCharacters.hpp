#pragma once

#include "Consolpp/Text/IsControl.hpp"
#include "Consolpp/Text/Utf8/CodePoint.hpp"
#include "Consolpp/Text/Utf8/Decode.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Consolpp::Text
{
    // Control characters and invalid UTF-8 become U+FFFD.
    inline std::string replaceControlCharacters(const std::string_view text)
    {
        constexpr std::string_view replacement{"\xEF\xBF\xBD"};
        std::string replaced;
        std::size_t position = 0;

        replaced.reserve(text.size());

        while (position < text.size()) {
            try {
                const Utf8::CodePoint point = Utf8::decode(text.substr(position));

                replaced += Text::isControl(point.value) ? replacement : text.substr(position, point.length);
                position += point.length;
            } catch (const std::invalid_argument&) {
                replaced += replacement;
                ++position;
            }
        }

        return replaced;
    }
} // namespace Consolpp::Text
