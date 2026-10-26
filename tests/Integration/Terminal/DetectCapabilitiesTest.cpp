#include "Consolpp/Terminal/DetectCapabilities.hpp"

#include "../EnvironmentScope.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"
#include "Consolpp/Terminal/ColorMode.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Terminal::Capabilities;
    using Consolpp::Terminal::ColorMode;
    using Consolpp::Terminal::detectCapabilities;

    TEST(DetectCapabilitiesTest, DetectsTheColorModeAndHyperlinks)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLORTERM", "truecolor"}}};
        constexpr Capabilities expected{.colorMode = ColorMode::TrueColor, .hyperlinks = true};

        // Act
        const Capabilities capabilities = detectCapabilities();

        // Assert
        EXPECT_EQ(capabilities, expected);
    }

    TEST(DetectCapabilitiesTest, LeavesHyperlinksOutForTerminalsThatPrintThemAsText)
    {
        // Arrange
        const EnvironmentScope environment{{{"TERMINAL_EMULATOR", "JetBrains-JediTerm"}}};
        constexpr Capabilities expected{.colorMode = ColorMode::Colors16, .hyperlinks = false};

        // Act
        const Capabilities capabilities = detectCapabilities();

        // Assert
        EXPECT_EQ(capabilities, expected);
    }
} // namespace Consolpp::Tests::Integration::Terminal
