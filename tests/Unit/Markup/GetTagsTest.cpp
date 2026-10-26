#include "Consolpp/Markup/GetTags.hpp"

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"
#include "Consolpp/Markup/ValidateTags.hpp"

#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::getTags;
    using Consolpp::Markup::Style;
    using Consolpp::Markup::TagDefinition;
    using Consolpp::Markup::validateTags;

    TEST(GetTagsTest, StartsWithTheGeneralAndThePromptTags)
    {
        // Arrange
        const std::vector<TagDefinition> expected{
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

        // Act
        const std::vector<TagDefinition>& tags = getTags();

        // Assert
        EXPECT_EQ(tags, expected);
    }

    TEST(GetTagsTest, StartsWithValidTags)
    {
        // Act
        const auto validate = [] {
            validateTags(getTags());
        };

        // Assert
        EXPECT_NO_THROW(validate());
    }
} // namespace Consolpp::Tests::Unit::Markup
