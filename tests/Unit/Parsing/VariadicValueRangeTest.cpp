#include "Consolpp/Parsing/VariadicValueRange.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <array>
#include <ranges>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::TokenRange;
    using Consolpp::Parsing::VariadicValueRange;

    namespace
    {
        constexpr std::array<std::string_view, 2> defaults{"x", "y"};

        constexpr std::array options{
            Option{.name = "force", .shortcut = 'f'},
            Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
        };
    } // namespace

    TEST(VariadicValueRangeTest, SatisfiesInputRange)
    {
        // Arrange
        using Range = VariadicValueRange;

        // Act
        constexpr bool satisfied = std::ranges::input_range<Range>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(VariadicValueRangeTest, BeginEqualsEndForAnEmptyRange)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a", "b"};
        const TokenRange tokens{arguments};
        const VariadicValueRange values{options, tokens.end(), tokens.end(), true, {}};

        // Act
        const bool empty = values.begin() == VariadicValueRange::end();

        // Assert
        EXPECT_TRUE(empty);
    }

    TEST(VariadicValueRangeTest, RangeForVisitsEveryValueInOrder)
    {
        // Arrange
        constexpr std::array<const char*, 5> arguments{"a", "-f", "-o", "out.txt", "b"};
        const TokenRange tokens{arguments};
        const VariadicValueRange values{options, tokens.begin(), tokens.end(), true, {}};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : values) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(VariadicValueRangeTest, VisitsTheDefaultsInsteadOfArgv)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"a"};
        const TokenRange tokens{arguments};
        const VariadicValueRange values{options, tokens.begin(), tokens.end(), true, defaults};
        const std::vector<std::string_view> expected{"x", "y"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : values) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }
} // namespace Consolpp::Tests::Unit::Parsing
