#include "Consolpp/Text/Grapheme/BreakRanges.hpp"

#include "Consolpp/Text/FindProperty.hpp"
#include "Consolpp/Text/Grapheme/Break.hpp"

#include <cstdint>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Grapheme
{
    using Consolpp::Text::findProperty;
    using Consolpp::Text::Grapheme::Break;
    using Consolpp::Text::Grapheme::breakRanges;

    TEST(BreakRangesTest, FindsCarriageReturn)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x0D;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::CarriageReturn);
    }

    TEST(BreakRangesTest, FindsLineFeed)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x0A;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::LineFeed);
    }

    TEST(BreakRangesTest, FindsControl)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x07;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::Control);
    }

    TEST(BreakRangesTest, FindsExtend)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x301;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::Extend);
    }

    TEST(BreakRangesTest, FindsZeroWidthJoiner)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x200D;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::ZeroWidthJoiner);
    }

    TEST(BreakRangesTest, FindsRegionalIndicator)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x1F1E6;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::RegionalIndicator);
    }

    TEST(BreakRangesTest, FindsPrepend)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x600;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::Prepend);
    }

    TEST(BreakRangesTest, FindsSpacingMark)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x903;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::SpacingMark);
    }

    TEST(BreakRangesTest, FindsLeadingJamo)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x1100;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::LeadingJamo);
    }

    TEST(BreakRangesTest, FindsVowelJamo)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x1161;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::VowelJamo);
    }

    TEST(BreakRangesTest, FindsTrailingJamo)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x11A8;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::TrailingJamo);
    }

    TEST(BreakRangesTest, FindsLeadingVowelSyllable)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0xAC00;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::LeadingVowelSyllable);
    }

    TEST(BreakRangesTest, FindsLeadingVowelTrailingSyllable)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0xAC01;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::LeadingVowelTrailingSyllable);
    }

    TEST(BreakRangesTest, FallsBackToOtherForLetters)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x41;

        // Act
        const auto value = findProperty<Break>(breakRanges, codePoint, Break::Other);

        // Assert
        EXPECT_EQ(value, Break::Other);
    }
} // namespace Consolpp::Tests::Unit::Text::Grapheme
