#include "Consolpp/Stream/BufferingScope.hpp"

#include "Channel.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "PacketChannel.hpp"

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Stream::BufferingScope;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;

    TEST(BufferingScopeTest, HoldsBackLinesWhileItLives)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Line};
        const BufferingScope buffering{output};

        // Act
        output << "a\nb\n";

        // Assert
        EXPECT_EQ(channel.drain(), "");
    }

    TEST(BufferingScopeTest, LetsTheFlushSendEverythingInOneWrite)
    {
        // Arrange
        const PacketChannel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate};
        const BufferingScope buffering{output};
        const std::vector<std::string> expected{"a\nb\n"};

        output << "a\n";
        output << "b\n";

        // Act
        output.flush();

        // Assert
        EXPECT_EQ(channel.drainPackets(), expected);
    }

    TEST(BufferingScopeTest, GivesTheStreamItsFlushModeBackAtTheEnd)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Line};
        std::optional<BufferingScope> buffering{std::in_place, output};

        // Act
        buffering.reset();

        // Assert
        EXPECT_EQ(output.getFlushMode(), FlushMode::Line);
    }
} // namespace Consolpp::Tests::Integration::Stream
