#include "Consolpp/Formatting/EncodeOptionJson.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::encodeOptionJson;

    namespace
    {
        constexpr std::array<std::string_view, 1> values{"a"};
    } // namespace

    TEST(EncodeOptionJsonTest, EncodesAFlag)
    {
        // Arrange
        constexpr Option option{.name = "force", .shortcut = 'f', .description = "Overwrite"};
        std::string encoded;

        // Act
        encodeOptionJson(option, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"force","shortcut":"f","value":"none","repeatable":false,"description":"Overwrite","defaultValue":null,"defaultValues":[],"defaultEnabled":null})");
    }

    TEST(EncodeOptionJsonTest, EncodesAFlagThatIsEnabledByDefault)
    {
        // Arrange
        constexpr Option option{.name = "color", .defaultEnabled = true};
        std::string encoded;

        // Act
        encodeOptionJson(option, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"color","shortcut":null,"value":"none","repeatable":false,"description":"","defaultValue":null,"defaultValues":[],"defaultEnabled":true})");
    }

    TEST(EncodeOptionJsonTest, EncodesAnOptionWithARequiredValue)
    {
        // Arrange
        constexpr Option option{.name = "output", .value = OptionValue::Required, .defaultValue = "out"};
        std::string encoded;

        // Act
        encodeOptionJson(option, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"output","shortcut":null,"value":"required","repeatable":false,"description":"","defaultValue":"out","defaultValues":[],"defaultEnabled":null})");
    }

    TEST(EncodeOptionJsonTest, EncodesARepeatableOptionWithAnOptionalValue)
    {
        // Arrange
        constexpr Option option{
            .name = "tag",
            .value = OptionValue::Optional,
            .repeatable = true,
            .defaultValues = values,
        };
        std::string encoded;

        // Act
        encodeOptionJson(option, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"tag","shortcut":null,"value":"optional","repeatable":true,"description":"","defaultValue":null,"defaultValues":["a"],"defaultEnabled":null})");
    }

    TEST(EncodeOptionJsonTest, StripsTheMarkupOfTheDescription)
    {
        // Arrange
        constexpr Option option{.name = "force", .description = "<info>Overwrite</info>"};
        std::string encoded;

        // Act
        encodeOptionJson(option, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"force","shortcut":null,"value":"none","repeatable":false,"description":"Overwrite","defaultValue":null,"defaultValues":[],"defaultEnabled":null})");
    }
} // namespace Consolpp::Tests::Unit::Formatting
