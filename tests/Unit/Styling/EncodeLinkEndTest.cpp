#include "Consolpp/Styling/EncodeLinkEnd.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Styling::encodeLinkEnd;

    TEST(EncodeLinkEndTest, ClosesTheLink)
    {
        // Arrange
        std::string written;

        // Act
        encodeLinkEnd(std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b]8;;\x1b\\");
    }
} // namespace Consolpp::Tests::Unit::Styling
