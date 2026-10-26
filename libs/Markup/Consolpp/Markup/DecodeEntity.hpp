#pragma once

#include "Consolpp/Markup/DecodeCharacterReference.hpp"
#include "Consolpp/Markup/DecodeError.hpp"
#include "Consolpp/Markup/DecodeErrorKind.hpp"
#include "Consolpp/Markup/Entity.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

namespace Consolpp::Markup
{
    // The text starts with the entity; the position is where it starts, so errors point to it.
    constexpr Entity decodeEntity(const std::string_view text, const std::size_t position)
    {
        constexpr std::array<std::pair<std::string_view, std::uint32_t>, 5> names{
            {
                {"lt", '<'},
                {"gt", '>'},
                {"amp", '&'},
                {"quot", '"'},
                {"apos", '\''},
            },
        };
        const std::size_t end = text.find(';');

        if (end == std::string_view::npos) {
            throw DecodeError{DecodeErrorKind::UnterminatedEntity, text.substr(0, 1), position};
        }

        const std::string_view entity = text.substr(0, end + 1);

        if (entity.starts_with("&#")) {
            return Entity{.codePoint = Markup::decodeCharacterReference(entity, position), .length = entity.size()};
        }

        const std::string_view name = entity.substr(1, entity.size() - 2);
        const auto* const named = std::ranges::find(names, name, &std::pair<std::string_view, std::uint32_t>::first);

        if (!entity.starts_with('&') || named == names.end()) {
            throw DecodeError{DecodeErrorKind::UnknownEntity, entity, position};
        }

        return Entity{.codePoint = named->second, .length = entity.size()};
    }
} // namespace Consolpp::Markup
