#include "Consolpp/Styling/EncodeStyleEnd.hpp"

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::Style;
    using Consolpp::Styling::encodeStyleEnd;

    TEST(EncodeStyleEndTest, WritesNothingForAnEmptyStyle)
    {
        // Arrange
        constexpr Style style{};
        std::string written;

        // Act
        encodeStyleEnd(style, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "");
    }

    TEST(EncodeStyleEndTest, ResetsTheForeground)
    {
        // Arrange
        constexpr Style style{.foreground = Color::green};
        std::string written;

        // Act
        encodeStyleEnd(style, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[39m");
    }

    TEST(EncodeStyleEndTest, ResetsForegroundThenBackground)
    {
        // Arrange
        constexpr Style style{.foreground = Color::white, .background = Color::red};
        std::string written;

        // Act
        encodeStyleEnd(style, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[39;49m");
    }

    TEST(EncodeStyleEndTest, ResetsTheOptionsInAFixedOrder)
    {
        // Arrange
        constexpr Style style{.bold = true, .underline = true, .blink = true, .reverse = true, .conceal = true};
        std::string written;

        // Act
        encodeStyleEnd(style, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[22;24;25;27;28m");
    }
} // namespace Consolpp::Tests::Unit::Styling
