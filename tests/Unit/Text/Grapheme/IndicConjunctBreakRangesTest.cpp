#include "Consolpp/Text/Grapheme/IndicConjunctBreakRanges.hpp"

#include "Consolpp/Text/FindProperty.hpp"
#include "Consolpp/Text/Grapheme/IndicConjunctBreak.hpp"

#include <cstdint>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Grapheme
{
    using Consolpp::Text::findProperty;
    using Consolpp::Text::Grapheme::IndicConjunctBreak;
    using Consolpp::Text::Grapheme::indicConjunctBreakRanges;

    TEST(IndicConjunctBreakRangesTest, FindsLinker)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x94D;

        // Act
        const auto value = findProperty<IndicConjunctBreak>(indicConjunctBreakRanges,
                                                            codePoint,
                                                            IndicConjunctBreak::None);

        // Assert
        EXPECT_EQ(value, IndicConjunctBreak::Linker);
    }

    TEST(IndicConjunctBreakRangesTest, FindsConsonant)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x915;

        // Act
        const auto value = findProperty<IndicConjunctBreak>(indicConjunctBreakRanges,
                                                            codePoint,
                                                            IndicConjunctBreak::None);

        // Assert
        EXPECT_EQ(value, IndicConjunctBreak::Consonant);
    }

    TEST(IndicConjunctBreakRangesTest, FindsExtend)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x301;

        // Act
        const auto value = findProperty<IndicConjunctBreak>(indicConjunctBreakRanges,
                                                            codePoint,
                                                            IndicConjunctBreak::None);

        // Assert
        EXPECT_EQ(value, IndicConjunctBreak::Extend);
    }

    TEST(IndicConjunctBreakRangesTest, FallsBackToNoneForLetters)
    {
        // Arrange
        constexpr std::uint32_t codePoint = 0x41;

        // Act
        const auto value = findProperty<IndicConjunctBreak>(indicConjunctBreakRanges,
                                                            codePoint,
                                                            IndicConjunctBreak::None);

        // Assert
        EXPECT_EQ(value, IndicConjunctBreak::None);
    }
} // namespace Consolpp::Tests::Unit::Text::Grapheme
