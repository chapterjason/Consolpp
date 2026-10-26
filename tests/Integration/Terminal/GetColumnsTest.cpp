#include "Consolpp/Terminal/GetColumns.hpp"

#include "../EnvironmentScope.hpp"
#include "../Stream/Channel.hpp"
#include "../Stream/PseudoTerminal.hpp"

#include <cstddef>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Terminal::getColumns;
    using Integration::Stream::Channel;
    using Integration::Stream::PseudoTerminal;

    TEST(GetColumnsTest, QueriesTheColumnsOfATerminal)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "120"}}};
        const PseudoTerminal terminal;
        constexpr unsigned short terminalColumns = 42;

        terminal.setColumns(terminalColumns);

        // Act
        const std::size_t columns = getColumns(terminal.getTerminal());

        // Assert
        EXPECT_EQ(columns, 42U);
    }

    TEST(GetColumnsTest, TakesTheColumnsFromTheEnvironmentWithoutATerminal)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "120"}}};
        const Channel channel;

        // Act
        const std::size_t columns = getColumns(channel.getWriteEnd());

        // Assert
        EXPECT_EQ(columns, 120U);
    }

    TEST(GetColumnsTest, FallsBackTo80WithoutTerminalAndColumns)
    {
        // Arrange
        const EnvironmentScope environment{{}};
        const Channel channel;

        // Act
        const std::size_t columns = getColumns(channel.getWriteEnd());

        // Assert
        EXPECT_EQ(columns, 80U);
    }

    TEST(GetColumnsTest, FallsBackTo80ForInvalidColumns)
    {
        // Arrange
        const EnvironmentScope environment{{{"COLUMNS", "wide"}}};
        const Channel channel;

        // Act
        const std::size_t columns = getColumns(channel.getWriteEnd());

        // Assert
        EXPECT_EQ(columns, 80U);
    }
} // namespace Consolpp::Tests::Integration::Terminal
