#include "Consolpp/Markup/ValidateTags.hpp"

#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <array>
#include <stdexcept>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Style;
    using Consolpp::Markup::TagDefinition;
    using Consolpp::Markup::validateTags;

    TEST(ValidateTagsTest, AcceptsDistinctValidNames)
    {
        // Arrange
        static constexpr std::array<TagDefinition, 2> tags{
            {{.name = "deploy", .style = Style{}}, {.name = "deploy-step", .style = Style{}}},
        };

        // Act
        const auto validate = [] {
            validateTags(tags);
        };

        // Assert
        EXPECT_NO_THROW(validate());
    }

    TEST(ValidateTagsTest, ThrowsForAnInvalidName)
    {
        // Arrange
        static constexpr std::array<TagDefinition, 1> tags{{{.name = "Deploy", .style = Style{}}}};

        // Act
        const auto validate = [] {
            validateTags(tags);
        };

        // Assert
        EXPECT_THROW(validate(), std::invalid_argument);
    }

    TEST(ValidateTagsTest, ThrowsForStyle)
    {
        // Arrange
        static constexpr std::array<TagDefinition, 1> tags{{{.name = "style", .style = Style{}}}};

        // Act
        const auto validate = [] {
            validateTags(tags);
        };

        // Assert
        EXPECT_THROW(validate(), std::invalid_argument);
    }

    TEST(ValidateTagsTest, ThrowsForLink)
    {
        // Arrange
        static constexpr std::array<TagDefinition, 1> tags{{{.name = "link", .style = Style{}}}};

        // Act
        const auto validate = [] {
            validateTags(tags);
        };

        // Assert
        EXPECT_THROW(validate(), std::invalid_argument);
    }

    TEST(ValidateTagsTest, ThrowsForANameGivenTwice)
    {
        // Arrange
        static constexpr std::array<TagDefinition, 2> tags{
            {{.name = "info", .style = Style{}}, {.name = "info", .style = Style::info}},
        };

        // Act
        const auto validate = [] {
            validateTags(tags);
        };

        // Assert
        EXPECT_THROW(validate(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Markup
