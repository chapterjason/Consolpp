#pragma once

#include "Consolpp/Text/PropertyRange.hpp"

#include <algorithm>
#include <cstdint>
#include <span>

namespace Consolpp::Text
{
    // The ranges are sorted and do not overlap.
    template <typename Value>
    constexpr Value findProperty(const std::span<const PropertyRange<Value>> ranges,
                                 const std::uint32_t codePoint,
                                 const Value fallback)
    {
        const auto found = std::ranges::partition_point(ranges, [codePoint](const PropertyRange<Value>& range) {
            return range.last < codePoint;
        });

        return found != ranges.end() && found->first <= codePoint ? found->value : fallback;
    }
} // namespace Consolpp::Text
