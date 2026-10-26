#include "Consolpp/Formatting/FormatSignatureAsText.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Option.hpp"

#include <array>
#include <iterator>
#include <span>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatSignatureAsText;

    TEST(FormatSignatureAsTextTest, ListsOnlyTheHelpOptionWithoutOptionsAndArguments)
    {
        // Arrange
        constexpr std::span<const Option> options{};
        constexpr std::span<const Argument> arguments{};
        std::string formatted;

        // Act
        formatSignatureAsText(options, arguments, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "<comment>Options:</comment>\n"
                  "  <info>-h, --help</info>            Display help for the command\n"
                  "  <info>    --version</info>         Display the program version\n"
                  "  <info>-q, --quiet</info>           Only errors are displayed. All other output is suppressed\n"
                  "  <info>    --silent</info>          Do not output any message\n"
                  "  <info>-v, --verbose</info>         Increase the verbosity of messages: once for verbose output, "
                  "twice for more, three times for debug\n"
                  "  <info>-n, --no-interaction</info>  Do not ask any interactive question");
    }

    TEST(FormatSignatureAsTextTest, KeepsTheDescriptionMarkupOfAnArgumentWithItsDefault)
    {
        // Arrange
        constexpr std::span<const Option> options{};
        constexpr std::array<Argument, 1> arguments{
            {{.name = "target", .required = false, .description = "Where <info>to</info> go", .defaultValue = "out"}},
        };
        std::string formatted;

        // Act
        formatSignatureAsText(options, arguments, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted,
                  "<comment>Arguments:</comment>\n"
                  "  <info>target</info>                Where <info>to</info> go<comment> [default: out]</comment>\n"
                  "\n"
                  "<comment>Options:</comment>\n"
                  "  <info>-h, --help</info>            Display help for the command\n"
                  "  <info>    --version</info>         Display the program version\n"
                  "  <info>-q, --quiet</info>           Only errors are displayed. All other output is suppressed\n"
                  "  <info>    --silent</info>          Do not output any message\n"
                  "  <info>-v, --verbose</info>         Increase the verbosity of messages: once for verbose output, "
                  "twice for more, three times for debug\n"
                  "  <info>-n, --no-interaction</info>  Do not ask any interactive question");
    }
} // namespace Consolpp::Tests::Unit::Formatting
