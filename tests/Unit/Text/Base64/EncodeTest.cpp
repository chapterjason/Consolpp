#include "Consolpp/Text/Base64/Encode.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Base64
{
    using Consolpp::Text::Base64::encode;

    TEST(EncodeTest, EncodesWholeGroups)
    {
        // Arrange
        std::string encoded;

        // Act
        encode("foobar", std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "Zm9vYmFy");
    }

    TEST(EncodeTest, PadsAShortLastGroup)
    {
        // Arrange
        std::string one;
        std::string two;

        // Act
        encode("f", std::back_inserter(one));
        encode("fo", std::back_inserter(two));

        // Assert
        EXPECT_EQ(one, "Zg==");
        EXPECT_EQ(two, "Zm8=");
    }
} // namespace Consolpp::Tests::Unit::Text::Base64
