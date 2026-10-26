#include "Consolpp/Markup/HasTag.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::hasTag;

    TEST(HasTagTest, IsTrueForAStoredTag)
    {
        // Act
        const bool found = hasTag("info");

        // Assert
        EXPECT_TRUE(found);
    }

    TEST(HasTagTest, IsFalseForAnUnknownTag)
    {
        // Act
        const bool found = hasTag("deploy");

        // Assert
        EXPECT_FALSE(found);
    }
} // namespace Consolpp::Tests::Unit::Markup
