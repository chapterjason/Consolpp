#include "Consolpp/Parsing/VariadicValueIterator.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"

#include <array>
#include <iterator>
#include <span>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Parsing
{
    using Consolpp::Parsing::TokenRange;
    using Consolpp::Parsing::VariadicValueIterator;

    namespace
    {
        constexpr std::array<std::string_view, 2> defaults{"x", "y"};

        constexpr std::array options{
            Option{.name = "force", .shortcut = 'f'},
            Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
            Option{.name = "tag", .shortcut = 't', .value = OptionValue::Optional},
        };
    } // namespace

    TEST(VariadicValueIteratorTest, SatisfiesInputIterator)
    {
        // Arrange
        using Iterator = VariadicValueIterator;

        // Act
        constexpr bool satisfied = std::input_iterator<Iterator>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(VariadicValueIteratorTest, EqualsTheSentinelWithoutPositionals)
    {
        // Arrange
        constexpr std::array<const char*, 1> arguments{"--force"};
        const TokenRange tokens{arguments};

        // Act
        const VariadicValueIterator iterator{options, tokens.begin(), tokens.end(), true};

        // Assert
        EXPECT_TRUE(iterator == std::default_sentinel);
    }

    TEST(VariadicValueIteratorTest, YieldsPositionalsInOrder)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"a", "b", "c"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "b", "c"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{options, tokens.begin(), tokens.end(), true};
             iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(VariadicValueIteratorTest, SkipsAValueALongOptionTakesFromTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"a", "--output", "out.txt", "b"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{options, tokens.begin(), tokens.end(), true};
             iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(VariadicValueIteratorTest, SkipsAValueAShortcutEndingABundleTakesFromTheNextArgument)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"a", "-fo", "out.txt", "b"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{options, tokens.begin(), tokens.end(), true};
             iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(VariadicValueIteratorTest, YieldsTheArgumentAfterAnAttachedShortValue)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"a", "-oout.txt", "b"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{options, tokens.begin(), tokens.end(), true};
             iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(VariadicValueIteratorTest, YieldsTheArgumentAfterAnOptionalValueOption)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"a", "--tag", "b"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "b"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{options, tokens.begin(), tokens.end(), true};
             iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(VariadicValueIteratorTest, YieldsASingleDashAsValue)
    {
        // Arrange
        constexpr std::array<const char*, 2> arguments{"a", "-"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "-"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{options, tokens.begin(), tokens.end(), true};
             iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(VariadicValueIteratorTest, YieldsEverythingAfterTheSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 4> arguments{"a", "--", "--force", "-o"};
        const TokenRange tokens{arguments};
        const std::vector<std::string_view> expected{"a", "--force", "-o"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{options, tokens.begin(), tokens.end(), true};
             iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(VariadicValueIteratorTest, YieldsOptionsAsValuesWhenStartingAfterTheSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 3> arguments{"--", "--force", "b"};
        const TokenRange remaining{std::span<const char* const>{arguments}.subspan(1)};
        const std::vector<std::string_view> expected{"--force", "b"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{options, remaining.begin(), remaining.end(), false};
             iterator != std::default_sentinel;
             ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }

    TEST(VariadicValueIteratorTest, YieldsTheDefaultsInOrder)
    {
        // Arrange
        const std::vector<std::string_view> expected{"x", "y"};
        std::vector<std::string_view> values;

        // Act
        for (VariadicValueIterator iterator{defaults}; iterator != std::default_sentinel; ++iterator) {
            values.push_back(*iterator);
        }

        // Assert
        EXPECT_EQ(values, expected);
    }
} // namespace Consolpp::Tests::Unit::Parsing
