#include "Consolpp/Terminal/GetRows.hpp"

#include "../EnvironmentScope.hpp"
#include "../Stream/Channel.hpp"
#include "../Stream/PseudoTerminal.hpp"

#include <cstddef>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Terminal::getRows;
    using Integration::Stream::Channel;
    using Integration::Stream::PseudoTerminal;

    TEST(GetRowsTest, QueriesTheRowsOfATerminal)
    {
        // Arrange
        const EnvironmentScope environment{{{"LINES", "50"}}};
        const PseudoTerminal terminal;
        constexpr unsigned short terminalRows = 42;

        terminal.setRows(terminalRows);

        // Act
        const std::size_t rows = getRows(terminal.getTerminal());

        // Assert
        EXPECT_EQ(rows, 42U);
    }

    TEST(GetRowsTest, TakesTheRowsFromTheEnvironmentWithoutATerminal)
    {
        // Arrange
        const EnvironmentScope environment{{{"LINES", "50"}}};
        const Channel channel;

        // Act
        const std::size_t rows = getRows(channel.getWriteEnd());

        // Assert
        EXPECT_EQ(rows, 50U);
    }

    TEST(GetRowsTest, FallsBackTo24WithoutTerminalAndLines)
    {
        // Arrange
        const EnvironmentScope environment{{}};
        const Channel channel;

        // Act
        const std::size_t rows = getRows(channel.getWriteEnd());

        // Assert
        EXPECT_EQ(rows, 24U);
    }

    TEST(GetRowsTest, FallsBackTo24ForInvalidLines)
    {
        // Arrange
        const EnvironmentScope environment{{{"LINES", "tall"}}};
        const Channel channel;

        // Act
        const std::size_t rows = getRows(channel.getWriteEnd());

        // Assert
        EXPECT_EQ(rows, 24U);
    }
} // namespace Consolpp::Tests::Integration::Terminal
