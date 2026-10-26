#pragma once

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <algorithm>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace Consolpp::Markup
{
    // Not thread-safe, like setTags(). Throws std::invalid_argument when there is no tag with the name.
    inline void removeTag(const std::string_view name)
    {
        std::vector<TagDefinition>& tags = Markup::getTags();
        const auto found = std::ranges::find(tags, name, &TagDefinition::name);

        if (found == tags.end()) {
            throw std::invalid_argument{"There is no tag with the name"};
        }

        tags.erase(found);
    }
} // namespace Consolpp::Markup
