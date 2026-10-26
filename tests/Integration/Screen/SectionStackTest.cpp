#include "Consolpp/Screen/SectionStack.hpp"

#include "../Stream/Channel.hpp"
#include "../Stream/PseudoTerminal.hpp"
#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Paragraph.hpp"
#include "Consolpp/Screen/CursorPlacement.hpp"
#include "Consolpp/Screen/Section.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <utility>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Screen
{
    using Consolpp::Layout::Canvas;
    using Consolpp::Layout::Paragraph;
    using Consolpp::Screen::CursorPlacement;
    using Consolpp::Screen::Section;
    using Consolpp::Screen::SectionStack;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;
    using Integration::Stream::PseudoTerminal;

    namespace
    {
        constexpr unsigned short columns = 20;
    } // namespace

    // A terminal turns "\n" into "\r\n" on the way out.
    TEST(SectionStackTest, AppendWritesTheLines)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        terminal.setColumns(columns);

        // Act
        section.append("a\nb");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026ha\r\nb\r\n\x1b[2A\x1b[?2026l");
    }

    TEST(SectionStackTest, AppendWritesTheStyles)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        terminal.setColumns(columns);

        // Act
        section.append("<info>ok</info>");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\x1b[32mok\x1b[39m\r\n\x1b[1A\x1b[?2026l");
    }

    TEST(SectionStackTest, AppendToAnEarlierSectionWritesTheLaterSectionsAgain)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& first = stack.createSection();
        Section& second = stack.createSection();

        terminal.setColumns(columns);
        first.append("one");
        second.append("two");
        static_cast<void>(terminal.drain());

        // Act
        first.append("three");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[0Jone\r\nthree\r\ntwo\r\n\x1b[3A\x1b[?2026l");
    }

    TEST(SectionStackTest, OverwriteReplacesTheLines)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        terminal.setColumns(columns);
        section.append("a\nb");
        static_cast<void>(terminal.drain());

        // Act
        section.overwrite("c");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[0Jc\r\n\x1b[1A\x1b[?2026l");
    }

    TEST(SectionStackTest, ClearRemovesTheLinesAndWritesTheLaterSectionsAgain)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& first = stack.createSection();
        Section& second = stack.createSection();

        terminal.setColumns(columns);
        first.append("one");
        second.append("two");
        static_cast<void>(terminal.drain());

        // Act
        first.clear();

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[0Jtwo\r\n\x1b[1A\x1b[?2026l");
    }

    TEST(SectionStackTest, MaxHeightShowsOnlyTheLastLines)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr std::size_t maxHeight = 2;

        terminal.setColumns(columns);
        section.setMaxHeight(maxHeight);
        section.append("a\nb");
        static_cast<void>(terminal.drain());

        // Act
        section.append("c");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[0Jb\r\nc\r\n\x1b[2A\x1b[?2026l");
    }

    TEST(SectionStackTest, ClearsWrappedLinesFromTheTop)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr unsigned short narrow = 4;

        terminal.setColumns(narrow);
        section.append("abcdef");
        static_cast<void>(terminal.drain());

        // Act
        section.overwrite("x");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[0Jx\r\n\x1b[1A\x1b[?2026l");
    }

    TEST(SectionStackTest, OverwriteWritesANewLineWhenNotATerminal)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        section.append("a");
        static_cast<void>(channel.drain());

        // Act
        section.overwrite("<info>b</info>");

        // Assert
        EXPECT_EQ(channel.drain(), "b\n");
    }

    TEST(SectionStackTest, ClearWritesNothingWhenNotATerminal)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        section.append("a");
        static_cast<void>(channel.drain());

        // Act
        section.clear();

        // Assert
        EXPECT_EQ(channel.drain(), "");
    }

    TEST(SectionStackTest, OverwriteWritesStylesWithoutCursorMovesWhenNotATerminal)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        section.append("a");
        static_cast<void>(channel.drain());

        // Act
        section.overwrite("<info>b</info>");

        // Assert
        EXPECT_EQ(channel.drain(), "\x1b[32mb\x1b[39m\n");
    }

    TEST(SectionStackTest, KeepsTheFlushModeOfTheStream)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Line, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        terminal.setColumns(columns);
        section.append("a");

        // Act
        section.overwrite("b\nc");

        // Assert
        EXPECT_EQ(output.getFlushMode(), FlushMode::Line);
    }

    TEST(SectionStackTest, LeavesOutTheFirstLinesToKeepTheCursorRowInTheWindow)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr unsigned short rows = 3;

        terminal.setColumns(columns);
        terminal.setRows(rows);

        // Act
        section.append("a\nb\nc");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026hb\r\nc\r\n\x1b[2A\x1b[?2026l");
    }

    TEST(SectionStackTest, LeavesOutTheFirstLinesAcrossSections)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& first = stack.createSection();
        Section& second = stack.createSection();
        constexpr unsigned short rows = 4;

        terminal.setColumns(columns);
        terminal.setRows(rows);
        first.append("a\nb");
        static_cast<void>(terminal.drain());

        // Act
        second.append("c\nd");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[0Jb\r\nc\r\nd\r\n\x1b[3A\x1b[?2026l");
    }

    TEST(SectionStackTest, ClearsFromTheTopAfterTheWindowGrew)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr unsigned short rows = 3;
        constexpr unsigned short tallerRows = 10;

        terminal.setColumns(columns);
        terminal.setRows(rows);
        section.append("a\nb\nc");
        terminal.setRows(tallerRows);
        static_cast<void>(terminal.drain());

        // Act
        section.overwrite("x");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[0Jx\r\n\x1b[1A\x1b[?2026l");
    }

    TEST(SectionStackTest, CountsWrappedLinesAgainstTheLimit)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr unsigned short narrow = 2;
        constexpr unsigned short rows = 3;

        terminal.setColumns(narrow);
        terminal.setRows(rows);

        // Act
        section.append("aaaa\nb");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026hb\r\n\x1b[1A\x1b[?2026l");
    }

    TEST(SectionStackTest, TakesTheRowsTheHeightLimitReturns)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr unsigned short rows = 10;
        constexpr std::size_t share = 5;
        std::size_t given = 0;

        terminal.setColumns(columns);
        terminal.setRows(rows);
        stack.setHeightLimit([&given](const std::size_t windowRows) {
            given = windowRows;

            return windowRows / share;
        });
        static_cast<void>(terminal.drain());

        // Act
        section.append("a\nb\nc");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026hb\r\nc\r\n\x1b[2A\x1b[?2026l");
        EXPECT_EQ(given, 10U);
    }

    TEST(SectionStackTest, GrowsFreelyWithoutHeightLimit)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr unsigned short rows = 2;

        terminal.setColumns(columns);
        terminal.setRows(rows);
        stack.setHeightLimit(nullptr);
        static_cast<void>(terminal.drain());

        // Act
        section.append("a\nb\nc");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026ha\r\nb\r\nc\r\n\x1b[3A\x1b[?2026l");
    }

    TEST(SectionStackTest, SetHeightLimitDrawsTheStackAgain)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr std::size_t limit = 1;

        terminal.setColumns(columns);
        section.append("a\nb");
        static_cast<void>(terminal.drain());

        // Act
        stack.setHeightLimit([](const std::size_t /*rows*/) {
            return limit;
        });

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[0Jb\r\n\x1b[1A\x1b[?2026l");
    }

    TEST(SectionStackTest, OverwriteDrawsAnElementAtTheAvailableWidth)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        const Paragraph paragraph{"ab cd"};
        constexpr unsigned short narrow = 4;

        terminal.setColumns(narrow);

        // Act
        section.overwrite(paragraph);

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026hab \r\ncd \r\n\x1b[2A\x1b[?2026l");
    }

    TEST(SectionStackTest, AppendTakesTheLinesOfACanvas)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        Canvas canvas;

        terminal.setColumns(columns);
        canvas.addLine();
        canvas.append("a", 1);
        canvas.addLine();
        canvas.append("b", 1);

        // Act
        section.append(canvas);

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026ha\r\nb\r\n\x1b[2A\x1b[?2026l");
    }

    TEST(SectionStackTest, WritesNothingWhenNothingIsDrawnOrToDraw)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr std::size_t maxHeight = 3;

        terminal.setColumns(columns);

        // Act
        section.setMaxHeight(maxHeight);

        // Assert
        EXPECT_EQ(terminal.drain(), "");
    }

    TEST(SectionStackTest, MovesTheCursorBelowTheStackAtTheEnd)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        std::optional<SectionStack> stack{std::in_place, output};

        terminal.setColumns(columns);
        stack->createSection().append("a\nb");
        static_cast<void>(terminal.drain());

        // Act
        stack.reset();

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[2B");
    }

    TEST(SectionStackTest, CountsLinesWiderThanHalfTheWindowTwiceByDefault)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr unsigned short rows = 6;

        terminal.setColumns(columns);
        terminal.setRows(rows);

        // Act
        section.append("aaaaaaaaaaaaaaa\nbbbbbbbbbbbbbbb\nccccccccccccccc");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026hbbbbbbbbbbbbbbb\r\nccccccccccccccc\r\n\x1b[2A\x1b[?2026l");
    }

    TEST(SectionStackTest, CountsAtTheCurrentWidthWithoutReflowWidth)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();
        constexpr unsigned short rows = 6;

        terminal.setColumns(columns);
        terminal.setRows(rows);
        stack.setReflowWidth(nullptr);

        // Act
        section.append("aaaaaaaaaaaaaaa\nbbbbbbbbbbbbbbb\nccccccccccccccc");

        // Assert
        EXPECT_EQ(terminal.drain(),
                  "\x1b[?2026haaaaaaaaaaaaaaa\r\nbbbbbbbbbbbbbbb\r\nccccccccccccccc\r\n\x1b[3A\x1b[?2026l");
    }

    TEST(SectionStackTest, ErasesByCountingWithTheCursorBelow)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        terminal.setColumns(columns);
        stack.setCursorPlacement(CursorPlacement::Below);
        section.append("a\nb");
        static_cast<void>(terminal.drain());

        // Act
        section.overwrite("c");

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[?2026h\r\x1b[2A\x1b[0Jc\r\n\x1b[?2026l");
    }

    TEST(SectionStackTest, MovesTheCursorWhenThePlacementChanges)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        SectionStack stack{output};
        Section& section = stack.createSection();

        terminal.setColumns(columns);
        section.append("a\nb");
        static_cast<void>(terminal.drain());

        // Act
        stack.setCursorPlacement(CursorPlacement::Below);

        // Assert
        EXPECT_EQ(terminal.drain(), "\x1b[2B");
    }

    TEST(SectionStackTest, LeavesTheCursorBelowAtTheEndWhenItIsThereAlready)
    {
        // Arrange
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, true, Capabilities{}};
        std::optional<SectionStack> stack{std::in_place, output};

        terminal.setColumns(columns);
        stack->setCursorPlacement(CursorPlacement::Below);
        stack->createSection().append("a\nb");
        static_cast<void>(terminal.drain());

        // Act
        stack.reset();

        // Assert
        EXPECT_EQ(terminal.drain(), "");
    }
} // namespace Consolpp::Tests::Integration::Screen
