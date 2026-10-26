#pragma once

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace Consolpp::Markup
{
    // Replaces the style of the tag with the same name. Not thread-safe, like setTags(). Throws
    // std::invalid_argument when there is no tag with the name.
    inline void replaceTag(const TagDefinition& tag)
    {
        std::vector<TagDefinition>& tags = Markup::getTags();
        const auto found = std::ranges::find(tags, tag.name, &TagDefinition::name);

        if (found == tags.end()) {
            throw std::invalid_argument{"There is no tag with the name"};
        }

        found->style = tag.style;
    }
} // namespace Consolpp::Markup
