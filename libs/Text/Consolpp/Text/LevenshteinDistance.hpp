#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Text
{
    template <std::size_t MaxLength>
    constexpr std::size_t levenshteinDistance(const std::string_view from, const std::string_view to)
    {
        if (to.size() > MaxLength) {
            throw std::length_error{"The compared text is longer than the maximum length"};
        }

        std::array<std::size_t, MaxLength + 1> row{};

        for (std::size_t column = 0; column <= to.size(); ++column) {
            row.at(column) = column;
        }

        for (const char fromCharacter : from) {
            std::size_t diagonal = row.front();

            row.front() += 1;

            for (std::size_t column = 1; column <= to.size(); ++column) {
                const std::size_t above = row.at(column);
                const std::size_t cost = fromCharacter == to.at(column - 1) ? 0 : 1;

                row.at(column) = std::min({above + 1, row.at(column - 1) + 1, diagonal + cost});
                diagonal = above;
            }
        }

        return row.at(to.size());
    }
} // namespace Consolpp::Text
