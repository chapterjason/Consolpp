#include "Consolpp/CommandManager.hpp"

#include "../Stream/Channel.hpp"
#include "../Stream/PseudoTerminal.hpp"
#include "Consolpp/Argument.hpp"
#include "Consolpp/CommandError.hpp"
#include "Consolpp/CommandErrorKind.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/ParseError.hpp"
#include "Consolpp/Parsing/ParseErrorKind.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <array>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Running
{
    using Consolpp::Parsing::Parameters;
    using Consolpp::Stream::ErrorOutputStream;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::Channel;
    using Integration::Stream::PseudoTerminal;

    namespace
    {
        constexpr int deployExitCode = 3;
        constexpr int statusExitCode = 7;

        class DeployCommand
        {
            private:
                int exitCode{deployExitCode};

            public:
                static constexpr Definition definition{
                    .name = "deploy",
                    .aliases = std::array<std::string_view, 1>{"d"},
                    .signature =
                        Signature{
                            std::array{Option{.name = "force", .shortcut = 'f'}},
                            std::array{Argument{.name = "source"}},
                        },
                };

                [[nodiscard]]
                int run(const Parameters<definition>& parameters, Consolpp::Stream::OutputStream& output) const
                {
                    output << "deploy " << parameters.getArgument<"source">();

                    if (parameters.getOption<"force">().value_or(false)) {
                        output << " forced";
                    }

                    return this->exitCode;
                }
        };

        class StatusCommand
        {
            private:
                int exitCode{statusExitCode};

            public:
                static constexpr Definition definition{
                    .name = "status",
                    .signature = Signature{std::array<Option, 0>{}, std::array<Argument, 0>{}},
                };

                [[nodiscard]]
                int run(Consolpp::Stream::ErrorOutputStream& errorOutput) const
                {
                    errorOutput << "status";

                    return this->exitCode;
                }
        };

        class DeployAllCommand
        {
            private:
                int exitCode{0};

            public:
                static constexpr Definition definition{
                    .name = "deploy:all",
                    .aliases = std::array<std::string_view, 1>{"deploy-all"},
                    .signature = Signature{std::array<Option, 0>{}, std::array<Argument, 0>{}},
                };

                [[nodiscard]]
                int run() const
                {
                    return this->exitCode;
                }
        };

        class HiddenDeployCommand
        {
            private:
                int exitCode{0};

            public:
                static constexpr Definition definition{
                    .name = "deploy:hidden",
                    .hidden = true,
                    .signature = Signature{std::array<Option, 0>{}, std::array<Argument, 0>{}},
                };

                [[nodiscard]]
                int run() const
                {
                    return this->exitCode;
                }
        };

        class InteractiveDeployCommand
        {
            public:
                static constexpr Definition definition{
                    .name = "deploy:interactive",
                    .signature = Signature{std::array<Option, 0>{}, std::array{Argument{.name = "source"}}},
                };

                static void interact(Parameters<definition>& parameters)
                {
                    parameters.setArgument<"source">("set in interact");
                }

                [[nodiscard]]
                static int run(const Parameters<definition>& parameters, Consolpp::Stream::OutputStream& output)
                {
                    output << "deploy " << parameters.getArgument<"source">();

                    return 0;
                }
        };

        class CopyCommand
        {
            public:
                static constexpr Definition definition{
                    .name = "copy",
                    .signature =
                        Signature{
                            std::array<Option, 0>{},
                            std::array{Argument{.name = "files", .variadic = true, .description = "File to copy"}},
                        },
                };

                [[nodiscard]]
                static int run(const Parameters<definition>& parameters, Consolpp::Stream::OutputStream& output)
                {
                    output << "copy";

                    for (const std::string_view file : parameters.getArgument<"files">()) {
                        output << ' ' << file;
                    }

                    return 0;
                }
        };

        class ReleaseCommand
        {
            private:
                std::vector<std::string> environments{"production", "staging"};

            public:
                static constexpr Definition definition{
                    .name = "release",
                    .signature =
                        Signature{
                            std::array<Option, 0>{},
                            std::array{Argument{.name = "environment", .completion = ValueCompletion::Computed}},
                        },
                };

                [[nodiscard]]
                static int run()
                {
                    return 0;
                }

                [[nodiscard]]
                std::vector<std::string> complete(const Completion::Request& /*request*/) const
                {
                    return this->environments;
                }
        };

        using CaughtCommandError = std::optional<std::tuple<CommandErrorKind, std::string>>;
    } // namespace

    TEST(CommandManagerTest, RunsTheNamedCommandWithItsParsedParameters)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).addCommand(status);
        constexpr std::array<const char*, 3> arguments{"deploy", "a.txt", "--force"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, deployExitCode);
        EXPECT_EQ(standardOutput.drain(), "deploy a.txt forced");
    }

    TEST(CommandManagerTest, RunsACommandByItsAlias)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).addCommand(status);
        constexpr std::array<const char*, 2> arguments{"d", "a.txt"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, deployExitCode);
        EXPECT_EQ(standardOutput.drain(), "deploy a.txt");
    }

    TEST(CommandManagerTest, PassesTheErrorOutputToTheCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).addCommand(status);
        constexpr std::array<const char*, 1> arguments{"status"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, statusExitCode);
        EXPECT_EQ(standardError.drain(), "status");
    }

    TEST(CommandManagerTest, ThrowsForAnUnknownCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 1> arguments{"bogus"};
        const CaughtCommandError expected{std::in_place, CommandErrorKind::UnknownCommand, "bogus"};
        CaughtCommandError caught;

        // Act
        try {
            static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));
        } catch (const CommandError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()});
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(CommandManagerTest, SuggestsVisibleNamesAndAliasesInNaturalOrderForAnUnknownCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployAllCommand deployAll;
        DeployCommand deploy;
        StatusCommand status;
        HiddenDeployCommand hiddenDeploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deployAll)
                                 .addCommand(deploy)
                                 .addCommand(status)
                                 .addCommand(hiddenDeploy);
        constexpr std::array<const char*, 1> arguments{"deplo"};
        const std::vector<std::string_view> expected{"deploy", "deploy-all", "deploy:all"};
        std::vector<std::string_view> suggestions;

        // Act
        try {
            static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));
        } catch (const CommandError& error) {
            for (const std::string_view suggestion : error.createSuggestionRange()) {
                suggestions.push_back(suggestion);
            }
        }

        // Assert
        EXPECT_EQ(suggestions, expected);
    }

    TEST(CommandManagerTest, RunsListWithoutACommandName)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{std::span<const char* const>{}});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_NE(standardOutput.drain().find("Available commands:"), std::string::npos);
    }

    TEST(CommandManagerTest, RunsListForAHelpOptionWithoutACommandName)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 1> arguments{"--help"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_NE(standardOutput.drain().find("Available commands:"), std::string::npos);
    }

    TEST(CommandManagerTest, RunsHelpForTheCommandInsteadOfTheCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 2> arguments{"d", "-h"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_TRUE(standardOutput.drain().starts_with("Usage:\n  deploy [options] [--] <source>"));
    }

    TEST(CommandManagerTest, RunsHelpForAHelpShortcutInABundle)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 2> arguments{"deploy", "-fh"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_TRUE(standardOutput.drain().starts_with("Usage:\n  deploy [options] [--] <source>"));
    }

    TEST(CommandManagerTest, PassesAHelpOptionAfterTheSeparatorToTheCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 3> arguments{"deploy", "--", "--help"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, deployExitCode);
        EXPECT_EQ(standardOutput.drain(), "deploy --help");
    }

    TEST(CommandManagerTest, ThrowsForTheHelpOfAnUnknownCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 2> arguments{"help", "bogus"};
        const CaughtCommandError expected{std::in_place, CommandErrorKind::UnknownCommand, "bogus"};
        CaughtCommandError caught;

        // Act
        try {
            static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));
        } catch (const CommandError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()});
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(CommandManagerTest, LetsParseErrorsThrough)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 1> arguments{"deploy"};
        std::optional<Parsing::ParseErrorKind> kind;

        // Act
        try {
            static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));
        } catch (const Parsing::ParseError& error) {
            kind = error.getKind();
        }

        // Assert
        EXPECT_EQ(kind, std::optional<Parsing::ParseErrorKind>{Parsing::ParseErrorKind::MissingArgument});
    }

    TEST(CommandManagerTest, CallsInteractBeforeRun)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        InteractiveDeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 1> arguments{"deploy:interactive"};

        input.enableInteraction();

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "deploy set in interact");
    }

    TEST(CommandManagerTest, SkipsInteractWhileInteractionIsDisabled)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        InteractiveDeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 1> arguments{"deploy:interactive"};

        input.disableInteraction();

        // Act
        const auto run = [&manager, &arguments] {
            static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));
        };

        // Assert
        EXPECT_THROW(run(), Parsing::ParseError);
    }

    TEST(CommandManagerTest, AsksForAMissingArgument)
    {
        // Arrange
        const PseudoTerminal terminal;
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        Consolpp::Stream::InputStream input{terminal.getTerminal(), output, loop};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 1> arguments{"deploy"};

        terminal.switchToRaw();
        terminal.type("\ra.txt\r");

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_NE(standardOutput.drain().find("deploy a.txt"), std::string::npos);
    }

    TEST(CommandManagerTest, AsksTheVariadicArgumentValueByValue)
    {
        // Arrange
        const PseudoTerminal terminal;
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        Consolpp::Stream::InputStream input{terminal.getTerminal(), output, loop};
        CopyCommand copy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(copy);
        constexpr std::array<const char*, 1> arguments{"copy"};

        terminal.switchToRaw();
        terminal.type("a\rb\r\r");

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        const std::string written = standardOutput.drain();
        EXPECT_NE(written.find(" files:"), std::string::npos);
        EXPECT_NE(written.find("copy a b"), std::string::npos);
    }

    TEST(CommandManagerTest, RunsTheDefaultCommandWithoutACommandName)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .addCommand(status)
                                 .setDefaultCommand<StatusCommand>();
        constexpr std::array<const char*, 0> arguments{};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardError.drain(), "status");
    }

    TEST(CommandManagerTest, RunsOtherCommandsBesideTheDefaultCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .addCommand(status)
                                 .setDefaultCommand<StatusCommand>();
        constexpr std::array<const char*, 2> arguments{"deploy", "a.txt"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "deploy a.txt");
    }

    TEST(CommandManagerTest, ShowsTheHelpOfTheDefaultCommandForHelpWithoutACommandName)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .addCommand(status)
                                 .setDefaultCommand<DeployCommand>();
        constexpr std::array<const char*, 1> arguments{"--help"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_NE(standardOutput.drain().find("Usage:"), std::string::npos);
    }

    TEST(CommandManagerTest, GivesAllOfArgvToTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .addCommand(status)
                                 .setDefaultCommand<DeployCommand>(true);
        constexpr std::array<const char*, 2> arguments{"status", "--force"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "deploy status forced");
    }

    TEST(CommandManagerTest, ShowsTheHelpOfTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .addCommand(status)
                                 .setDefaultCommand<DeployCommand>(true);
        constexpr std::array<const char*, 2> arguments{"a.txt", "-h"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_NE(standardOutput.drain().find("Usage:"), std::string::npos);
    }

    TEST(CommandManagerTest, TakesGlobalOptionsBeforeTheCommandName)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 3> arguments{"-q", "deploy", "a.txt"};

        input.enableInteraction();

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "");
        EXPECT_TRUE(output.isQuiet());
        EXPECT_TRUE(errorOutput.isQuiet());
        EXPECT_FALSE(input.isInteractive());
    }

    TEST(CommandManagerTest, TakesGlobalOptionsAmongTheArguments)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 4> arguments{"deploy", "a.txt", "-vvv", "-n"};

        input.enableInteraction();

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "deploy a.txt");
        EXPECT_TRUE(output.isDebug());
        EXPECT_FALSE(input.isInteractive());
    }

    TEST(CommandManagerTest, NamesTheProgramAfterArgvZero)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 3> argv{"/usr/local/bin/tool", "man", nullptr};

        // Act
        static_cast<void>(manager.run(argv.data()));

        // Assert
        output.flush();
        EXPECT_TRUE(standardOutput.drain().starts_with(".TH \"TOOL\" \"1\" \"\" \"tool\" \"User Commands\"\n"));
    }

    TEST(CommandManagerTest, TakesTheSetNameAndVersionOverArgvZero)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Buffered, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setName("app")
                                 .setVersion("2.0");
        constexpr std::array<const char*, 3> argv{"/usr/local/bin/tool", "man", nullptr};

        // Act
        static_cast<void>(manager.run(argv.data()));

        // Assert
        output.flush();
        EXPECT_TRUE(standardOutput.drain().starts_with(".TH \"APP\" \"1\" \"\" \"app 2.0\" \"User Commands\"\n"));
    }

    TEST(CommandManagerTest, PrintsTheNameAndVersionForAVersionOptionWithoutACommandName)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setName("app")
                                 .setVersion("2.0");
        constexpr std::array<const char*, 1> arguments{"--version"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_EQ(standardOutput.drain(), "app 2.0\n");
    }

    TEST(CommandManagerTest, PrintsTheVersionInsteadOfRunningTheCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setName("app")
                                 .setVersion("2.0");
        constexpr std::array<const char*, 2> arguments{"d", "--version"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_EQ(standardOutput.drain(), "app 2.0\n");
    }

    TEST(CommandManagerTest, PassesAVersionOptionAfterTheSeparatorToTheCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setName("app")
                                 .setVersion("2.0");
        constexpr std::array<const char*, 3> arguments{"deploy", "--", "--version"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, deployExitCode);
        EXPECT_EQ(standardOutput.drain(), "deploy --version");
    }

    TEST(CommandManagerTest, PrintsTheVersionInsteadOfRunningTheDefaultCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setDefaultCommand<DeployCommand>()
                                 .setName("app")
                                 .setVersion("2.0");
        constexpr std::array<const char*, 1> arguments{"--version"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_EQ(standardOutput.drain(), "app 2.0\n");
    }

    TEST(CommandManagerTest, PrintsTheVersionInsteadOfRunningTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setDefaultCommand<DeployCommand>(true)
                                 .setName("app")
                                 .setVersion("2.0");
        constexpr std::array<const char*, 2> arguments{"a.txt", "--version"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_EQ(standardOutput.drain(), "app 2.0\n");
    }

    TEST(CommandManagerTest, PrintsOnlyTheNameForAVersionOptionWithoutAVersion)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).setName("app");
        constexpr std::array<const char*, 1> arguments{"--version"};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_EQ(standardOutput.drain(), "app\n");
    }

    TEST(CommandManagerTest, AnswersACompletionRequestWithOneSuggestionPerLine)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).addCommand(status);
        constexpr std::array<const char*, 4> arguments{"_complete", "0", "--", ""};

        // Act
        const int exitCode = manager.run(Consolpp::Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_EQ(standardOutput.drain(), "words\nd\ndeploy\nhelp\nlist\nstatus\n");
    }

    TEST(CommandManagerTest, CompletesTheOptionsOfTheNamedCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 5> arguments{"_complete", "1", "--", "deploy", "--fo"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "words\n--force\n");
    }

    TEST(CommandManagerTest, CompletesTheWordsOfTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        StatusCommand status;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .addCommand(status)
                                 .setDefaultCommand<DeployCommand>(true);
        constexpr std::array<const char*, 5> arguments{"_complete", "1", "--", "status", "--fo"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "words\n--force\n");
    }

    TEST(CommandManagerTest, ThrowsForACompletionRequestWithoutTheSeparator)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 3> arguments{"_complete", "0", "deploy"};

        // Act
        const auto run = [&manager, &arguments] {
            static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));
        };

        // Assert
        EXPECT_THROW(run(), std::invalid_argument);
    }

    TEST(CommandManagerTest, ThrowsForACompletionRequestWithACurrentWordThatIsNoNumber)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy);
        constexpr std::array<const char*, 4> arguments{"_complete", "1x", "--", "deploy"};

        // Act
        const auto run = [&manager, &arguments] {
            static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));
        };

        // Assert
        EXPECT_THROW(run(), std::invalid_argument);
    }

    TEST(CommandManagerTest, PrintsTheCompletionScriptForTheProgram)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).setName("app");
        constexpr std::array<const char*, 2> arguments{"completion", "bash"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        const std::string script = standardOutput.drain();
        EXPECT_NE(script.find("\n_app_complete()\n"), std::string::npos);
        EXPECT_TRUE(script.ends_with("complete -F _app_complete 'app'\n"));
    }

    TEST(CommandManagerTest, PrintsTheCompletionScriptForTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setDefaultCommand<DeployCommand>(true)
                                 .setName("app");
        constexpr std::array<const char*, 2> arguments{"completion", "bash"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_NE(standardOutput.drain().find("\n_app_complete()\n"), std::string::npos);
    }

    TEST(CommandManagerTest, ShowsTheHelpOfCompletionBesideTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setDefaultCommand<DeployCommand>(true)
                                 .setName("app");
        constexpr std::array<const char*, 2> arguments{"completion", "--help"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_NE(standardOutput.drain().find("Prints the completion script of the program for the shell"),
                  std::string::npos);
    }

    TEST(CommandManagerTest, GivesCompletionAfterTheSeparatorToTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setDefaultCommand<DeployCommand>(true)
                                 .setName("app");
        constexpr std::array<const char*, 2> arguments{"--", "completion"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "deploy completion");
    }

    TEST(CommandManagerTest, PrintsTheManPageForTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setDefaultCommand<DeployCommand>(true)
                                 .setName("app");
        constexpr std::array<const char*, 1> arguments{"man"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_TRUE(standardOutput.drain().starts_with(".TH \"APP\" \"1\" \"\" \"app\" \"User Commands\"\n"));
    }

    TEST(CommandManagerTest, ShowsTheHelpOfManBesideTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setDefaultCommand<DeployCommand>(true)
                                 .setName("app");
        constexpr std::array<const char*, 2> arguments{"man", "--help"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_NE(standardOutput.drain().find("Generates the man page of the program"), std::string::npos);
    }

    TEST(CommandManagerTest, GivesManAfterTheSeparatorToTheSingleCommand)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        const auto manager = CommandManager{input, output, errorOutput, loop}
                                 .addCommand(deploy)
                                 .setDefaultCommand<DeployCommand>(true)
                                 .setName("app");
        constexpr std::array<const char*, 2> arguments{"--", "man"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "deploy man");
    }

    TEST(CommandManagerTest, CompletesTheValuesTheNamedCommandComputes)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        ReleaseCommand release;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).addCommand(release);
        constexpr std::array<const char*, 5> arguments{"_complete", "1", "--", "release", "p"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "words\nproduction\n");
    }

    TEST(CommandManagerTest, CompletesTheCommandNamesForHelp)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        ReleaseCommand release;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).addCommand(release);
        constexpr std::array<const char*, 5> arguments{"_complete", "1", "--", "help", "de"};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "words\ndeploy\n");
    }

    TEST(CommandManagerTest, LeavesTheFileOfManToTheShell)
    {
        // Arrange
        const Channel standardOutput;
        const Channel standardError;
        Event::Loop loop;
        Consolpp::Stream::InputStream input{-1};
        OutputStream output{standardOutput.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        ErrorOutputStream errorOutput{standardError.getWriteEnd(), FlushMode::Immediate, false, Capabilities{}};
        DeployCommand deploy;
        ReleaseCommand release;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(deploy).addCommand(release);
        constexpr std::array<const char*, 5> arguments{"_complete", "1", "--", "man", ""};

        // Act
        static_cast<void>(manager.run(Consolpp::Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(standardOutput.drain(), "files\n\n");
    }
} // namespace Consolpp::Tests::Integration::Running
