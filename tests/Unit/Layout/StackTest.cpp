#include "Consolpp/Layout/Stack.hpp"

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
    using Consolpp::Layout::Element;
    using Consolpp::Layout::Extent;
    using Consolpp::Layout::Paragraph;
    using Consolpp::Layout::Stack;

    TEST(StackTest, TakesTheWidestChild)
    {
        // Arrange
        const Paragraph first{"aa bbbb"};
        const Paragraph second{"ccc"};
        const std::array<const Element*, 2> children{&first, &second};
        const Stack stack{children, 1};

        // Act
        const Extent extent = stack.measure();

        // Assert
        EXPECT_EQ(extent, (Extent{.minimum = 4, .natural = 7}));
    }

    TEST(StackTest, PutsTheChildrenOnTopOfEachOtherWithBlankLinesBetween)
    {
        // Arrange
        const Paragraph first{"a"};
        const Paragraph second{"b"};
        const std::array<const Element*, 2> children{&first, &second};
        const Stack stack{children, 1};
        constexpr std::size_t width = 2;
        const std::vector<std::string> expected{"a ", "  ", "b "};
        Canvas canvas;

        // Act
        stack.render(width, canvas);

        // Assert
        std::vector<std::string> lines;

        lines.reserve(canvas.getCount());

        for (std::size_t index = 0; index < canvas.getCount(); ++index) {
            lines.emplace_back(canvas.getLine(index));
        }

        EXPECT_EQ(lines, expected);
    }
} // namespace Consolpp::Tests::Unit::Layout
