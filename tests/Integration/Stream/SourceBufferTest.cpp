#include "Consolpp/Stream/SourceBuffer.hpp"

#include "Channel.hpp"

#include <istream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Stream::SourceBuffer;

    TEST(SourceBufferTest, IstreamExtractsThroughTheSource)
    {
        // Arrange
        const Channel channel;
        SourceBuffer buffer{channel.getReadEnd()};
        std::istream stream{&buffer};
        constexpr int expected = 42;
        int number = 0;
        std::string word;

        channel.send("42 abc");
        channel.endWriting();

        // Act
        stream >> number >> word;

        // Assert
        EXPECT_EQ(number, expected);
        EXPECT_EQ(word, "abc");
    }

    TEST(SourceBufferTest, GetlineReadsEveryLine)
    {
        // Arrange
        const Channel channel;
        SourceBuffer buffer{channel.getReadEnd()};
        std::istream stream{&buffer};
        const std::vector<std::string> expected{"first", "second"};
        std::vector<std::string> received;
        std::string line;

        channel.send("first\nsecond\n");
        channel.endWriting();

        // Act
        while (std::getline(stream, line)) {
            received.push_back(line);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(SourceBufferTest, ReportsTheEndOfInput)
    {
        // Arrange
        const Channel channel;
        SourceBuffer buffer{channel.getReadEnd()};
        std::istream stream{&buffer};
        int number = 0;

        channel.endWriting();

        // Act
        stream >> number;

        // Assert
        EXPECT_TRUE(stream.eof());
        EXPECT_TRUE(stream.fail());
    }
} // namespace Consolpp::Tests::Integration::Stream
