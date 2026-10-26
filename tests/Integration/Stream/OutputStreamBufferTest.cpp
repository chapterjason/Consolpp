#include "Consolpp/Stream/OutputStreamBuffer.hpp"

#include "Channel.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <ostream>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Stream::OutputStreamBuffer;

    TEST(OutputStreamBufferTest, OstreamWritesThroughTheWriter)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate};
        OutputStreamBuffer buffer{writer};
        std::ostream stream{&buffer};
        constexpr int value = 42;

        // Act
        stream << "value " << value;

        // Assert
        EXPECT_EQ(channel.drain(), "value 42");
    }

    TEST(OutputStreamBufferTest, FlushReachesTheWriter)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered};
        OutputStreamBuffer buffer{writer};
        std::ostream stream{&buffer};

        // Act
        stream << "abc" << std::flush;

        // Assert
        EXPECT_EQ(channel.drain(), "abc");
    }
} // namespace Consolpp::Tests::Integration::Stream
