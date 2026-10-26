#pragma once

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/HasTag.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"
#include "Consolpp/Markup/ValidateTags.hpp"

#include <span>
#include <stdexcept>

namespace Consolpp::Markup
{
    // Not thread-safe, like setTags(). Throws std::invalid_argument for a tag validateTags() rejects and for a name
    // that is already taken.
    inline void addTag(const TagDefinition& tag)
    {
        Markup::validateTags(std::span<const TagDefinition>{&tag, 1});

        if (Markup::hasTag(tag.name)) {
            throw std::invalid_argument{"The tag name is already taken"};
        }

        Markup::getTags().push_back(tag);
    }
} // namespace Consolpp::Markup
