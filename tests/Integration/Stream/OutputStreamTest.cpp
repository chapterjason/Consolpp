#include "Consolpp/Stream/OutputStream.hpp"

#include "Channel.hpp"
#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Paragraph.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/Verbosity.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"
#include "PseudoTerminal.hpp"

#include <cerrno>
#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include <system_error>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Layout::Canvas;
    using Consolpp::Layout::Paragraph;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Stream::Verbosity;
    using Consolpp::Terminal::Capabilities;

    TEST(OutputStreamTest, IsTerminalIsFalseForASocket)
    {
        // Arrange
        const Channel channel;
        const OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered};

        // Act
        const bool terminal = writer.isTerminal();

        // Assert
        EXPECT_FALSE(terminal);
    }

    TEST(OutputStreamTest, BufferedModeKeepsTextUntilFlushed)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered};

        // Act
        writer << "abc";

        // Assert
        EXPECT_EQ(channel.drain(), "");
    }

    TEST(OutputStreamTest, FlushWritesTheBufferedText)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered};

        writer << "abc";

        // Act
        writer.flush();

        // Assert
        EXPECT_EQ(channel.drain(), "abc");
    }

    TEST(OutputStreamTest, BufferedModeFlushesWhenTheBufferIsFull)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered};
        const std::string first(65530, 'a');

        writer << first;

        // Act
        writer << "0123456789";

        // Assert
        EXPECT_EQ(channel.drain(), first);
    }

    TEST(OutputStreamTest, WritesTextLargerThanTheBufferDirectly)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered};
        const std::string text(70000, 'x');

        // Act
        writer << text;

        // Assert
        EXPECT_EQ(channel.drain(), text);
    }

    TEST(OutputStreamTest, LineModeFlushesAtANewline)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Line};

        // Act
        writer << "abc\n";

        // Assert
        EXPECT_EQ(channel.drain(), "abc\n");
    }

    TEST(OutputStreamTest, LineModeKeepsTextWithoutNewline)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Line};

        // Act
        writer << "abc";

        // Assert
        EXPECT_EQ(channel.drain(), "");
    }

    TEST(OutputStreamTest, SetFlushModeChangesWhenTextIsFlushed)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Line};

        writer.setFlushMode(FlushMode::Buffered);

        // Act
        writer << "abc\n";

        // Assert
        EXPECT_EQ(channel.drain(), "");
        EXPECT_EQ(writer.getFlushMode(), FlushMode::Buffered);
    }

    TEST(OutputStreamTest, ImmediateModeFlushesEveryWrite)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate};

        // Act
        writer << "abc";

        // Assert
        EXPECT_EQ(channel.drain(), "abc");
    }

    TEST(OutputStreamTest, WritesIntegersAndCharacters)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate};
        constexpr int positive = 42;
        constexpr int negative = -7;

        // Act
        writer << positive << ' ' << negative;

        // Assert
        EXPECT_EQ(channel.drain(), "42 -7");
    }

    TEST(OutputStreamTest, WritesFloatingPointNumbers)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate};
        constexpr double number = 1.5;

        // Act
        writer << number;

        // Assert
        EXPECT_EQ(channel.drain(), "1.5");
    }

    TEST(OutputStreamTest, WritesBooleansAsWords)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate};

        // Act
        writer << true << ' ' << false;

        // Assert
        EXPECT_EQ(channel.drain(), "true false");
    }

    TEST(OutputStreamTest, WritesAStringLiteralAsText)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate};

        // Act
        writer << "literal";

        // Assert
        EXPECT_EQ(channel.drain(), "literal");
    }

    TEST(OutputStreamTest, DestructionFlushesTheBufferedText)
    {
        // Arrange
        const Channel channel;
        std::optional<OutputStream> writer;

        writer.emplace(channel.getWriteEnd(), FlushMode::Buffered);
        *writer << "abc";

        // Act
        writer.reset();

        // Assert
        EXPECT_EQ(channel.drain(), "abc");
    }

    TEST(OutputStreamTest, FlushThrowsWhenTheWriteFails)
    {
        // Arrange
        OutputStream writer{-1, FlushMode::Buffered};
        const std::optional<std::error_code> expected{std::error_code{EBADF, std::generic_category()}};
        std::optional<std::error_code> error;

        writer << "abc";

        // Act
        try {
            writer.flush();
        } catch (const std::system_error& caught) {
            error = caught.code();
        }

        // Assert
        EXPECT_EQ(error, expected);
    }

    TEST(OutputStreamTest, WritesAtALevelUpToItsVerbosity)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        writer.setVerbosity(Verbosity::Verbose);

        // Act
        writer.write(Verbosity::Verbose, "a");
        writer.write(Verbosity::VeryVerbose, "b");

        // Assert
        EXPECT_EQ(channel.drain(), "a");
    }

    TEST(OutputStreamTest, QuietHidesNormalOutput)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        writer.setVerbosity(Verbosity::Quiet);

        // Act
        writer << "a";
        writer.write("b");

        // Assert
        EXPECT_EQ(channel.drain(), "");
    }

    TEST(OutputStreamTest, QuietKeepsQuietLevelOutput)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        writer.setVerbosity(Verbosity::Quiet);

        // Act
        writer.write(Verbosity::Quiet, "a");

        // Assert
        EXPECT_EQ(channel.drain(), "a");
    }

    TEST(OutputStreamTest, SilentHidesEverything)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};

        writer.setVerbosity(Verbosity::Silent);

        // Act
        writer.write(Verbosity::Quiet, "a");
        writer.writeRaw("b");

        // Assert
        EXPECT_EQ(channel.drain(), "");
    }

    TEST(OutputStreamTest, HasUnlimitedWidthOutsideATerminal)
    {
        // Arrange
        const Channel channel;
        const OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered};

        // Act
        const std::size_t width = writer.getAvailableWidth();

        // Assert
        EXPECT_EQ(width, std::numeric_limits<std::size_t>::max());
    }

    TEST(OutputStreamTest, LeavesTheLastColumnOfATerminalFree)
    {
        // Arrange
        const PseudoTerminal terminal;
        const OutputStream writer{terminal.getTerminal(), FlushMode::Buffered};
        constexpr unsigned short columns = 40;

        terminal.setColumns(columns);

        // Act
        const std::size_t width = writer.getAvailableWidth();

        // Assert
        EXPECT_EQ(width, 39U);
    }

    TEST(OutputStreamTest, WritesTheLinesOfACanvas)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        Canvas canvas;

        canvas.addLine();
        canvas.append("<info>a</info>", 1);
        canvas.addLine();
        canvas.appendText("<");

        // Act
        writer << canvas;

        // Assert
        writer.flush();
        EXPECT_EQ(channel.drain(), "a\n<\n");
    }

    TEST(OutputStreamTest, WritesAnElementAtItsNaturalWidth)
    {
        // Arrange
        const Channel channel;
        OutputStream writer{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        const Paragraph paragraph{"ab cd\ne"};

        // Act
        writer << paragraph;

        // Assert
        writer.flush();
        EXPECT_EQ(channel.drain(), "ab cd\ne    \n");
    }

    TEST(OutputStreamTest, WritesAnElementAtMostAtTheAvailableWidth)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream writer{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        const Paragraph paragraph{"ab cd"};
        constexpr unsigned short columns = 4;

        terminal.setColumns(columns);

        // Act
        writer << paragraph;

        // Assert
        writer.flush();
        EXPECT_EQ(terminal.drain(), "ab \r\ncd \r\n");
    }
} // namespace Consolpp::Tests::Integration::Stream
