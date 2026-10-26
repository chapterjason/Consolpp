#pragma once

#include "Consolpp/Markup/DecodeEntity.hpp"
#include "Consolpp/Markup/Entity.hpp"
#include "Consolpp/Text/Utf8/Encode.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Markup
{
    // Throws DecodeError for an unknown entity or an invalid character reference.
    template <std::output_iterator<char> Output>
    constexpr Output decode(const std::string_view text, Output output)
    {
        std::size_t index = 0;

        while (true) {
            const std::size_t ampersand = text.find('&', index);

            output = std::ranges::copy(text.substr(index, ampersand - index), output).out;

            if (ampersand == std::string_view::npos) {
                return output;
            }

            const Markup::Entity entity = Markup::decodeEntity(text.substr(ampersand), ampersand);

            output = Text::Utf8::encode(entity.codePoint, output);
            index = ampersand + entity.length;
        }
    }
} // namespace Consolpp::Markup
