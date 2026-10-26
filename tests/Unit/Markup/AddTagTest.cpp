#include "Consolpp/Markup/AddTag.hpp"

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/SetTags.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <cstddef>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::addTag;
    using Consolpp::Markup::getTags;
    using Consolpp::Markup::setTags;
    using Consolpp::Markup::Style;
    using Consolpp::Markup::TagDefinition;

    namespace
    {
        class AddTagTest : public ::testing::Test
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

    TEST_F(AddTagTest, AddsTheTagAfterTheOthers)
    {
        // Arrange
        constexpr TagDefinition tag{.name = "deploy", .style = Style{.bold = true}};
        const std::size_t count = getTags().size();

        // Act
        addTag(tag);

        // Assert
        ASSERT_EQ(getTags().size(), count + 1);
        EXPECT_EQ(getTags().back(), tag);
    }

    TEST_F(AddTagTest, ThrowsForATakenName)
    {
        // Arrange
        constexpr TagDefinition tag{.name = "info", .style = Style{.bold = true}};
        const std::vector<TagDefinition> before{getTags().begin(), getTags().end()};

        bool thrown = false;

        // Act
        try {
            addTag(tag);
        } catch (const std::invalid_argument&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
        EXPECT_EQ(getTags(), before);
    }

    TEST_F(AddTagTest, ThrowsForABuiltInName)
    {
        // Arrange
        constexpr TagDefinition tag{.name = "link", .style = Style{}};

        // Act
        const auto add = [&tag] {
            addTag(tag);
        };

        // Assert
        EXPECT_THROW(add(), std::invalid_argument);
    }

    TEST_F(AddTagTest, ThrowsForANameTheMarkupCannotHold)
    {
        // Arrange
        constexpr TagDefinition tag{.name = "Deploy", .style = Style{}};

        // Act
        const auto add = [&tag] {
            addTag(tag);
        };

        // Assert
        EXPECT_THROW(add(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Markup
