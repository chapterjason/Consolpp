#include "Consolpp/Markup/RemoveTag.hpp"

#include "Consolpp/Markup/GetTags.hpp"
#include "Consolpp/Markup/HasTag.hpp"
#include "Consolpp/Markup/SetTags.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

#include <cstddef>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::getTags;
    using Consolpp::Markup::hasTag;
    using Consolpp::Markup::removeTag;
    using Consolpp::Markup::setTags;
    using Consolpp::Markup::TagDefinition;

    namespace
    {
        class RemoveTagTest : public ::testing::Test
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

    TEST_F(RemoveTagTest, RemovesTheTagWithTheName)
    {
        // Arrange
        const std::size_t count = getTags().size();

        // Act
        removeTag("info");

        // Assert
        EXPECT_FALSE(hasTag("info"));
        EXPECT_EQ(getTags().size(), count - 1);
    }

    TEST_F(RemoveTagTest, ThrowsWithoutATagWithTheName)
    {
        // Act
        const auto remove = [] {
            removeTag("deploy");
        };

        // Assert
        EXPECT_THROW(remove(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Markup
