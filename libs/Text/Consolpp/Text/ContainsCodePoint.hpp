#pragma once

#include "Consolpp/Text/CodePointRange.hpp"

#include <algorithm>
#include <cstdint>
#include <span>

namespace Consolpp::Text
{
    // The ranges are sorted and do not overlap.
    constexpr bool containsCodePoint(const std::span<const CodePointRange> ranges, const std::uint32_t codePoint)
    {
        const auto found = std::ranges::partition_point(ranges, [codePoint](const CodePointRange& range) {
            return range.last < codePoint;
        });

        return found != ranges.end() && found->first <= codePoint;
    }
} // namespace Consolpp::Text
