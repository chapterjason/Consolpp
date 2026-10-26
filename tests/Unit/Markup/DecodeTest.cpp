#include "Consolpp/Markup/Decode.hpp"

#include "Consolpp/Markup/DecodeError.hpp"
#include "Consolpp/Markup/DecodeErrorKind.hpp"

#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::decode;
    using Consolpp::Markup::DecodeError;
    using Consolpp::Markup::DecodeErrorKind;

    TEST(DecodeTest, DecodesNamedEntities)
    {
        // Arrange
        constexpr std::string_view text = "&lt;a href=&quot;x&quot;&gt;&apos;&amp;&apos;";
        std::string decoded;

        // Act
        decode(text, std::back_inserter(decoded));

        // Assert
        EXPECT_EQ(decoded, R"(<a href="x">'&')");
    }

    TEST(DecodeTest, DecodesCharacterReferencesAsUtf8)
    {
        // Arrange
        constexpr std::string_view text = "&#228;&#x1F600;";
        std::string decoded;

        // Act
        decode(text, std::back_inserter(decoded));

        // Assert
        EXPECT_EQ(decoded, "\xC3\xA4\xF0\x9F\x98\x80");
    }

    TEST(DecodeTest, LeavesTextWithoutEntitiesAsItIs)
    {
        // Arrange
        constexpr std::string_view text = "plain <text>";
        std::string decoded;

        // Act
        decode(text, std::back_inserter(decoded));

        // Assert
        EXPECT_EQ(decoded, "plain <text>");
    }

    TEST(DecodeTest, ThrowsForAnUnknownEntityAtItsPosition)
    {
        // Arrange
        std::string decoded;
        std::optional<DecodeErrorKind> kind;
        std::size_t position = 0;

        // Act
        try {
            decode("ab&nbsp;", std::back_inserter(decoded));
        } catch (const DecodeError& error) {
            kind = error.getKind();
            position = error.getPosition();
        }

        // Assert
        EXPECT_EQ(kind, std::optional<DecodeErrorKind>{DecodeErrorKind::UnknownEntity});
        EXPECT_EQ(position, 2U);
    }
} // namespace Consolpp::Tests::Unit::Markup
