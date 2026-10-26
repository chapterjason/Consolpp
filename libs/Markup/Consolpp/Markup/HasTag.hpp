#pragma once

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <algorithm>
#include <string_view>
#include <vector>

namespace Consolpp::Markup
{
    inline bool hasTag(const std::string_view name)
    {
        const std::vector<TagDefinition>& tags = Markup::getTags();

        return std::ranges::find(tags, name, &TagDefinition::name) != tags.end();
    }
} // namespace Consolpp::Markup
