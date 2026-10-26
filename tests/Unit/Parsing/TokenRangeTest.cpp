#include "Consolpp/Parsing/TokenRange.hpp"

#include "Consolpp/Parsing/Token.hpp"

#include <array>
#include <span>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    TEST(TokenRangeTest, GetArgumentsReturnsTheWrappedArguments)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"first", "second"};
        const Consolpp::Parsing::TokenRange tokens{arguments};

        // Act
        const std::span<const char* const> wrapped = tokens.getArguments();

        // Assert
        EXPECT_EQ(wrapped.data(), arguments.data());
        EXPECT_EQ(wrapped.size(), arguments.size());
    }

    TEST(TokenRangeTest, BeginEqualsEndWithoutArguments)
    {
        // Arrange
        const Consolpp::Parsing::TokenRange tokens{std::span<const char* const>{}};

        // Act
        const bool empty = tokens.begin() == tokens.end();

        // Assert
        EXPECT_TRUE(empty);
    }

    TEST(TokenRangeTest, BeginReadsTheFirstArgument)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"first", "second"};
        const Consolpp::Parsing::TokenRange tokens{arguments};

        // Act
        const Consolpp::Parsing::Token token = *tokens.begin();

        // Assert
        EXPECT_EQ(token.getName(), "first");
    }

    TEST(TokenRangeTest, RangeForVisitsEveryArgumentInOrder)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--long", "-s", "positional"};
        const Consolpp::Parsing::TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"--long", "-s", "positional"};
        std::vector<std::string_view> visited;

        // Act
        for (const Consolpp::Parsing::Token token : tokens) {
            visited.push_back(token.getArgument());
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }
} // namespace Consolpp::Tests::Unit::Parsing
