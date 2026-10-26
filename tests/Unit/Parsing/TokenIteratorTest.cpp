#include "Consolpp/Parsing/TokenIterator.hpp"

#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"

#include <array>
#include <optional>
#include <span>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    TEST(TokenIteratorTest, DereferenceReadsASingleDashAsDash)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::Dash);
        EXPECT_EQ(token.getName(), "-");
        EXPECT_FALSE(token.getValue().has_value());
    }

    TEST(TokenIteratorTest, DereferenceReadsADoubleDashAsSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::Separator);
        EXPECT_EQ(token.getName(), "--");
        EXPECT_FALSE(token.getValue().has_value());
    }

    TEST(TokenIteratorTest, DereferenceReadsTextWithoutDashAsPositional)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"file.txt"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::Positional);
        EXPECT_EQ(token.getName(), "file.txt");
        EXPECT_FALSE(token.getValue().has_value());
    }

    TEST(TokenIteratorTest, DereferenceReadsAnEmptyArgumentAsPositional)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{""};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::Positional);
        EXPECT_EQ(token.getName(), "");
        EXPECT_FALSE(token.getValue().has_value());
    }

    TEST(TokenIteratorTest, DereferenceReadsASingleDashPrefixAsShortOptions)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-abc"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::ShortOptions);
        EXPECT_EQ(token.getName(), "abc");
        EXPECT_EQ(token.getArgument(), "-abc");
        EXPECT_FALSE(token.getValue().has_value());
    }

    TEST(TokenIteratorTest, DereferenceReadsANegativeNumberAsShortOptions)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"-5"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::ShortOptions);
        EXPECT_EQ(token.getName(), "5");
    }

    TEST(TokenIteratorTest, DereferenceReadsADoubleDashPrefixAsLongOption)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--name"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::LongOption);
        EXPECT_EQ(token.getName(), "name");
        EXPECT_EQ(token.getArgument(), "--name");
        EXPECT_FALSE(token.getValue().has_value());
    }

    TEST(TokenIteratorTest, DereferenceSplitsALongOptionAtTheEquals)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--name=value"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};
        const std::optional<std::string_view> expected{"value"};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::LongOption);
        EXPECT_EQ(token.getName(), "name");
        EXPECT_EQ(token.getValue(), expected);
        EXPECT_EQ(token.getArgument(), "--name=value");
    }

    TEST(TokenIteratorTest, DereferenceSplitsALongOptionAtTheFirstEquals)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--name=a=b"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};
        const std::optional<std::string_view> expected{"a=b"};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getName(), "name");
        EXPECT_EQ(token.getValue(), expected);
    }

    TEST(TokenIteratorTest, DereferenceReadsATrailingEqualsAsAnEmptyValue)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--name="};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};
        const std::optional<std::string_view> expected{""};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getName(), "name");
        EXPECT_EQ(token.getValue(), expected);
    }

    TEST(TokenIteratorTest, DereferenceReadsALeadingEqualsAsAnEmptyName)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--=value"};
        const Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};
        const std::optional<std::string_view> expected{"value"};

        // Act
        const Consolpp::Parsing::Token token = *iterator;

        // Assert
        EXPECT_EQ(token.getKind(), Consolpp::Parsing::TokenKind::LongOption);
        EXPECT_EQ(token.getName(), "");
        EXPECT_EQ(token.getValue(), expected);
    }

    TEST(TokenIteratorTest, IncrementMovesToTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"first", "second"};
        Consolpp::Parsing::TokenIterator iterator{std::span<const char* const>{arguments}.begin()};

        // Act
        ++iterator;

        // Assert
        EXPECT_EQ((*iterator).getName(), "second");
    }

    TEST(TokenIteratorTest, IteratorsAtTheSameArgumentAreEqual)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"first"};
        const std::span<const char* const> view{arguments};
        const Consolpp::Parsing::TokenIterator first{view.begin()};
        const Consolpp::Parsing::TokenIterator second{view.begin()};

        // Act
        const bool equal = first == second;

        // Assert
        EXPECT_TRUE(equal);
    }

    TEST(TokenIteratorTest, IteratorsAtDifferentArgumentsAreNotEqual)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"first"};
        const std::span<const char* const> view{arguments};
        const Consolpp::Parsing::TokenIterator first{view.begin()};
        const Consolpp::Parsing::TokenIterator second{view.end()};

        // Act
        const bool equal = first == second;

        // Assert
        EXPECT_FALSE(equal);
    }
} // namespace Consolpp::Tests::Unit::Parsing
