#include "Consolpp/Styling/WrapMarkup.hpp"

#include "Consolpp/Markup/ParseError.hpp"
#include "Consolpp/Styling/MarkupLine.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::ParseError;
    using Consolpp::Styling::MarkupLine;
    using Consolpp::Styling::wrapMarkup;

    TEST(WrapMarkupTest, KeepsMarkupThatFitsAsItIs)
    {
        // Arrange
        constexpr std::size_t width = 10;
        std::string text{"x"};
        std::vector<MarkupLine> lines;
        const std::vector<MarkupLine> expected{{.start = 1, .length = 15, .width = 2}};

        // Act
        wrapMarkup("<info>ab</info>", width, text, lines);

        // Assert
        EXPECT_EQ(text, "x<info>ab</info>");
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapMarkupTest, WritesWrappedLinesBackAsMarkup)
    {
        // Arrange
        std::string text;
        std::vector<MarkupLine> lines;
        const std::vector<MarkupLine> expected{
            {.start = 0, .length = 37, .width = 3},
            {.start = 37, .length = 37, .width = 3},
        };

        // Act
        wrapMarkup("<info>aaa bbb</info>", 3, text, lines);

        // Assert
        EXPECT_EQ(text, R"(<style foreground="green">aaa</style><style foreground="green">bbb</style>)");
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapMarkupTest, BreaksAtLineBreaksEvenWhenTheLinesFit)
    {
        // Arrange
        constexpr std::size_t width = 10;
        std::string text;
        std::vector<MarkupLine> lines;
        const std::vector<MarkupLine> expected{
            {.start = 0, .length = 1, .width = 1},
            {.start = 1, .length = 1, .width = 1},
        };

        // Act
        wrapMarkup("a\nb", width, text, lines);

        // Assert
        EXPECT_EQ(text, "ab");
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapMarkupTest, MeasuresEntitiesByWhatTheyStandFor)
    {
        // Arrange
        std::string text;
        std::vector<MarkupLine> lines;
        const std::vector<MarkupLine> expected{{.start = 0, .length = 8, .width = 2}};

        // Act
        wrapMarkup("&lt;&gt;", 2, text, lines);

        // Assert
        EXPECT_EQ(text, "&lt;&gt;");
        EXPECT_EQ(lines, expected);
    }

    TEST(WrapMarkupTest, ThrowsForInvalidMarkup)
    {
        // Arrange
        std::string text;
        std::vector<MarkupLine> lines;

        // Act
        const auto wrap = [&text, &lines] {
            wrapMarkup("<info>a", 2, text, lines);
        };

        // Assert
        EXPECT_THROW(wrap(), ParseError);
    }
} // namespace Consolpp::Tests::Unit::Styling
