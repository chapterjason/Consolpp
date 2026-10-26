#include "Consolpp/Styling/EncodeLinkStart.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Styling::encodeLinkStart;

    TEST(EncodeLinkStartTest, OpensAnOperatingSystemCommandLink)
    {
        // Arrange
        std::string written;

        // Act
        encodeLinkStart("https://example.com", std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b]8;;https://example.com\x1b\\");
    }
} // namespace Consolpp::Tests::Unit::Styling
