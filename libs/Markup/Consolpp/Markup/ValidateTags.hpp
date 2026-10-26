#pragma once

#include "Consolpp/Markup/IsName.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <algorithm>
#include <span>
#include <stdexcept>

namespace Consolpp::Markup
{
    // Throws std::invalid_argument for a name the markup cannot hold, for "style" and "link", which are built in,
    // and for a name given twice.
    constexpr void validateTags(const std::span<const TagDefinition> tags)
    {
        for (auto tag = tags.begin(); tag != tags.end(); ++tag) {
            const std::span<const TagDefinition> earlier{tags.begin(), tag};

            if (!Markup::isName(tag->name)) {
                throw std::invalid_argument{"A tag name has to be lowercase letters with single hyphens between them"};
            }

            if (tag->name == "style" || tag->name == "link") {
                throw std::invalid_argument{"The tags style and link are built in"};
            }

            if (std::ranges::find(earlier, tag->name, &TagDefinition::name) != earlier.end()) {
                throw std::invalid_argument{"A tag name is given twice"};
            }
        }
    }
} // namespace Consolpp::Markup
