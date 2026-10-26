#include "Consolpp/Parsing/RepeatedValueIterator.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <array>
#include <iterator>
#include <optional>
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
            Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
            Option{.name = "tag", .shortcut = 't', .value = OptionValue::Required, .repeatable = true},
            Option{.name = "label", .shortcut = 'l', .value = OptionValue::Optional, .repeatable = true},
        };

        constexpr const Option& tag = std::get<2>(options);
        constexpr const Option& label = std::get<3>(options);

        using RequiredIterator = Consolpp::Parsing::RepeatedValueIterator<std::string_view>;
        using OptionalIterator = Consolpp::Parsing::RepeatedValueIterator<std::optional<std::string_view>>;
    } // namespace

    TEST(RepeatedValueIteratorTest, SatisfiesInputIterator)
    {
        // Arrange
        using Iterator = RequiredIterator;

        // Act
        constexpr bool satisfied = std::input_iterator<Iterator>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(RepeatedValueIteratorTest, EqualsTheSentinelWithoutOccurrences)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a.txt", "--force"};
        const TokenRange tokens{arguments};

        // Act
        const RequiredIterator iterator{options, tag, tokens.begin(), tokens.end()};

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }

    TEST(RepeatedValueIteratorTest, YieldsLongValuesInOrder)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--tag=a", "x.txt", "--tag=b"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> values;

        // Act
        for (RequiredIterator iterator{options, tag, tokens.begin(), tokens.end()}; iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(RepeatedValueIteratorTest, TakesALongValueFromTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--tag", "a", "--tag=b"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> values;

        // Act
        for (RequiredIterator iterator{options, tag, tokens.begin(), tokens.end()}; iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(RepeatedValueIteratorTest, YieldsShortValues)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"-ta", "-t", "b"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> values;

        // Act
        for (RequiredIterator iterator{options, tag, tokens.begin(), tokens.end()}; iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(RepeatedValueIteratorTest, TakesTheValueOfAShortcutEndingABundleFromTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-ft", "a"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a"};
        std::vector<std::string_view> values;

        // Act
        for (RequiredIterator iterator{options, tag, tokens.begin(), tokens.end()}; iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(RepeatedValueIteratorTest, SkipsTheValueOfAnotherShortcutInABundle)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"-ot", "-ta"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a"};
        std::vector<std::string_view> values;

        // Act
        for (RequiredIterator iterator{options, tag, tokens.begin(), tokens.end()}; iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(RepeatedValueIteratorTest, StopsAtTheSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--tag=a", "--", "--tag=b"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a"};
        std::vector<std::string_view> values;

        // Act
        for (RequiredIterator iterator{options, tag, tokens.begin(), tokens.end()}; iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(RepeatedValueIteratorTest, YieldsAnOptionalValueGivenWithoutAsEmpty)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--label", "a.txt", "-lx"};
        const TokenRange tokens{arguments};
        const std::vector<std::optional<std::string_view>> expected{std::nullopt, "x"};
        std::vector<std::optional<std::string_view>> values;

        // Act
        for (OptionalIterator iterator{options, label, tokens.begin(), tokens.end()}; iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(RepeatedValueIteratorTest, YieldsTheDefaultsInOrder)
    {
        // Arrange
        const std::vector<std::string_view> expected{"x", "y"};
        std::vector<std::string_view> values;

        // Act
        for (RequiredIterator iterator{defaults}; iterator != std::default_sentinel; ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(RepeatedValueIteratorTest, YieldsTheDefaultsAsOptionalValues)
    {
        // Arrange
        const std::vector<std::optional<std::string_view>> expected{"x", "y"};
        std::vector<std::optional<std::string_view>> values;

        // Act
        for (OptionalIterator iterator{defaults}; iterator != std::default_sentinel; ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }
} // namespace Consolpp::Tests::Unit::Parsing
