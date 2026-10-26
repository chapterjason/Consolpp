#include "Consolpp/Prompt/DrawLines.hpp"

#include "../EnvironmentScope.hpp"
#include "../Stream/Channel.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <cstddef>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Prompt
{
    using Consolpp::Prompt::drawLines;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    TEST(DrawLinesTest, DrawsTheLinesAndPlacesTheCursorInTheCursorLine)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "80"}}};
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        const std::array<std::string, 3> lines{"first", "<info>second</info>", "third"};

        // Act
        const std::size_t rowsAboveCursor = drawLines(lines, 1, 2, output, 0);

        // Assert
        EXPECT_EQ(channel.drain(), "\r\x1b[0Jfirst\nsecond\nthird\x1b[1A\r\x1b[2C");
        EXPECT_EQ(rowsAboveCursor, 1U);
    }

    TEST(DrawLinesTest, ReplacesTheLinesDrawnBefore)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "80"}}};
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        const std::array<std::string, 1> lines{"only"};

        // Act
        static_cast<void>(drawLines(lines, 0, 0, output, 2));

        // Assert
        EXPECT_EQ(channel.drain(), "\r\x1b[2A\x1b[0Jonly\r");
    }
} // namespace Consolpp::Tests::Integration::Prompt
