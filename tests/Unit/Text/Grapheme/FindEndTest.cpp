#include "Consolpp/Text/Grapheme/FindEnd.hpp"

#include "BreakCase.hpp"
#include "BreakCases.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text::Grapheme
{
    using Consolpp::Text::Grapheme::findEnd;

    TEST(FindEndTest, FindsTheEndOfASingleCodePoint)
    {
        // Arrange
        constexpr std::string_view text = "ab";

        // Act
        const std::size_t end = findEnd(text, 0);

        // Assert
        EXPECT_EQ(end, 1U);
    }

    TEST(FindEndTest, KeepsACombiningMarkInTheCluster)
    {
        // Arrange
        constexpr std::string_view text = "éx";

        // Act
        const std::size_t end = findEnd(text, 0);

        // Assert
        EXPECT_EQ(end, 3U);
    }

    TEST(FindEndTest, StartsAtTheGivenByte)
    {
        // Arrange
        constexpr std::string_view text = "a\U0001F1E9\U0001F1EA\U0001F1EB";

        // Act
        const std::size_t end = findEnd(text, 1);

        // Assert
        EXPECT_EQ(end, 9U);
    }

    TEST(FindEndTest, PassesEveryCaseOfTheUnicodeTestFile)
    {
        // Arrange
        std::vector<std::size_t> failing;

        // Act
        for (std::size_t index = 0; index < breakCases.size(); ++index) {
            const BreakCase& testCase = breakCases.at(index);
            std::size_t start = 0;
            std::size_t cluster = 0;
            bool matching = true;

            while (start < testCase.text.size() && matching) {
                const std::size_t end = findEnd(testCase.text, start);

                matching = cluster < testCase.count && end - start == testCase.lengths.at(cluster);
                start = end;
                ++cluster;
            }

            if (!matching || cluster != testCase.count) {
                failing.push_back(index);
            }
        }

        // Assert
        EXPECT_EQ(failing, std::vector<std::size_t>{});
    }
} // namespace Consolpp::Tests::Unit::Text::Grapheme
