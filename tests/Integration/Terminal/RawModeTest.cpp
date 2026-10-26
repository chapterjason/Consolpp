#include "Consolpp/Terminal/RawMode.hpp"

#include "../Stream/Channel.hpp"
#include "../Stream/PseudoTerminal.hpp"

#include <gtest/gtest.h>
#include <termios.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Terminal::RawMode;
    using Integration::Stream::Channel;
    using Integration::Stream::PseudoTerminal;

    namespace
    {
        termios getSettings(const int descriptor)
        {
            termios settings{};

            tcgetattr(descriptor, &settings);

            return settings;
        }
    } // namespace

    TEST(RawModeTest, TurnsOffLineBufferingEchoAndSignalKeys)
    {
        // Arrange
        const PseudoTerminal terminal;

        // Act
        const RawMode raw{terminal.getTerminal()};

        // Assert
        const termios settings = getSettings(terminal.getTerminal());

        EXPECT_EQ(settings.c_lflag & static_cast<tcflag_t>(ICANON | ECHO | ISIG), 0U);
    }

    TEST(RawModeTest, RestoresTheSettingsAtTheEnd)
    {
        // Arrange
        const PseudoTerminal terminal;
        const termios before = getSettings(terminal.getTerminal());
        // macOS sets PENDIN itself when canonical mode returns, to retype pending input; it is no setting.
        constexpr tcflag_t settable = ~static_cast<tcflag_t>(PENDIN);

        // Act
        {
            const RawMode raw{terminal.getTerminal()};
        }

        // Assert
        EXPECT_EQ(getSettings(terminal.getTerminal()).c_lflag & settable, before.c_lflag & settable);
    }

    TEST(RawModeTest, LeavesADescriptorAloneThatIsNoTerminal)
    {
        // Arrange
        const Channel channel;

        // Act
        const auto switchToRaw = [&channel] {
            const RawMode raw{channel.getWriteEnd()};
        };

        // Assert
        EXPECT_NO_THROW(switchToRaw());
    }
} // namespace Consolpp::Tests::Integration::Terminal
