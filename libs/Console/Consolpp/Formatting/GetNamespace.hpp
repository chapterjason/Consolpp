#pragma once

#include <cstddef>
#include <string_view>

namespace Consolpp::Formatting
{
    constexpr std::string_view getNamespace(const std::string_view name)
    {
        const std::size_t colon = name.find(':');

        return colon == std::string_view::npos ? std::string_view{} : name.substr(0, colon);
    }
} // namespace Consolpp::Formatting
