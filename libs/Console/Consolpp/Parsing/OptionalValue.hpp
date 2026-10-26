#pragma once

#include <optional>
#include <string_view>

namespace Consolpp::Parsing
{
    struct OptionalValue
    {
        public:
            bool given{false};
            std::optional<std::string_view> value{};

            constexpr bool operator==(const OptionalValue& other) const = default;
    };
} // namespace Consolpp::Parsing
