#pragma once

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"
#include "Consolpp/Markup/ValidateTags.hpp"

#include <span>

namespace Consolpp::Markup
{
    // Not thread-safe: the tags are set up before threads and output start. The names must outlive the tags.
    // Throws std::invalid_argument for tags validateTags() rejects.
    inline void setTags(const std::span<const TagDefinition> tags)
    {
        Markup::validateTags(tags);

        Markup::getTags().assign(tags.begin(), tags.end());
    }
} // namespace Consolpp::Markup
