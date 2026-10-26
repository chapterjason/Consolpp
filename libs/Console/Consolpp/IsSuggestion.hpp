#pragma once

#include "Consolpp/Text/LevenshteinDistance.hpp"
#include "Consolpp/Text/Splitter.hpp"

#include <cstddef>
#include <string_view>

namespace Consolpp
{
    template <std::size_t MaxLength>
    constexpr bool isSuggestion(const std::string_view name, const std::string_view candidate)
    {
        const auto isSimilar = [](const std::string_view typed, const std::string_view existing) {
            return !typed.empty()
                   && (Text::levenshteinDistance<MaxLength>(typed, existing) * 3 <= typed.size()
                       || existing.find(typed) != std::string_view::npos);
        };
        Text::Splitter nameParts{name, ':'};
        Text::Splitter candidateParts{candidate, ':'};
        std::size_t matching = 0;
        std::size_t differing = 0;

        if (isSimilar(name, candidate)) {
            return true;
        }

        while (nameParts.hasNext()) {
            const std::string_view namePart = nameParts.takeNext();

            if (candidateParts.hasNext() && isSimilar(namePart, candidateParts.takeNext())) {
                ++matching;
            } else {
                ++differing;
            }
        }

        return matching > 0 && differing <= 1;
    }
} // namespace Consolpp
