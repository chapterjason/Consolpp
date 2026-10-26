#include "Consolpp/Layout/Columns.hpp"

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Layout/Extent.hpp"
#include "Consolpp/Layout/Paragraph.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Layout
{
    using Consolpp::Layout::Canvas;
    using Consolpp::Layout::Columns;
    using Consolpp::Layout::Element;
    using Consolpp::Layout::Extent;
    using Consolpp::Layout::Paragraph;

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

    TEST(ColumnsTest, AddsUpTheWidthsAndGaps)
    {
        // Arrange
        const Paragraph first{"aa bbbb"};
        const Paragraph second{"c"};
        const std::array<const Element*, 2> children{&first, &second};
        const Columns columns{children, 2};

        // Act
        const Extent extent = columns.measure();

        // Assert
        EXPECT_EQ(extent, (Extent{.minimum = 7, .natural = 10}));
    }

    TEST(ColumnsTest, PutsTheChildrenSideBySideAndFillsUpShorterOnes)
    {
        // Arrange
        const Paragraph first{"a\nb"};
        const Paragraph second{"cc"};
        const std::array<const Element*, 2> children{&first, &second};
        const Columns columns{children, 1};
        constexpr std::size_t width = 6;
        const std::vector<std::string> expected{"a cc  ", "b     "};
        Canvas canvas;

        // Act
        columns.render(width, canvas);

        // Assert
        EXPECT_EQ(collectLines(canvas), expected);
    }

    TEST(ColumnsTest, NarrowsTheWidestChildFirst)
    {
        // Arrange
        const Paragraph first{"aa bb cc"};
        const Paragraph second{"dd"};
        const std::array<const Element*, 2> children{&first, &second};
        const Columns columns{children, 1};
        constexpr std::size_t width = 8;
        const std::vector<std::string> expected{"aa bb dd", "cc      "};
        Canvas canvas;

        // Act
        columns.render(width, canvas);

        // Assert
        EXPECT_EQ(collectLines(canvas), expected);
    }
} // namespace Consolpp::Tests::Unit::Layout
