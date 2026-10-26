#include "Consolpp/Formatting/EncodeRoff.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::encodeRoff;

    TEST(EncodeRoffTest, EscapesBackslashesAndDashes)
    {
        // Arrange
        std::string encoded;

        // Act
        encodeRoff("a\\b-c", std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "a\\eb\\-c");
    }

    TEST(EncodeRoffTest, ProtectsLinesThatStartLikeARequest)
    {
        // Arrange
        std::string encoded;

        // Act
        encodeRoff(".TH\n'b", std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "\\&.TH\n\\&'b");
    }
} // namespace Consolpp::Tests::Unit::Formatting
