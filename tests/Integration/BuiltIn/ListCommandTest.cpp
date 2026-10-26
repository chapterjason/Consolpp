#include "Consolpp/BuiltIn/ListCommand.hpp"

#include "../Stream/Channel.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Completion/ValueTarget.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/IsSuggestion.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/Parser.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::BuiltIn
{
    using Consolpp::BuiltIn::ListCommand;
    using Consolpp::Parsing::Parameters;
    using Consolpp::Parsing::Parser;
    using Consolpp::Parsing::TokenRange;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;

    namespace
    {
        constexpr std::array<std::string_view, 1> aliases{"d"};
        constexpr std::array<DefinitionView, 2> definitions{
            {
                {.name = "deploy", .description = "Deploys the files", .aliases = aliases},
                {.name = "cache:clear", .description = "Clears the cache"},
            },
        };
        constexpr std::array<std::string_view, 3> names{"cache:clear", "d", "deploy"};
        constexpr CommandList commands{
            .definitions = definitions,
            .names = names,
            .isSuggestion = &Consolpp::isSuggestion<names.front().size()>,
        };
    } // namespace

    TEST(ListCommandTest, ListsTheCommandsAsText)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::span<const char* const> arguments{};
        const Parameters<ListCommand::definition> parameters = Parser<ListCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const int exitCode = ListCommand::run(parameters, output, commands);

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_NE(channel.drain().find("  deploy       [d] Deploys the files\n"), std::string::npos);
    }

    TEST(ListCommandTest, ListsTheCommandsOfANamespace)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"cache"};
        const Parameters<ListCommand::definition> parameters = Parser<ListCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const int exitCode = ListCommand::run(parameters, output, commands);

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_NE(
            channel.drain().find("Available commands for the \"cache\" namespace:\n  cache:clear  Clears the cache\n"),
            std::string::npos);
    }

    TEST(ListCommandTest, ListsTheCommandsAsJson)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"--format=json"};
        const Parameters<ListCommand::definition> parameters = Parser<ListCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const int exitCode = ListCommand::run(parameters, output, commands);

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_TRUE(channel.drain().starts_with(R"({"namespace":null,"definitions":[{"name":"deploy")"));
    }

    TEST(ListCommandTest, ListsTheCommandsAsMarkdown)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"--format=md"};
        const Parameters<ListCommand::definition> parameters = Parser<ListCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const int exitCode = ListCommand::run(parameters, output, commands);

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_EQ(channel.drain(),
                  "- `deploy` (alias `d`): Deploys the files\n\n### cache\n\n- `cache:clear`: Clears the cache\n");
    }

    TEST(ListCommandTest, ThrowsForAnUnknownFormat)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"--format=xml"};
        const Parameters<ListCommand::definition> parameters = Parser<ListCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const auto run = [&parameters, &output] {
            static_cast<void>(ListCommand::run(parameters, output, commands));
        };

        // Assert
        EXPECT_THROW(run(), std::invalid_argument);
    }

    TEST(ListCommandTest, CompletesTheNamespacesOfTheVisibleNames)
    {
        // Arrange
        const Completion::Request request{
            .target = Completion::ValueTarget::Argument,
            .name = "namespace",
            .value = "",
        };

        // Act
        const std::vector<std::string> namespaces = ListCommand::complete(request, commands);

        // Assert
        EXPECT_EQ(namespaces, (std::vector<std::string>{"cache"}));
    }
} // namespace Consolpp::Tests::Integration::BuiltIn
