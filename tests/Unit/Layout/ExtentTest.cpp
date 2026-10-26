#include "Consolpp/Layout/Extent.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Layout
{
    using Consolpp::Layout::Extent;

    TEST(ExtentTest, ComparesBothWidths)
    {
        // Arrange
        constexpr Extent extent{.minimum = 1, .natural = 2};
        constexpr Extent other{.minimum = 1, .natural = 3};

        // Act
        constexpr bool equal = extent == other;

        // Assert
        EXPECT_FALSE(equal);
    }
} // namespace Consolpp::Tests::Unit::Layout
