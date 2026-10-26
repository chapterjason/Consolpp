#include "Consolpp/Parsing/Token.hpp"

#include "Consolpp/Parsing/TokenKind.hpp"

#include <optional>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    TEST(TokenTest, GetKindReturnsTheKind)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::ShortOptions, "-abc", 1};

        // Act
        const Consolpp::Parsing::TokenKind kind = token.getKind();

        // Assert
        EXPECT_EQ(kind, Consolpp::Parsing::TokenKind::ShortOptions);
    }

    TEST(TokenTest, GetArgumentReturnsTheWholeArgument)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::LongOption, "--name=value", 2, 4};

        // Act
        const std::string_view argument = token.getArgument();

        // Assert
        EXPECT_EQ(argument, "--name=value");
    }

    TEST(TokenTest, GetNameSkipsTheDashes)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::LongOption, "--name", 2};

        // Act
        const std::string_view name = token.getName();

        // Assert
        EXPECT_EQ(name, "name");
    }

    TEST(TokenTest, GetNameStopsBeforeTheValue)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::LongOption, "--name=value", 2, 4};

        // Act
        const std::string_view name = token.getName();

        // Assert
        EXPECT_EQ(name, "name");
    }

    TEST(TokenTest, GetNameReturnsTheWholeArgumentWithoutDashes)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::Positional, "file.txt", 0};

        // Act
        const std::string_view name = token.getName();

        // Assert
        EXPECT_EQ(name, "file.txt");
    }

    TEST(TokenTest, GetValueReturnsTheTextAfterTheEquals)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::LongOption, "--name=value", 2, 4};
        const std::optional<std::string_view> expected{"value"};

        // Act
        const std::optional<std::string_view> value = token.getValue();

        // Assert
        EXPECT_EQ(value, expected);
    }

    TEST(TokenTest, GetValueReturnsAnEmptyValueAfterATrailingEquals)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::LongOption, "--name=", 2, 4};
        const std::optional<std::string_view> expected{""};

        // Act
        const std::optional<std::string_view> value = token.getValue();

        // Assert
        EXPECT_EQ(value, expected);
    }

    TEST(TokenTest, GetValueReturnsNothingWithoutAValue)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::LongOption, "--name", 2};

        // Act
        const std::optional<std::string_view> value = token.getValue();

        // Assert
        EXPECT_FALSE(value.has_value());
    }

    TEST(TokenTest, GetValueReturnsNothingForAPositional)
    {
        // Arrange
        const Consolpp::Parsing::Token token{Consolpp::Parsing::TokenKind::Positional, "file.txt", 0};

        // Act
        const std::optional<std::string_view> value = token.getValue();

        // Assert
        EXPECT_FALSE(value.has_value());
    }
} // namespace Consolpp::Tests::Unit::Parsing
