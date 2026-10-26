#include "Consolpp/Terminal/GetEnvironmentSize.hpp"

#include "Consolpp/Environment/GetValue.hpp"

#include <charconv>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <system_error>

namespace Consolpp::Terminal
{
    std::size_t getEnvironmentSize(const char* const name, const std::size_t fallback)
    {
        const std::optional<std::string_view> value = Environment::getValue(name);

        if (!value.has_value() || value->empty()) {
            return fallback;
        }

        const char* const first = std::to_address(value->begin());
        const char* const last = std::to_address(value->end());
        std::size_t size = 0;
        const std::from_chars_result result = std::from_chars(first, last, size);

        return result.ec == std::errc{} && result.ptr == last && size > 0 ? size : fallback;
    }
} // namespace Consolpp::Terminal
