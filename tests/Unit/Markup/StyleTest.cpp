#include "Consolpp/Markup/Style.hpp"

#include "Consolpp/Markup/Color.hpp"

#include <optional>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::Color;
    using Consolpp::Markup::Style;

    TEST(StyleTest, DefaultsToNoColorsOptionsOrLink)
    {
        // Arrange
        constexpr Style expected{
            .foreground = std::nullopt,
            .background = std::nullopt,
            .bold = false,
            .underline = false,
            .blink = false,
            .reverse = false,
            .conceal = false,
        };

        // Act
        constexpr Style style{};

        // Assert
        EXPECT_EQ(style, expected);
    }

    TEST(StyleTest, ErrorIsWhiteOnRed)
    {
        // Arrange
        constexpr Style expected{.foreground = Color::white, .background = Color::red};

        // Act
        constexpr Style style = Style::error;

        // Assert
        EXPECT_EQ(style, expected);
    }

    TEST(StyleTest, InfoIsGreen)
    {
        // Arrange
        constexpr Style expected{.foreground = Color::green};

        // Act
        constexpr Style style = Style::info;

        // Assert
        EXPECT_EQ(style, expected);
    }

    TEST(StyleTest, CommentIsYellow)
    {
        // Arrange
        constexpr Style expected{.foreground = Color::yellow};

        // Act
        constexpr Style style = Style::comment;

        // Assert
        EXPECT_EQ(style, expected);
    }

    TEST(StyleTest, QuestionIsBlackOnCyan)
    {
        // Arrange
        constexpr Style expected{.foreground = Color::black, .background = Color::cyan};

        // Act
        constexpr Style style = Style::question;

        // Assert
        EXPECT_EQ(style, expected);
    }
} // namespace Consolpp::Tests::Unit::Markup
