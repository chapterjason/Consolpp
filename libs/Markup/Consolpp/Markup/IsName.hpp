#pragma once

#include <string_view>

namespace Consolpp::Markup
{
    // Lowercase letters with single hyphens between them, the names of elements and attributes.
    constexpr bool isName(const std::string_view name)
    {
        const bool lettersAndHyphens = !name.empty()
                                       && name.find_first_not_of("abcdefghijklmnopqrstuvwxyz-")
                                              == std::string_view::npos;

        return lettersAndHyphens
               && !name.starts_with('-')
               && !name.ends_with('-')
               && name.find("--") == std::string_view::npos;
    }
} // namespace Consolpp::Markup
