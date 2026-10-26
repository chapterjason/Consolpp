#include "Consolpp/Layout/Paragraph.hpp"

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Extent.hpp"
#include "Consolpp/Markup/ParseError.hpp"
#include "Consolpp/Text/Alignment.hpp"

#include <cstddef>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Layout
{
    using Consolpp::Layout::Canvas;
    using Consolpp::Layout::Extent;
    using Consolpp::Layout::Paragraph;
    using Consolpp::Markup::ParseError;
    using Consolpp::Text::Alignment;

    namespace
    {
        std::vector<std::string> collectLines(const Canvas& canvas)
        {
            std::vector<std::string> lines;

            lines.reserve(canvas.getCount());

            for (std::size_t index = 0; index < canvas.getCount(); ++index) {
                lines.emplace_back(canvas.getLine(index));
            }

            return lines;
        }
    } // namespace

    TEST(ParagraphTest, MeasuresTheWidestWordAndTheWidestLine)
    {
        // Arrange
        const Paragraph paragraph{"ab <info>abcd</info>\nabc"};

        // Act
        const Extent extent = paragraph.measure();

        // Assert
        EXPECT_EQ(extent, (Extent{.minimum = 4, .natural = 7}));
    }

    TEST(ParagraphTest, WrapsAndFillsUpTheLines)
    {
        // Arrange
        const Paragraph paragraph{"aa bbb"};
        constexpr std::size_t width = 4;
        const std::vector<std::string> expected{"aa  ", "bbb "};
        Canvas canvas;

        // Act
        paragraph.render(width, canvas);

        // Assert
        EXPECT_EQ(collectLines(canvas), expected);
    }

    TEST(ParagraphTest, AlignsTheLines)
    {
        // Arrange
        const Paragraph paragraph{"a\nbb", Alignment::Right};
        constexpr std::size_t width = 3;
        const std::vector<std::string> expected{"  a", " bb"};
        Canvas canvas;

        // Act
        paragraph.render(width, canvas);

        // Assert
        EXPECT_EQ(collectLines(canvas), expected);
    }

    TEST(ParagraphTest, ThrowsForInvalidMarkup)
    {
        // Act
        const auto create = [] {
            static_cast<void>(Paragraph{"<info>a"});
        };

        // Assert
        EXPECT_THROW(create(), ParseError);
    }
} // namespace Consolpp::Tests::Unit::Layout
