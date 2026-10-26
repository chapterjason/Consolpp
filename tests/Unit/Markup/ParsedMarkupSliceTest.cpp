#include "Consolpp/Markup/ParsedMarkupSlice.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::ParsedMarkupSlice;

    TEST(ParsedMarkupSliceTest, KeepsItsRange)
    {
        // Act
        constexpr ParsedMarkupSlice<int> slice{.markup = nullptr, .start = 2, .end = 5};

        // Assert
        EXPECT_EQ(slice.end - slice.start, 3U);
    }
} // namespace Consolpp::Tests::Unit::Markup
