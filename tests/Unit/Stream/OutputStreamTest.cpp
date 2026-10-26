#include "Consolpp/Stream/OutputStream.hpp"

#include "Consolpp/Terminal/Capabilities.hpp"

#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Stream
{
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;

    TEST(OutputStreamTest, WritesIntoTheTargetAtOnce)
    {
        // Arrange
        std::string target;
        OutputStream output{target};

        // Act
        output.write("{} files", 3);

        // Assert
        EXPECT_EQ(target, "3 files");
    }

    TEST(OutputStreamTest, WritesMarkupWithoutStylesIntoAnUndecoratedTarget)
    {
        // Arrange
        std::string target;
        OutputStream output{target};

        // Act
        output << "<info>done</info>\n";

        // Assert
        EXPECT_EQ(target, "done\n");
    }

    TEST(OutputStreamTest, WritesTheStylesIntoADecoratedTarget)
    {
        // Arrange
        std::string target;
        OutputStream output{target, true, Capabilities{}};

        // Act
        output << "<info>done</info>";

        // Assert
        EXPECT_NE(target.find("\x1b["), std::string::npos);
        EXPECT_NE(target.find("done"), std::string::npos);
    }

    TEST(OutputStreamTest, IsNoTerminalForATarget)
    {
        // Arrange
        std::string target;

        // Act
        const OutputStream output{target};

        // Assert
        EXPECT_FALSE(output.isTerminal());
        EXPECT_FALSE(output.isDecorated());
    }
} // namespace Consolpp::Tests::Unit::Stream
