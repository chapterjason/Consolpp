#pragma once

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <vector>

namespace Consolpp::Markup
{
    // Starts with the ready-made tags: the prompt tags style the prompt fields. Change them only through setTags(),
    // addTag(), replaceTag() and removeTag(), which validate them.
    inline std::vector<TagDefinition>& getTags()
    {
        static std::vector<TagDefinition> tags{
            {.name = "info", .style = Style::info},
            {.name = "comment", .style = Style::comment},
            {.name = "question", .style = Style::question},
            {.name = "error", .style = Style::error},
            {.name = "warning", .style = Style::warning},
            {.name = "prompt-label", .style = Style::info},
            {.name = "prompt-default", .style = Style::comment},
            {.name = "prompt-suggestion", .style = Style{.foreground = Color::gray}},
            {.name = "prompt-highlight", .style = Style::info},
        };

        return tags;
    }
} // namespace Consolpp::Markup
