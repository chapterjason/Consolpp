#include "Consolpp/BuiltIn/CompletionCommand.hpp"

#include "../Stream/Channel.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/Parser.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::BuiltIn
{
    using Consolpp::BuiltIn::CompletionCommand;
    using Consolpp::Parsing::Parameters;
    using Consolpp::Parsing::Parser;
    using Consolpp::Parsing::TokenRange;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    namespace
    {
        constexpr Program program{.name = "tool"};
    } // namespace

    TEST(CompletionCommandTest, WritesTheBashScriptOfTheProgram)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"bash"};
        const Parameters<CompletionCommand::definition> parameters = Parser<CompletionCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const int exitCode = CompletionCommand::run(parameters, output, program);

        // Assert
        const std::string script = channel.drain();
        EXPECT_EQ(exitCode, 0);
        EXPECT_NE(script.find("\n_tool_complete()\n"), std::string::npos);
        EXPECT_TRUE(script.ends_with("complete -F _tool_complete 'tool'\n"));
    }

    TEST(CompletionCommandTest, ThrowsForAnUnknownShell)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"tcsh"};
        const Parameters<CompletionCommand::definition> parameters = Parser<CompletionCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const auto run = [&parameters, &output] {
            static_cast<void>(CompletionCommand::run(parameters, output, program));
        };

        // Assert
        EXPECT_THROW(run(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Integration::BuiltIn
