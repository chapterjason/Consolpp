#include "Consolpp/Argument.hpp"
#include "Consolpp/CommandManager.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Parsing/ParseError.hpp"
#include "Consolpp/Parsing/TokenRange.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <optional>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Functional::Running
{
    using Consolpp::Stream::ErrorOutputStream;
    using Consolpp::Stream::InputStream;
    using Consolpp::Stream::OutputStream;

    namespace
    {
        class CountCommand
        {
            public:
                static constexpr Definition definition{
                    .name = "count",
                    .signature =
                        Signature{
                            std::array<Option, 0>{},
                            std::array{Argument{.name = "label"}},
                        },
                };

                // Writes the label with the number of input lines and ends with that number.
                static int run(const Parsing::Parameters<definition>& parameters,
                               InputStream& input,
                               OutputStream& output,
                               ErrorOutputStream& errorOutput)
                {
                    int lines = 0;

                    while (input.readLine().has_value()) {
                        ++lines;
                    }

                    output.write("<info>{}</info>: {}\n", parameters.getArgument<"label">(), lines);
                    errorOutput.write("counted\n");

                    return lines;
                }
        };
    } // namespace

    TEST(InMemoryTest, RunsACommandWithItsInputAndOutputInMemory)
    {
        // Arrange
        std::string written;
        std::string writtenErrors;
        InputStream input{std::string_view{"a\nb\nc\n"}};
        OutputStream output{written};
        ErrorOutputStream errorOutput{writtenErrors};
        Event::Loop loop;
        CountCommand count;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(count);
        constexpr std::array<const char*, 2> arguments{"count", "lines"};

        // Act
        const int exitCode = manager.run(Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 3);
        EXPECT_EQ(written, "lines: 3\n");
        EXPECT_EQ(writtenErrors, "counted\n");
    }

    TEST(InMemoryTest, AppliesGlobalOptionsInMemory)
    {
        // Arrange
        std::string written;
        std::string writtenErrors;
        InputStream input{std::string_view{}};
        OutputStream output{written};
        ErrorOutputStream errorOutput{writtenErrors};
        Event::Loop loop;
        CountCommand count;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(count);
        constexpr std::array<const char*, 3> arguments{"-q", "count", "lines"};

        // Act
        static_cast<void>(manager.run(Parsing::TokenRange{arguments}));

        // Assert
        EXPECT_EQ(written, "");
    }

    TEST(InMemoryTest, AsksNothingForAMissingArgumentInMemory)
    {
        // Arrange
        std::string written;
        std::string writtenErrors;
        InputStream input{std::string_view{"label\n"}};
        OutputStream output{written};
        ErrorOutputStream errorOutput{writtenErrors};
        Event::Loop loop;
        CountCommand count;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(count);
        constexpr std::array<const char*, 1> arguments{"count"};

        // Act
        const auto run = [&manager, &arguments] {
            static_cast<void>(manager.run(Parsing::TokenRange{arguments}));
        };

        // Assert
        EXPECT_THROW(run(), Parsing::ParseError);
    }

    TEST(InMemoryTest, WritesTheHelpOfACommandInMemory)
    {
        // Arrange
        std::string written;
        std::string writtenErrors;
        InputStream input{std::string_view{}};
        OutputStream output{written};
        ErrorOutputStream errorOutput{writtenErrors};
        Event::Loop loop;
        CountCommand count;
        const auto manager = CommandManager{input, output, errorOutput, loop}.addCommand(count);
        constexpr std::array<const char*, 2> arguments{"count", "--help"};

        // Act
        const int exitCode = manager.run(Parsing::TokenRange{arguments});

        // Assert
        EXPECT_EQ(exitCode, 0);
        EXPECT_NE(written.find("Usage:"), std::string::npos);
    }
} // namespace Consolpp::Tests::Functional::Running
