#include "Consolpp/Terminal/BracketedPaste.hpp"

#include "../Stream/Channel.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::BracketedPaste;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    TEST(BracketedPasteTest, TurnsBracketedPasteOnAndOffAgainAtTheEnd)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};

        // Act
        {
            const BracketedPaste bracketedPaste{output};
        }

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[?2004h\x1b[?2004l");
    }
} // namespace Consolpp::Tests::Integration::Terminal
