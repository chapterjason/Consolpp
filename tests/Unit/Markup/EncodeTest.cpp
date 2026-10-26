#include "Consolpp/Markup/Encode.hpp"

#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::encode;

    TEST(EncodeTest, EncodesTheFiveReservedCharacters)
    {
        // Arrange
        constexpr std::string_view text = R"(<a href="x">'&'</a>)";
        std::string encoded;

        // Act
        encode(text, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "&lt;a href=&quot;x&quot;&gt;&apos;&amp;&apos;&lt;/a&gt;");
    }

    TEST(EncodeTest, LeavesOtherTextAsItIs)
    {
        // Arrange
        constexpr std::string_view text = "plain text\n\xC3\xA4";
        std::string encoded;

        // Act
        encode(text, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(encoded, "plain text\n\xC3\xA4");
    }
} // namespace Consolpp::Tests::Unit::Markup
