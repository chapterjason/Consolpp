#include "Consolpp/Environment/GetValue.hpp"

#include <cstdlib>
#include <optional>
#include <string_view>

namespace Consolpp::Environment
{
    std::optional<std::string_view> getValue(const char* name)
    {
        // Only racy against a concurrent setenv(), which Consolpp never calls.
        const char* value = std::getenv(name); // NOLINT(concurrency-mt-unsafe)

        if (value == nullptr) {
            return std::nullopt;
        }

        return std::string_view{value};
    }
} // namespace Consolpp::Environment
