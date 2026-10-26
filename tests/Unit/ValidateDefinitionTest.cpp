#include "Consolpp/ValidateDefinition.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Signature.hpp"

#include <array>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{

    namespace
    {
        // A failed check inside the consteval function is a substitution failure here, not a hard error.
        template <auto CreateCommand>
        concept Valid = requires { typename std::bool_constant<Consolpp::validateDefinition(CreateCommand())>; };

        constexpr Consolpp::Signature signature{std::array<Option, 0>{}, std::array<Argument, 0>{}};
    } // namespace

    TEST(ValidateDefinitionTest, AcceptsASimpleName)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "deploy", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_TRUE(valid);
    }

    TEST(ValidateDefinitionTest, AcceptsANameWithColonSeparatedParts)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "cache:clear", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_TRUE(valid);
    }

    TEST(ValidateDefinitionTest, AcceptsDistinctAliases)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{
                .name = "cache:clear",
                .aliases = std::array<std::string_view, 2>{"cc", "cache:flush"},
                .signature = signature,
            };
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_TRUE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsAnEmptyName)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsANameStartingWithAColon)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = ":clear", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsANameEndingWithAColon)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "cache:", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsANameWithTwoColonsInARow)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "cache::clear", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsAnEmptyAlias)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "deploy", .aliases = std::array<std::string_view, 1>{""}, .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsAnAliasWithAnEmptyPart)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{
                .name = "deploy",
                .aliases = std::array<std::string_view, 1>{"d:"},
                .signature = signature,
            };
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsAnAliasEqualToTheName)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{
                .name = "deploy",
                .aliases = std::array<std::string_view, 1>{"deploy"},
                .signature = signature,
            };
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsTwoEqualAliases)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{
                .name = "deploy",
                .aliases = std::array<std::string_view, 2>{"d", "d"},
                .signature = signature,
            };
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsTheCompletionRequestAsName)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "_complete", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsTheCompletionRequestAsAlias)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{
                .name = "complete",
                .aliases = std::array<std::string_view, 1>{"_complete"},
                .signature = signature,
            };
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, AcceptsMarkupInTheDescriptionAndHelp)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{
                .name = "deploy",
                .description = "<info>Deploys</info>",
                .help = "Copies &amp; checks",
                .signature = signature,
            };
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_TRUE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsInvalidMarkupInTheDescription)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "deploy", .description = "<info>Deploys", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }

    TEST(ValidateDefinitionTest, RejectsInvalidMarkupInTheHelp)
    {
        // Arrange
        constexpr auto command = [] {
            return Definition{.name = "deploy", .help = "Copies &nbsp; checks", .signature = signature};
        };

        // Act
        constexpr bool valid = Valid<command>;

        // Assert
        EXPECT_FALSE(valid);
    }
} // namespace Consolpp::Tests::Unit
