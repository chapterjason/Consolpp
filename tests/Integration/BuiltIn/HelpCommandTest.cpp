#include "Consolpp/BuiltIn/HelpCommand.hpp"

#include "../Stream/Channel.hpp"
#include "Consolpp/CommandError.hpp"
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
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::BuiltIn
{
    using Consolpp::BuiltIn::HelpCommand;
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

    TEST(HelpCommandTest, DescribesTheNamedCommand)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"d"};
        const Parameters<HelpCommand::definition> parameters = Parser<HelpCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const int exitCode = HelpCommand::run(parameters, output, commands);

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_TRUE(channel.drain().starts_with("Description:\n  Deploys the files\n"));
    }

    TEST(HelpCommandTest, DescribesTheCommandAsJson)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 2> arguments{"deploy", "--format=json"};
        const Parameters<HelpCommand::definition> parameters = Parser<HelpCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const int exitCode = HelpCommand::run(parameters, output, commands);

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_TRUE(channel.drain().starts_with(R"({"definition":{"name":"deploy")"));
    }

    TEST(HelpCommandTest, DescribesTheCommandAsMarkdown)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 2> arguments{"deploy", "--format=md"};
        const Parameters<HelpCommand::definition> parameters = Parser<HelpCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const int exitCode = HelpCommand::run(parameters, output, commands);

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_TRUE(channel.drain().starts_with("## `deploy`\n\nDeploys the files\n"));
    }

    TEST(HelpCommandTest, ThrowsForAnUnknownCommand)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"bogus"};
        const Parameters<HelpCommand::definition> parameters = Parser<HelpCommand::definition>{
            TokenRange{arguments}}.parse();

        // Act
        const auto run = [&parameters, &output] {
            static_cast<void>(HelpCommand::run(parameters, output, commands));
        };

        // Assert
        EXPECT_THROW(run(), CommandError);
    }

    TEST(HelpCommandTest, SuggestsSimilarNamesForAnUnknownCommand)
    {
        // Arrange
        const Channel channel;
        OutputStream output{channel.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        constexpr std::array<const char*, 1> arguments{"deplyo"};
        const Parameters<HelpCommand::definition> parameters = Parser<HelpCommand::definition>{
            TokenRange{arguments}}.parse();
        const std::vector<std::string_view> expected{"deploy"};
        std::vector<std::string_view> suggestions;

        // Act
        try {
            static_cast<void>(HelpCommand::run(parameters, output, commands));
        } catch (const CommandError& error) {
            for (const std::string_view suggestion : error.createSuggestionRange()) {
                suggestions.push_back(suggestion);
            }
        }

        // Assert
        EXPECT_EQ(suggestions, expected);
    }

    TEST(HelpCommandTest, CompletesTheVisibleNamesAndAliases)
    {
        // Arrange
        const Completion::Request request{.target = Completion::ValueTarget::Argument, .name = "command", .value = ""};

        // Act
        const std::vector<std::string> completed = HelpCommand::complete(request, commands);

        // Assert
        EXPECT_EQ(completed, (std::vector<std::string>{"cache:clear", "d", "deploy"}));
    }
} // namespace Consolpp::Tests::Integration::BuiltIn
