#include "Consolpp/Parsing/HasValueInNextArgument.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"

#include <array>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::hasValueInNextArgument;
    using Consolpp::Parsing::Token;
    using Consolpp::Parsing::TokenKind;

    namespace
    {
        constexpr std::array options{
            Option{.name = "force", .shortcut = 'f'},
            Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
            Option{.name = "tag", .shortcut = 't', .value = OptionValue::Optional},
        };
    } // namespace

    TEST(HasValueInNextArgumentTest, ReturnsTrueForALongRequiredOptionWithoutValue)
    {
        // Arrange
        const Token token{TokenKind::LongOption, "--output", 2};

        // Act
        const bool result = hasValueInNextArgument(options, token);

        // Assert
        EXPECT_TRUE(result);
    }

    TEST(HasValueInNextArgumentTest, ReturnsFalseForALongRequiredOptionWithAnInlineValue)
    {
        // Arrange
        const Token token{TokenKind::LongOption, "--output=a", 2, 6};

        // Act
        const bool result = hasValueInNextArgument(options, token);

        // Assert
        EXPECT_FALSE(result);
    }

    TEST(HasValueInNextArgumentTest, ReturnsFalseForALongFlag)
    {
        // Arrange
        const Token token{TokenKind::LongOption, "--force", 2};

        // Act
        const bool result = hasValueInNextArgument(options, token);

        // Assert
        EXPECT_FALSE(result);
    }

    TEST(HasValueInNextArgumentTest, ReturnsFalseForAnUnknownLongOption)
    {
        // Arrange
        const Token token{TokenKind::LongOption, "--bogus", 2};

        // Act
        const bool result = hasValueInNextArgument(options, token);

        // Assert
        EXPECT_FALSE(result);
    }

    TEST(HasValueInNextArgumentTest, ReturnsTrueForShortOptionsEndingWithARequiredShortcut)
    {
        // Arrange
        const Token token{TokenKind::ShortOptions, "-fo", 1};

        // Act
        const bool result = hasValueInNextArgument(options, token);

        // Assert
        EXPECT_TRUE(result);
    }

    TEST(HasValueInNextArgumentTest, ReturnsFalseForARequiredShortcutWithAnInlineValue)
    {
        // Arrange
        const Token token{TokenKind::ShortOptions, "-oa", 1};

        // Act
        const bool result = hasValueInNextArgument(options, token);

        // Assert
        EXPECT_FALSE(result);
    }

    TEST(HasValueInNextArgumentTest, ReturnsFalseWhenAnOptionalShortcutTakesTheRest)
    {
        // Arrange
        const Token token{TokenKind::ShortOptions, "-to", 1};

        // Act
        const bool result = hasValueInNextArgument(options, token);

        // Assert
        EXPECT_FALSE(result);
    }

    TEST(HasValueInNextArgumentTest, ReturnsFalseForAPositional)
    {
        // Arrange
        const Token token{TokenKind::Positional, "output", 0};

        // Act
        const bool result = hasValueInNextArgument(options, token);

        // Assert
        EXPECT_FALSE(result);
    }
} // namespace Consolpp::Tests::Unit::Parsing
