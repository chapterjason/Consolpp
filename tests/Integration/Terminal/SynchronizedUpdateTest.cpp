#include "Consolpp/Terminal/SynchronizedUpdate.hpp"

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
    using Consolpp::Terminal::SynchronizedUpdate;
    using Integration::Stream::Channel;

    TEST(SynchronizedUpdateTest, WrapsWhatIsWrittenDuringItsLifetime)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};

        // Act
        {
            const SynchronizedUpdate update{output};

            output << "frame";
        }

        // Assert
        output.flush();
        EXPECT_EQ(channel.drain(), "\x1b[?2026hframe\x1b[?2026l");
    }

    TEST(SynchronizedUpdateTest, FlushesNothingItself)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};

        // Act
        {
            const SynchronizedUpdate update{output};
        }

        // Assert
        EXPECT_EQ(channel.drain(), "");
    }
} // namespace Consolpp::Tests::Integration::Terminal
