#include "Consolpp/BuiltIn/ManCommand.hpp"

#include "../Stream/Channel.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/IsSuggestion.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/Parser.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <cerrno>
#include <fstream>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <system_error>

#include <gtest/gtest.h>
#include <stdlib.h> // NOLINT(modernize-deprecated-headers)
#include <unistd.h>

namespace Consolpp::Tests::Integration::BuiltIn
{
    using Consolpp::BuiltIn::ManCommand;
    using Consolpp::Parsing::Parameters;
    using Consolpp::Parsing::Parser;
    using Consolpp::Parsing::TokenRange;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    namespace
    {
        constexpr std::array<DefinitionView, 1> definitions{
            {
                {.name = "deploy", .description = "Deploys the files"},
            },
        };
        constexpr std::array<std::string_view, 1> names{"deploy"};
        constexpr CommandList commands{
            .definitions = definitions,
            .names = names,
            .isSuggestion = &Consolpp::isSuggestion<names.front().size()>,
        };
        constexpr Program program{.name = "tool", .version = "1.2.3"};
    } // namespace

    TEST(ManCommandTest, WritesThePageToTheOutput)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        const Parameters<ManCommand::definition> parameters = Parser<ManCommand::definition>{
            TokenRange{std::span<const char* const>{}}}.parse();

        // Act
        const int exitCode = ManCommand::run(parameters, output, commands, program);

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_TRUE(channel.drain().starts_with(".TH \"TOOL\" \"1\" \"\" \"tool 1.2.3\" \"User Commands\"\n"));
    }

    TEST(ManCommandTest, WritesThePageToTheFile)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        std::string path{"/tmp/consolpp-man-XXXXXX"};
        const int created = mkstemp(path.data());

        if (created < 0) {
            throw std::system_error{errno, std::generic_category()};
        }

        const std::array<const char*, 1> arguments{path.c_str()};
        const Parameters<ManCommand::definition> parameters = Parser<ManCommand::definition>{
            TokenRange{arguments}}.parse();

        if (close(created) != 0) {
            throw std::system_error{errno, std::generic_category()};
        }

        // Act
        static_cast<void>(ManCommand::run(parameters, output, commands, program));

        // Assert
        std::ifstream file{path};
        const std::string page{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
        unlink(path.c_str());
        EXPECT_TRUE(page.starts_with(".TH \"TOOL\""));
        EXPECT_EQ(channel.drain(), "");
    }
} // namespace Consolpp::Tests::Integration::BuiltIn
