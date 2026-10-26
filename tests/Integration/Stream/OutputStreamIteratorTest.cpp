#include "Consolpp/Stream/OutputStreamIterator.hpp"

#include "Channel.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <format>
#include <iterator>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Stream::OutputStreamIterator;

    TEST(OutputStreamIteratorTest, SatisfiesOutputIteratorForCharacters)
    {
        // Arrange
        using Iterator = OutputStreamIterator;

        // Act
        constexpr bool satisfied = std::output_iterator<Iterator, char>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(OutputStreamIteratorTest, FormatToWritesTheFormattedText)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate};
        constexpr int done = 3;
        constexpr int total = 10;

        // Act
        std::format_to(writer.createIterator(), "{} of {}", done, total);

        // Assert
        EXPECT_EQ(channel.drain(), "3 of 10");
    }

    TEST(OutputStreamIteratorTest, FormatToKeepsTheLineModeOfTheWriter)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Line};

        // Act
        std::format_to(writer.createIterator(), "{}\n{}", "first", "second");

        // Assert
        EXPECT_EQ(channel.drain(), "first\n");
    }
} // namespace Consolpp::Tests::Integration::Stream
