#include "Consolpp/Layout/Canvas.hpp"

#include <stdexcept>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Layout
{
    using Consolpp::Layout::Canvas;

    TEST(CanvasTest, AppendsToTheLastLine)
    {
        // Arrange
        Canvas canvas;

        canvas.addLine();
        canvas.addLine();

        // Act
        canvas.append("<info>ab</info>", 2);
        canvas.appendSpaces(1);

        // Assert
        EXPECT_EQ(canvas.getCount(), 2U);
        EXPECT_EQ(canvas.getLine(0), "");
        EXPECT_EQ(canvas.getLine(1), "<info>ab</info> ");
        EXPECT_EQ(canvas.getWidth(1), 3U);
    }

    TEST(CanvasTest, EscapesText)
    {
        // Arrange
        Canvas canvas;

        canvas.addLine();

        // Act
        canvas.appendText("<│");

        // Assert
        EXPECT_EQ(canvas.getLine(0), "&lt;│");
        EXPECT_EQ(canvas.getWidth(0), 2U);
    }

    TEST(CanvasTest, ClearRemovesEveryLine)
    {
        // Arrange
        Canvas canvas;

        canvas.addLine();
        canvas.appendSpaces(2);

        // Act
        canvas.clear();

        // Assert
        EXPECT_EQ(canvas.getCount(), 0U);
    }

    TEST(CanvasTest, ThrowsForALinePastTheLast)
    {
        // Arrange
        const Canvas canvas;

        // Act
        const auto read = [&canvas] {
            static_cast<void>(canvas.getLine(0));
        };

        // Assert
        EXPECT_THROW(read(), std::out_of_range);
    }
} // namespace Consolpp::Tests::Unit::Layout
