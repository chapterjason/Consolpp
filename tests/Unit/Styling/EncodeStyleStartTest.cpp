#include "Consolpp/Styling/EncodeStyleStart.hpp"

#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"
#include "Consolpp/Terminal/ColorMode.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::Style;
    using Consolpp::Styling::encodeStyleStart;
    using Consolpp::Terminal::Capabilities;
    using Consolpp::Terminal::ColorMode;

    TEST(EncodeStyleStartTest, WritesNothingForAnEmptyStyle)
    {
        // Arrange
        constexpr Style style{};
        constexpr Terminal::Capabilities capabilities{};
        std::string written;

        // Act
        encodeStyleStart(style, capabilities, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "");
    }

    TEST(EncodeStyleStartTest, WritesTheForeground)
    {
        // Arrange
        constexpr Style style{.foreground = Color::green};
        constexpr Terminal::Capabilities capabilities{};
        std::string written;

        // Act
        encodeStyleStart(style, capabilities, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[32m");
    }

    TEST(EncodeStyleStartTest, WritesForegroundThenBackground)
    {
        // Arrange
        constexpr Style style{.foreground = Color::white, .background = Color::red};
        constexpr Terminal::Capabilities capabilities{};
        std::string written;

        // Act
        encodeStyleStart(style, capabilities, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[37;41m");
    }

    TEST(EncodeStyleStartTest, WritesTheOptionsInAFixedOrder)
    {
        // Arrange
        constexpr Style style{.bold = true, .underline = true, .blink = true, .reverse = true, .conceal = true};
        constexpr Terminal::Capabilities capabilities{};
        std::string written;

        // Act
        encodeStyleStart(style, capabilities, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[1;4;5;7;8m");
    }

    TEST(EncodeStyleStartTest, WritesAnRgbColorForTheColorMode)
    {
        // Arrange
        constexpr Style style{.foreground = Color::fromRgb(255, 128, 1)};
        constexpr Terminal::Capabilities capabilities{.colorMode = Terminal::ColorMode::TrueColor};
        std::string written;

        // Act
        encodeStyleStart(style, capabilities, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "\x1b[38;2;255;128;1m");
    }
} // namespace Consolpp::Tests::Unit::Styling
