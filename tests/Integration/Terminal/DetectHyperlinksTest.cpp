#include "Consolpp/Terminal/DetectHyperlinks.hpp"

#include "../EnvironmentScope.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Terminal::detectHyperlinks;

    TEST(DetectHyperlinksTest, AllowsLinksByDefault)
    {
        // Arrange
        const EnvironmentScope environment{{}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, true);
    }

    TEST(DetectHyperlinksTest, RejectsLinksInJetBrainsTerminals)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERMINAL_EMULATOR", "JetBrains-JediTerm"}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, false);
    }

    TEST(DetectHyperlinksTest, AllowsLinksInOtherTerminalEmulators)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERMINAL_EMULATOR", "xterm"}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, true);
    }

    TEST(DetectHyperlinksTest, RejectsLinksInsideJetBrainsIdes)
    {
        // Arrange
        const EnvironmentScope environment{{{"IDEA_INITIAL_DIRECTORY", ""}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, false);
    }

    TEST(DetectHyperlinksTest, RejectsLinksInOldKonsole)
    {
        // Arrange
        const EnvironmentScope environment{{{"KONSOLE_VERSION", "201100"}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, false);
    }

    TEST(DetectHyperlinksTest, AllowsLinksInNewKonsole)
    {
        // Arrange
        const EnvironmentScope environment{{{"KONSOLE_VERSION", "201101"}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, true);
    }

    TEST(DetectHyperlinksTest, TreatsAnEmptyKonsoleVersionAsNoKonsole)
    {
        // Arrange
        const EnvironmentScope environment{{{"KONSOLE_VERSION", ""}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, true);
    }

    TEST(DetectHyperlinksTest, RejectsLinksForAKonsoleVersionWithTrailingText)
    {
        // Arrange
        const EnvironmentScope environment{{{"KONSOLE_VERSION", "220401abc"}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, false);
    }

    TEST(DetectHyperlinksTest, RejectsLinksForANonNumericKonsoleVersion)
    {
        // Arrange
        const EnvironmentScope environment{{{"KONSOLE_VERSION", "abc"}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, false);
    }

    TEST(DetectHyperlinksTest, RejectsLinksForANegativeKonsoleVersion)
    {
        // Arrange
        const EnvironmentScope environment{{{"KONSOLE_VERSION", "-300000"}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, false);
    }

    TEST(DetectHyperlinksTest, RejectsLinksForAKonsoleVersionOutOfRange)
    {
        // Arrange
        const EnvironmentScope environment{{{"KONSOLE_VERSION", "99999999999999999999"}}};

        // Act
        const bool result = detectHyperlinks();

        // Assert
        EXPECT_EQ(result, false);
    }
} // namespace Consolpp::Tests::Integration::Terminal
