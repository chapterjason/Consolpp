#include "Consolpp/Markup/SetTags.hpp"

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::getTags;
    using Consolpp::Markup::setTags;
    using Consolpp::Markup::Style;
    using Consolpp::Markup::TagDefinition;

    namespace
    {
        class SetTagsTest : public ::testing::Test
        {
            private:
                std::vector<TagDefinition> tags{getTags().begin(), getTags().end()};

            protected:
                void TearDown() override
                {
                    setTags(this->tags);
                }
        };
    } // namespace

    TEST_F(SetTagsTest, ReplacesAllTags)
    {
        // Arrange
        constexpr std::array<TagDefinition, 2> tags{
            {
                {.name = "deploy", .style = Style{.bold = true}},
                {.name = "skip", .style = Style{.underline = true}},
            },
        };

        // Act
        setTags(tags);

        // Assert
        EXPECT_TRUE(std::ranges::equal(getTags(), tags));
    }

    TEST_F(SetTagsTest, ThrowsForInvalidTags)
    {
        // Arrange
        constexpr std::array<TagDefinition, 1> tags{{{.name = "style", .style = Style{}}}};

        // Act
        const auto set = [&tags] {
            setTags(tags);
        };

        // Assert
        EXPECT_THROW(set(), std::invalid_argument);
    }

    TEST_F(SetTagsTest, KeepsTheTagsWhenItThrows)
    {
        // Arrange
        constexpr std::array<TagDefinition, 2> tags{
            {
                {.name = "deploy", .style = Style{}},
                {.name = "deploy", .style = Style{}},
            },
        };
        const std::vector<TagDefinition> before{getTags().begin(), getTags().end()};

        bool thrown = false;

        // Act
        try {
            setTags(tags);
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
        EXPECT_EQ(getTags(), before);
    }
} // namespace Consolpp::Tests::Unit::Markup
