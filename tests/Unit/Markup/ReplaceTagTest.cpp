#include "Consolpp/Markup/ReplaceTag.hpp"

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/SetTags.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::getTags;
    using Consolpp::Markup::replaceTag;
    using Consolpp::Markup::setTags;
    using Consolpp::Markup::Style;
    using Consolpp::Markup::TagDefinition;

    namespace
    {
        class ReplaceTagTest : public ::testing::Test
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

    TEST_F(ReplaceTagTest, ReplacesTheStyleOfTheTagWithTheName)
    {
        // Arrange
        constexpr TagDefinition tag{.name = "info", .style = Style{.bold = true}};
        const std::size_t count = getTags().size();

        // Act
        replaceTag(tag);

        // Assert
        EXPECT_EQ(*std::ranges::find(getTags(), tag.name, &TagDefinition::name), tag);
        EXPECT_EQ(getTags().size(), count);
    }

    TEST_F(ReplaceTagTest, ThrowsWithoutATagWithTheName)
    {
        // Arrange
        constexpr TagDefinition tag{.name = "deploy", .style = Style{.bold = true}};

        // Act
        const auto replace = [&tag] {
            replaceTag(tag);
        };

        // Assert
        EXPECT_THROW(replace(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Markup
