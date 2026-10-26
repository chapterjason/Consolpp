#include "Consolpp/Styling/MarkupLine.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Styling::MarkupLine;

    TEST(MarkupLineTest, ComparesTheWidthToo)
    {
        // Arrange
        constexpr MarkupLine line{.start = 1, .length = 2, .width = 3};
        constexpr MarkupLine other{.start = 1, .length = 2, .width = 4};

        // Act
        constexpr bool equal = line == other;

        // Assert
        EXPECT_FALSE(equal);
    }
} // namespace Consolpp::Tests::Unit::Styling
