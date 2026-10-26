#include "Consolpp/Text/Base64/Decode.hpp"

#include <iterator>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Base64
{
    using Consolpp::Text::Base64::decode;

    TEST(DecodeTest, DecodesWithPadding)
    {
        // Arrange
        std::string decoded;

        // Act
        decode("Zm9vYg==", std::back_inserter(decoded));

        // Assert
        EXPECT_EQ(decoded, "foob");
    }

    TEST(DecodeTest, DecodesWithoutPadding)
    {
        // Arrange
        std::string decoded;

        // Act
        decode("Zm9vYmE", std::back_inserter(decoded));

        // Assert
        EXPECT_EQ(decoded, "fooba");
    }

    TEST(DecodeTest, ThrowsForAnInvalidCharacter)
    {
        // Arrange
        std::string decoded;

        // Act
        const auto decodeInvalid = [&decoded] {
            decode("Zm9*", std::back_inserter(decoded));
        };

        // Assert
        EXPECT_THROW(decodeInvalid(), std::invalid_argument);
    }

    TEST(DecodeTest, ThrowsForATruncatedGroup)
    {
        // Arrange
        std::string decoded;

        // Act
        const auto decodeTruncated = [&decoded] {
            decode("Zm9vY", std::back_inserter(decoded));
        };

        // Assert
        EXPECT_THROW(decodeTruncated(), std::invalid_argument);
    }

    TEST(DecodeTest, ThrowsForMisplacedPadding)
    {
        // Arrange
        std::string decoded;

        // Act
        const auto decodeMisplaced = [&decoded] {
            decode("Zg=", std::back_inserter(decoded));
        };

        // Assert
        EXPECT_THROW(decodeMisplaced(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Text::Base64
