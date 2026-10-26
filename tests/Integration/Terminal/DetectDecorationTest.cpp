#include "Consolpp/Terminal/DetectDecoration.hpp"

#include "../EnvironmentScope.hpp"
#include "../Stream/Channel.hpp"

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Terminal
{
    using Consolpp::Terminal::detectDecoration;
    using Integration::Stream::Channel;

    TEST(DetectDecorationTest, LeavesASocketUndecorated)
    {
        // Arrange
        const Channel channel;
        const EnvironmentScope environment{{}};

        // Act
        const bool decorated = detectDecoration(channel.getWriteEnd());

        // Assert
        EXPECT_FALSE(decorated);
    }

    TEST(DetectDecorationTest, DecoratesASocketWithForceColor)
    {
        // Arrange
        const Channel channel;
        const EnvironmentScope environment{{{"FORCE_COLOR", "1"}}};

        // Act
        const bool decorated = detectDecoration(channel.getWriteEnd());

        // Assert
        EXPECT_TRUE(decorated);
    }
} // namespace Consolpp::Tests::Integration::Terminal
