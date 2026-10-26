#include "Consolpp/Parsing/RepeatedValueRange.hpp"

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

    namespace
    {
        constexpr std::array<std::string_view, 2> defaults{"x", "y"};

        constexpr std::array options{
            Option{.name = "force", .shortcut = 'f'},
            Option{.name = "tag", .shortcut = 't', .value = OptionValue::Required, .repeatable = true},
        };

        constexpr const Option& tag = std::get<1>(options);

        using RepeatedValueRange = Consolpp::Parsing::RepeatedValueRange<std::string_view>;
    } // namespace

    TEST(RepeatedValueRangeTest, SatisfiesInputRange)
    {
        // Arrange
        using Range = RepeatedValueRange;

        // Act
        constexpr bool satisfied = std::ranges::input_range<Range>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(RepeatedValueRangeTest, BeginEqualsEndForAnEmptyRange)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"--tag=a", "--tag=b"};
        const TokenRange tokens{arguments};
        const RepeatedValueRange values{options, tag, tokens.end(), tokens.end(), {}};

        // Act
        const bool empty = values.begin() == values.end();

        // Assert
        EXPECT_TRUE(empty);
    }

    TEST(RepeatedValueRangeTest, RangeForVisitsEveryValueInOrder)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"--tag=a", "-f", "-t", "b"};
        const TokenRange tokens{arguments};
        const RepeatedValueRange values{options, tag, tokens.begin(), tokens.end(), {}};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> visited;

        // Act
        for (const std::string_view value : values) {
            visited.push_back(value);
        }

        // Assert
        EXPECT_EQ(visited, expected);
    }

    TEST(RepeatedValueRangeTest, VisitsTheDefaultsInsteadOfArgv)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--tag=a"};
        const TokenRange tokens{arguments};
        const RepeatedValueRange values{options, tag, tokens.begin(), tokens.end(), defaults};
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
