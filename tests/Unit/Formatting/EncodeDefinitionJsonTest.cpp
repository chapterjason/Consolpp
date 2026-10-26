#include "Consolpp/Formatting/EncodeDefinitionJson.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Option.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::encodeDefinitionJson;

    namespace
    {
        constexpr std::array<std::string_view, 1> aliases{"d"};
        constexpr std::array<std::string_view, 1> usages{"deploy a"};
        constexpr std::array<Option, 1> options{{{.name = "force"}}};
        constexpr std::array<Argument, 1> arguments{{{.name = "source"}}};
    } // namespace

    TEST(EncodeDefinitionJsonTest, EncodesEveryMember)
    {
        // Arrange
        constexpr DefinitionView definition{
            .name = "deploy",
            .description = "Deploys",
            .help = "Copies",
            .aliases = aliases,
            .usages = usages,
            .options = options,
            .arguments = arguments,
        };
        std::string encoded;

        // Act
        encodeDefinitionJson(definition, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"deploy","description":"Deploys","help":"Copies","aliases":["d"],"usages":["deploy a"],"options":[{"name":"force","shortcut":null,"value":"none","repeatable":false,"description":"","defaultValue":null,"defaultValues":[],"defaultEnabled":null}],"arguments":[{"name":"source","required":true,"variadic":false,"description":"","defaultValue":null,"defaultValues":[]}],"hidden":false})");
    }

    TEST(EncodeDefinitionJsonTest, EncodesAnEmptyHiddenDefinition)
    {
        // Arrange
        constexpr DefinitionView definition{.name = "secret", .hidden = true};
        std::string encoded;

        // Act
        encodeDefinitionJson(definition, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"secret","description":"","help":"","aliases":[],"usages":[],"options":[],"arguments":[],"hidden":true})");
    }

    TEST(EncodeDefinitionJsonTest, StripsTheMarkupOfTheDescriptionAndHelp)
    {
        // Arrange
        constexpr DefinitionView definition{
            .name = "status",
            .description = "<info>Shows</info>",
            .help = "Copies &lt;all&gt;",
        };
        std::string encoded;

        // Act
        encodeDefinitionJson(definition, std::back_inserter(encoded));

        // Assert
        EXPECT_EQ(
            encoded,
            R"({"name":"status","description":"Shows","help":"Copies <all>","aliases":[],"usages":[],"options":[],"arguments":[],"hidden":false})");
    }
} // namespace Consolpp::Tests::Unit::Formatting
