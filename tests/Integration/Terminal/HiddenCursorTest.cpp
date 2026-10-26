#include "Consolpp/Terminal/HiddenCursor.hpp"

#include "../Stream/Channel.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Consolpp::Terminal::HiddenCursor;
    using Integration::Stream::Channel;

    TEST(HiddenCursorTest, HidesTheCursorAndShowsItAgainAtTheEnd)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};

        // Act
        {
            const HiddenCursor hiddenCursor{output};
        }

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[?25l\x1b[?25h");
    }
} // namespace Consolpp::Tests::Integration::Terminal
