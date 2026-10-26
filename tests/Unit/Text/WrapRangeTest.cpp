#include "Consolpp/Text/WrapRange.hpp"

#include "Consolpp/Text/WrappedLine.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::WrappedLine;
    using Consolpp::Text::WrapRange;

    TEST(WrapRangeTest, KeepsShortText)
    {
        // Arrange
        constexpr std::string_view text = "short";
        constexpr std::size_t width = 10;
        const std::vector<std::string> expected{"short"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, BreaksAtSpaces)
    {
        // Arrange
        constexpr std::string_view text = "the quick brown fox";
        constexpr std::size_t width = 10;
        const std::vector<std::string> expected{"the quick", "brown fox"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, KeepsTextThatFitsExactly)
    {
        // Arrange
        constexpr std::string_view text = "abc def";
        constexpr std::size_t width = 7;
        const std::vector<std::string> expected{"abc def"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, CutsAWordWiderThanTheLine)
    {
        // Arrange
        constexpr std::string_view text = "abcdefghij";
        constexpr std::size_t width = 4;
        const std::vector<std::string> expected{"abcd", "efgh", "ij"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, StartsALongWordOnANewLine)
    {
        // Arrange
        constexpr std::string_view text = "ab abcdefgh";
        constexpr std::size_t width = 4;
        const std::vector<std::string> expected{"ab", "abcd", "efgh"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, KeepsExistingLineBreaks)
    {
        // Arrange
        constexpr std::string_view text = "a\nb c";
        constexpr std::size_t width = 10;
        const std::vector<std::string> expected{"a", "b c"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, CountsWideCharactersAsTwo)
    {
        // Arrange
        constexpr std::string_view text = "\u4E2D\u6587\u4E2D\u6587";
        constexpr std::size_t width = 5;
        const std::vector<std::string> expected{"\u4E2D\u6587", "\u4E2D\u6587"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, NeverSplitsAGraphemeCluster)
    {
        // Arrange
        constexpr std::string_view
            text = "\U0001F468\u200D\U0001F469\u200D\U0001F467\U0001F468\u200D\U0001F469\u200D\U0001F467";
        constexpr std::size_t width = 3;
        const std::vector<std::string> expected{
            "\U0001F468\u200D\U0001F469\u200D\U0001F467",
            "\U0001F468\u200D\U0001F469\u200D\U0001F467",
        };
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, KeepsSpacesInsideALine)
    {
        // Arrange
        constexpr std::string_view text = "a  b";
        constexpr std::size_t width = 10;
        const std::vector<std::string> expected{"a  b"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, DropsTheSpacesAtABreak)
    {
        // Arrange
        constexpr std::string_view text = "aaa   bbb";
        constexpr std::size_t width = 4;
        const std::vector<std::string> expected{"aaa", "bbb"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, KeepsLeadingSpaces)
    {
        // Arrange
        constexpr std::string_view text = "  ab";
        constexpr std::size_t width = 10;
        const std::vector<std::string> expected{"  ab"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, KeepsTrailingSpacesThatFit)
    {
        // Arrange
        constexpr std::string_view text = "ab  ";
        constexpr std::size_t width = 4;
        const std::vector<std::string> expected{"ab  "};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, DropsTrailingSpacesThatDoNotFit)
    {
        // Arrange
        constexpr std::string_view text = "ab  ";
        constexpr std::size_t width = 3;
        const std::vector<std::string> expected{"ab"};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, HasOneEmptyLineForAnEmptyText)
    {
        // Arrange
        constexpr std::string_view text{};
        constexpr std::size_t width = 5;
        const std::vector<std::string> expected{""};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, EndsWithAnEmptyLineAfterAFinalLineBreak)
    {
        // Arrange
        constexpr std::string_view text = "a\n";
        constexpr std::size_t width = 5;
        const std::vector<std::string> expected{"a", ""};
        std::vector<std::string> lines;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            lines.emplace_back(text.substr(line.start, line.end - line.start));
        }

        // Assert
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapRangeTest, KeepsTheDisplayWidthOfEveryLine)
    {
        // Arrange
        constexpr std::string_view text = "\u4E2D\u6587 ab";
        constexpr std::size_t width = 4;
        const std::vector<std::size_t> expected{4, 2};
        std::vector<std::size_t> widths;

        // Act
        for (const WrappedLine line : WrapRange{text, width}) {
            widths.push_back(line.width);
        }

        // Assert
        EXPECT_EQ(widths, expected);
    }

    TEST(WrapRangeTest, ThrowsForAWidthOfZero)
    {
        // Arrange
        constexpr WrapRange range{"a", 0};

        // Act
        const auto begin = [&range] {
            static_cast<void>(range.begin());
        };

        // Assert
        EXPECT_THROW(begin(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Text
