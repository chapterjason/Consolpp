#include "Consolpp/Text/WrappedLine.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::WrappedLine;

    TEST(WrappedLineTest, DiffersByWidth)
    {
        // Arrange
        constexpr WrappedLine first{.start = 0, .end = 2, .width = 2};
        constexpr WrappedLine second{.start = 0, .end = 2, .width = 4};

        // Act
        constexpr bool equal = first == second;

        // Assert
        EXPECT_FALSE(equal);
    }
} // namespace Consolpp::Tests::Unit::Text
