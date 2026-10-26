#include "Consolpp/Terminal/DetectColorMode.hpp"

#include "../EnvironmentScope.hpp"
#include "Consolpp/Terminal/ColorMode.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Terminal::ColorMode;
    using Consolpp::Terminal::detectColorMode;

    TEST(DetectColorModeTest, Uses16ColorsByDefault)
    {
        // Arrange
        const EnvironmentScope environment{{}};

        // Act
        const ColorMode result = detectColorMode();

        // Assert
        EXPECT_EQ(result, ColorMode::Colors16);
    }

    TEST(DetectColorModeTest, UsesTrueColorFromColorterm)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLORTERM", "truecolor"}}};

        // Act
        const ColorMode result = detectColorMode();

        // Assert
        EXPECT_EQ(result, ColorMode::TrueColor);
    }

    TEST(DetectColorModeTest, Uses256ColorsFromColorterm)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLORTERM", "256color"}}};

        // Act
        const ColorMode result = detectColorMode();

        // Assert
        EXPECT_EQ(result, ColorMode::Colors256);
    }

    TEST(DetectColorModeTest, IgnoresTheCaseOfColorterm)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLORTERM", "TrueColor"}}};

        // Act
        const ColorMode result = detectColorMode();

        // Assert
        EXPECT_EQ(result, ColorMode::TrueColor);
    }

    TEST(DetectColorModeTest, PrefersColortermOverTerm)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLORTERM", "256color"}, {"TERM", "xterm-truecolor"}}};

        // Act
        const ColorMode result = detectColorMode();

        // Assert
        EXPECT_EQ(result, ColorMode::Colors256);
    }

    TEST(DetectColorModeTest, FallsBackToTermWhenColortermSaysNothing)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLORTERM", "yes"}, {"TERM", "xterm-256color"}}};

        // Act
        const ColorMode result = detectColorMode();

        // Assert
        EXPECT_EQ(result, ColorMode::Colors256);
    }

    TEST(DetectColorModeTest, UsesTrueColorFromTerm)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "xterm-truecolor"}}};

        // Act
        const ColorMode result = detectColorMode();

        // Assert
        EXPECT_EQ(result, ColorMode::TrueColor);
    }

    TEST(DetectColorModeTest, Uses16ColorsForAPlainTerm)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "xterm"}}};

        // Act
        const ColorMode result = detectColorMode();

        // Assert
        EXPECT_EQ(result, ColorMode::Colors16);
    }
} // namespace Consolpp::Tests::Integration::Terminal
