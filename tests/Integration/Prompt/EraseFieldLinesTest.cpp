#include "Consolpp/Prompt/EraseFieldLines.hpp"

#include "../Stream/Channel.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Prompt
{
    using Consolpp::Prompt::eraseFieldLines;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    TEST(EraseFieldLinesTest, ClearsFromTheFirstLineOn)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};

        // Act
        eraseFieldLines(output, 2);

        // Assert
        output.flush();
        EXPECT_EQ(channel.drain(), "\r\x1b[2A\x1b[0J");
    }
} // namespace Consolpp::Tests::Integration::Prompt
