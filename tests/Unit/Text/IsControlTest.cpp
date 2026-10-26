#include "Consolpp/Text/IsControl.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::isControl;

    TEST(IsControlTest, CountsC0DeleteAndC1)
    {
        // Act
        const bool c0 = isControl(U'\x1b');
        const bool deleteCharacter = isControl(U'\x7f');
        const bool c1 = isControl(U'\u0085');

        // Assert
        EXPECT_TRUE(c0);
        EXPECT_TRUE(deleteCharacter);
        EXPECT_TRUE(c1);
    }

    TEST(IsControlTest, LeavesOutPrintableCharacters)
    {
        // Act
        const bool space = isControl(U' ');
        const bool noBreakSpace = isControl(U'\u00A0');

        // Assert
        EXPECT_FALSE(space);
        EXPECT_FALSE(noBreakSpace);
    }
} // namespace Consolpp::Tests::Unit::Text
