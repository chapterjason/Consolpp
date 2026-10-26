#include "Consolpp/Terminal/HasColorSupport.hpp"

#include "../EnvironmentScope.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Terminal::hasColorSupport;

    TEST(HasColorSupportTest, IsFalseForNoColor)
    {
        // Arrange
        const EnvironmentScope environment{{{"NO_COLOR", "1"}, {"FORCE_COLOR", "1"}, {"COLORTERM", "truecolor"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_FALSE(supported);
    }

    TEST(HasColorSupportTest, IgnoresAnEmptyNoColor)
    {
        // Arrange
        const EnvironmentScope environment{{{"NO_COLOR", ""}, {"TERM", "xterm"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_TRUE(supported);
    }

    TEST(HasColorSupportTest, IsTrueForForceColorWithoutTerminal)
    {
        // Arrange
        const EnvironmentScope environment{{{"FORCE_COLOR", "1"}}};

        // Act
        const bool supported = hasColorSupport(false);

        // Assert
        EXPECT_TRUE(supported);
    }

    TEST(HasColorSupportTest, IgnoresAnEmptyForceColor)
    {
        // Arrange
        const EnvironmentScope environment{{{"FORCE_COLOR", ""}}};

        // Act
        const bool supported = hasColorSupport(false);

        // Assert
        EXPECT_FALSE(supported);
    }

    TEST(HasColorSupportTest, IsFalseWithoutTerminal)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "xterm-256color"}, {"COLORTERM", "truecolor"}}};

        // Act
        const bool supported = hasColorSupport(false);

        // Assert
        EXPECT_FALSE(supported);
    }

    TEST(HasColorSupportTest, IsTrueForHyper)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM_PROGRAM", "Hyper"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_TRUE(supported);
    }

    TEST(HasColorSupportTest, IsTrueForAnySetColorterm)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLORTERM", ""}, {"TERM", "dumb"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_TRUE(supported);
    }

    TEST(HasColorSupportTest, IsFalseForADumbTerminal)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "dumb"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_FALSE(supported);
    }

    TEST(HasColorSupportTest, IsTrueForATermWithAKnownPrefix)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "screen.xterm-new"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_TRUE(supported);
    }

    TEST(HasColorSupportTest, IsTrueForATermEndingIn256Color)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "foot-256color"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_TRUE(supported);
    }

    TEST(HasColorSupportTest, IsTrueForATermEndingIn256ColorBce)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "custom-256color-bce"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_TRUE(supported);
    }

    TEST(HasColorSupportTest, IsFalseForAnUnknownTerm)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "foot"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_FALSE(supported);
    }

    TEST(HasColorSupportTest, IsFalseWithoutTerm)
    {
        // Arrange
        const EnvironmentScope environment{{}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_FALSE(supported);
    }

    TEST(HasColorSupportTest, ComparesTheTermCaseSensitively)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERM", "XTERM"}}};

        // Act
        const bool supported = hasColorSupport(true);

        // Assert
        EXPECT_FALSE(supported);
    }
} // namespace Consolpp::Tests::Integration::Terminal
