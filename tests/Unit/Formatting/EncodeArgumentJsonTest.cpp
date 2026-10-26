#include "Consolpp/Formatting/EncodeArgumentJson.hpp"

#include "Consolpp/Argument.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::encodeArgumentJson;

    namespace
    {
        constexpr std::array<std::string_view, 2> values{"a", "b"};
    } // namespace

    TEST(EncodeArgumentJsonTest, EncodesARequiredArgument)
    {
        // Arrange
        constexpr Argument argument{.name = "source", .description = "The source"};
        std::string encoded;

        // Act
        encodeArgumentJson(argument, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"source","required":true,"variadic":false,"description":"The source","defaultValue":null,"defaultValues":[]})");
    }

    TEST(EncodeArgumentJsonTest, EncodesAnOptionalVariadicArgumentWithDefaults)
    {
        // Arrange
        constexpr Argument argument{.name = "files", .required = false, .variadic = true, .defaultValues = values};
        std::string encoded;

        // Act
        encodeArgumentJson(argument, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"files","required":false,"variadic":true,"description":"","defaultValue":null,"defaultValues":["a","b"]})");
    }

    TEST(EncodeArgumentJsonTest, KeepsLineBreaksOfTheDescription)
    {
        // Arrange
        constexpr Argument argument{.name = "target", .required = false, .description = "a\nb", .defaultValue = "out"};
        std::string encoded;

        // Act
        encodeArgumentJson(argument, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"target","required":false,"variadic":false,"description":"a\nb","defaultValue":"out","defaultValues":[]})");
    }

    TEST(EncodeArgumentJsonTest, StripsTheMarkupOfTheDescription)
    {
        // Arrange
        constexpr Argument argument{.name = "source", .description = "The <info>source</info>"};
        std::string encoded;

        // Act
        encodeArgumentJson(argument, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"source","required":true,"variadic":false,"description":"The source","defaultValue":null,"defaultValues":[]})");
    }
} // namespace Consolpp::Tests::Unit::Formatting
