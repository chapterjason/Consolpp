#include "../Stream/PseudoTerminal.hpp"
#include "Consolpp/Completion/FormatBashScript.hpp"

#include <array>
#include <cerrno>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <ios>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include <gtest/gtest.h>
#include <signal.h> // NOLINT(modernize-deprecated-headers)
#include <spawn.h>
#include <stdlib.h> // NOLINT(modernize-deprecated-headers)
#include <sys/wait.h>
#include <unistd.h>

namespace Consolpp::Tests::Integration::Completion
{
    using Consolpp::Completion::formatBashScript;
    using Integration::Stream::PseudoTerminal;

    namespace
    {
        constexpr std::string_view prompt{"ready> "};
        constexpr int terminalReads = 10;
        // The call is renamed into place once written, so that it is never read half written.
        constexpr std::string_view standIn{R"(#!/bin/sh
directory=$(dirname "$0")

if [ "$1" = _complete ]; then
    printf '%s\n' "$@" > "$directory/request"
    cat "$directory/answer"
else
    printf '%s\n' "$@" > "$directory/call.tmp" && mv "$directory/call.tmp" "$directory/call"
fi
)"};

        // The script, a stand-in for the program named tool, and the files the stand-in reads and writes. The
        // stand-in answers a completion request with the set answer and keeps the arguments of every call.
        class Workspace
        {
            private:
                std::filesystem::path directory;

                void write(const std::string_view name, const std::string_view content) const
                {
                    std::ofstream file{this->directory / name};

                    file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
                    file << content;
                    file.close();
                }

                static std::filesystem::path createDirectory()
                {
                    std::string pattern{"/tmp/consolpp-completion-XXXXXX"};

                    if (mkdtemp(pattern.data()) == nullptr) {
                        throw std::system_error{errno, std::generic_category()};
                    }

                    return pattern;
                }

            public:
                Workspace()
                    : directory{Workspace::createDirectory()}
                {
                    std::string script;

                    formatBashScript("tool", std::back_inserter(script));
                    this->write("script", script);
                    this->write("tool", standIn);
                    std::filesystem::permissions(this->directory / "tool", std::filesystem::perms::owner_all);
                }

                Workspace(const Workspace&) = delete;
                Workspace(Workspace&&) = delete;
                Workspace& operator=(const Workspace&) = delete;
                Workspace& operator=(Workspace&&) = delete;

                ~Workspace()
                {
                    std::error_code ignored;

                    std::filesystem::remove_all(this->directory, ignored);
                }

                [[nodiscard]]
                std::string getPath() const
                {
                    return this->directory.string();
                }

                void setAnswer(const std::string_view answer) const
                {
                    this->write("answer", answer);
                }

                void addFile(const std::string_view name) const
                {
                    this->write(name, "");
                }

                void addDirectory(const std::string_view name) const
                {
                    std::filesystem::create_directory(this->directory / name);
                }

                [[nodiscard]]
                bool exists(const std::string_view name) const
                {
                    return std::filesystem::exists(this->directory / name);
                }

                // Empty for a file that does not exist.
                [[nodiscard]]
                std::string read(const std::string_view name) const
                {
                    const std::ifstream file{this->directory / name};
                    std::stringstream content;

                    content << file.rdbuf();

                    return content.str();
                }
        };

        // An interactive Bash in the pseudo terminal, without the startup files and the inputrc of the user.
        class Shell
        {
            private:
                pid_t process{-1};

            public:
                Shell(const PseudoTerminal& terminal, const std::string& directory)
                {
                    std::array<std::string, 4> arguments{"bash", "--norc", "--noprofile", "-i"};
                    auto environment = std::to_array<std::string>({
                        "PATH=" + directory + ":/usr/bin:/bin",
                        "HOME=" + directory,
                        "HISTFILE=/dev/null",
                        "TERM=dumb",
                        "INPUTRC=/dev/null",
                        "PS1=" + std::string{prompt},
                        "BASH_SILENCE_DEPRECATION_WARNING=1",
                    });
                    std::vector<char*> argumentPointers;
                    std::vector<char*> environmentPointers;
                    posix_spawn_file_actions_t actions{};

                    argumentPointers.reserve(arguments.size() + 1);
                    environmentPointers.reserve(environment.size() + 1);

                    for (std::string& argument : arguments) {
                        argumentPointers.push_back(argument.data());
                    }

                    for (std::string& variable : environment) {
                        environmentPointers.push_back(variable.data());
                    }

                    argumentPointers.push_back(nullptr);
                    environmentPointers.push_back(nullptr);

                    const int initialized = posix_spawn_file_actions_init(&actions);

                    if (initialized != 0) {
                        throw std::system_error{initialized, std::generic_category()};
                    }

                    for (const int target : {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO}) {
                        const int added = posix_spawn_file_actions_adddup2(&actions, terminal.getTerminal(), target);

                        if (added != 0) {
                            posix_spawn_file_actions_destroy(&actions);

                            throw std::system_error{added, std::generic_category()};
                        }
                    }

                    const int result = posix_spawnp(&this->process,
                                                    "bash",
                                                    &actions,
                                                    nullptr,
                                                    argumentPointers.data(),
                                                    environmentPointers.data());

                    posix_spawn_file_actions_destroy(&actions);

                    if (result != 0) {
                        throw std::system_error{result, std::generic_category()};
                    }
                }

                Shell(const Shell&) = delete;
                Shell(Shell&&) = delete;
                Shell& operator=(const Shell&) = delete;
                Shell& operator=(Shell&&) = delete;

                ~Shell()
                {
                    kill(this->process, SIGKILL);
                    waitpid(this->process, nullptr, 0);
                }
        };

        // What is typed before the prompt could meet the line discipline instead of readline.
        void waitForPrompt(const PseudoTerminal& terminal)
        {
            std::string received;

            for (int read = 0; read < terminalReads && received.find(prompt) == std::string::npos; ++read) {
                received += terminal.drain();
            }
        }

        // Bash does not go on while its output stays unread, so the terminal is read until the stand-in is called.
        void waitForCall(const PseudoTerminal& terminal, const Workspace& workspace)
        {
            for (int read = 0; read < terminalReads && !workspace.exists("call"); ++read) {
                static_cast<void>(terminal.drain());
            }
        }
    } // namespace

    TEST(BashCompletionTest, CompletesAWordWithAColon)
    {
        // Arrange
        const Workspace workspace;
        const PseudoTerminal terminal;
        const Shell shell{terminal, workspace.getPath()};

        workspace.setAnswer("words\ncache:clear\n");
        waitForPrompt(terminal);
        terminal.type("source " + workspace.getPath() + "/script\n");
        waitForPrompt(terminal);

        // Act
        terminal.type("tool cache:cl\t\n");

        // Assert
        waitForCall(terminal, workspace);
        EXPECT_EQ(workspace.read("call"), "cache:clear\n");
        EXPECT_EQ(workspace.read("request"), "_complete\n0\n--\ncache:cl\n");
    }

    TEST(BashCompletionTest, CompletesAfterAColonAtTheEndOfTheWord)
    {
        // Arrange
        const Workspace workspace;
        const PseudoTerminal terminal;
        const Shell shell{terminal, workspace.getPath()};

        workspace.setAnswer("words\ncache:clear\n");
        waitForPrompt(terminal);
        terminal.type("source " + workspace.getPath() + "/script\n");
        waitForPrompt(terminal);

        // Act
        terminal.type("tool cache:\t\n");

        // Assert
        waitForCall(terminal, workspace);
        EXPECT_EQ(workspace.read("call"), "cache:clear\n");
        EXPECT_EQ(workspace.read("request"), "_complete\n0\n--\ncache:\n");
    }

    TEST(BashCompletionTest, CompletesAnOptionAfterTheCommandName)
    {
        // Arrange
        const Workspace workspace;
        const PseudoTerminal terminal;
        const Shell shell{terminal, workspace.getPath()};

        workspace.setAnswer("words\n--force\n");
        waitForPrompt(terminal);
        terminal.type("source " + workspace.getPath() + "/script\n");
        waitForPrompt(terminal);

        // Act
        terminal.type("tool deploy --fo\t\n");

        // Assert
        waitForCall(terminal, workspace);
        EXPECT_EQ(workspace.read("call"), "deploy\n--force\n");
        EXPECT_EQ(workspace.read("request"), "_complete\n1\n--\ndeploy\n--fo\n");
    }

    TEST(BashCompletionTest, EscapesASuggestedValue)
    {
        // Arrange
        const Workspace workspace;
        const PseudoTerminal terminal;
        const Shell shell{terminal, workspace.getPath()};

        workspace.setAnswer("words\nmy file\n");
        waitForPrompt(terminal);
        terminal.type("source " + workspace.getPath() + "/script\n");
        waitForPrompt(terminal);

        // Act
        terminal.type("tool my\t\n");

        // Assert
        waitForCall(terminal, workspace);
        EXPECT_EQ(workspace.read("call"), "my file\n");
    }

    TEST(BashCompletionTest, SendsTheTypedWordsWithoutTheirQuoting)
    {
        // Arrange
        const Workspace workspace;
        const PseudoTerminal terminal;
        const Shell shell{terminal, workspace.getPath()};

        workspace.setAnswer("words\nmy file\n");
        waitForPrompt(terminal);
        terminal.type("source " + workspace.getPath() + "/script\n");
        waitForPrompt(terminal);

        // Act
        terminal.type("tool 'my' my\\ f\t\n");

        // Assert
        waitForCall(terminal, workspace);
        EXPECT_EQ(workspace.read("request"), "_complete\n1\n--\nmy\nmy f\n");
        EXPECT_EQ(workspace.read("call"), "my\nmy file\n");
    }

    TEST(BashCompletionTest, CompletesAFileAfterTheValuePrefix)
    {
        // Arrange
        const Workspace workspace;
        const PseudoTerminal terminal;
        const Shell shell{terminal, workspace.getPath()};

        workspace.setAnswer("files\n--output=\n");
        workspace.addFile("src.txt");
        waitForPrompt(terminal);
        terminal.type("source " + workspace.getPath() + "/script\n");
        waitForPrompt(terminal);
        terminal.type("cd " + workspace.getPath() + "\n");
        waitForPrompt(terminal);

        // Act
        terminal.type("tool --output=sr\t\n");

        // Assert
        waitForCall(terminal, workspace);
        EXPECT_EQ(workspace.read("call"), "--output=src.txt\n");
    }

    TEST(BashCompletionTest, CompletesOnlyDirectoriesForADirectory)
    {
        // Arrange
        const Workspace workspace;
        const PseudoTerminal terminal;
        const Shell shell{terminal, workspace.getPath()};

        workspace.setAnswer("directories\n\n");
        workspace.addDirectory("docs");
        workspace.addFile("dots.txt");
        waitForPrompt(terminal);
        terminal.type("source " + workspace.getPath() + "/script\n");
        waitForPrompt(terminal);
        terminal.type("cd " + workspace.getPath() + "\n");
        waitForPrompt(terminal);

        // Act
        terminal.type("tool do\t\n");

        // Assert
        waitForCall(terminal, workspace);
        EXPECT_EQ(workspace.read("call"), "docs/\n");
    }
} // namespace Consolpp::Tests::Integration::Completion
