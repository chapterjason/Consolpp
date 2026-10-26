#include "Consolpp/Completion/Complete.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Completion/Result.hpp"
#include "Consolpp/Completion/ValueTarget.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/IsSuggestion.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <array>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Completion
{
    using Consolpp::Completion::complete;
    using Consolpp::Completion::Request;
    using Consolpp::Completion::Result;
    using Consolpp::Completion::ValueTarget;

    namespace
    {
        constexpr std::array<std::string_view, 3> formats{"txt", "json", "md"};
        constexpr std::array<Option, 3> deployOptions{
            Option{.name = "force", .shortcut = 'f'},
            Option{.name = "format", .shortcut = 'F', .value = OptionValue::Required, .suggestedValues = formats},
            Option{.name = "output", .value = OptionValue::Required, .completion = ValueCompletion::File},
        };
        constexpr std::array<Argument, 2> deployArguments{
            Argument{.name = "environment", .completion = ValueCompletion::Computed},
            Argument{.name = "files", .required = false, .variadic = true, .completion = ValueCompletion::Path},
        };
        constexpr std::array<std::string_view, 1> deployAliases{"d"};
        constexpr std::array<DefinitionView, 2> definitions{
            DefinitionView{.name = "cache:clear"},
            DefinitionView{
                .name = "deploy",
                .aliases = deployAliases,
                .options = deployOptions,
                .arguments = deployArguments,
            },
        };
        constexpr std::array<std::string_view, 3> names{"cache:clear", "d", "deploy"};
        constexpr CommandList commands{
            .definitions = definitions,
            .names = names,
            .isSuggestion = &Consolpp::isSuggestion<names.front().size()>,
        };

        std::vector<std::string> computeEnvironments(const std::string_view command, const Request& request)
        {
            if (command != "deploy" || request.name != "environment") {
                return std::vector<std::string>{};
            }

            return std::vector<std::string>{"production", "staging"};
        }
    } // namespace

    TEST(CompleteTest, SuggestsTheCommandNamesThatStartWithTheTypedWord)
    {
        // Arrange
        constexpr std::array<const char*, 1> words{"de"};

        // Act
        const Result result = complete(commands, "", words, 0, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"deploy"}));
    }

    TEST(CompleteTest, SuggestsEveryCommandNameForAnEmptyWord)
    {
        // Arrange
        constexpr std::array<const char*, 1> words{""};

        // Act
        const Result result = complete(commands, "", words, 0, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"cache:clear", "d", "deploy"}));
    }

    TEST(CompleteTest, SuggestsOnlyTheGlobalOptionsBeforeTheCommandName)
    {
        // Arrange
        constexpr std::array<const char*, 1> words{"-"};

        // Act
        const Result result = complete(commands, "", words, 0, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions,
                  (std::vector<std::string>{"--help",
                                            "--version",
                                            "--quiet",
                                            "--silent",
                                            "--verbose",
                                            "--no-interaction"}));
    }

    TEST(CompleteTest, SuggestsTheOptionsOfTheCommand)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"deploy", "--f"};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"--force", "--format"}));
    }

    TEST(CompleteTest, SuggestsTheNegationOfAFlagButNotOfAGlobalOption)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"deploy", "--no"};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"--no-force", "--no-interaction"}));
    }

    TEST(CompleteTest, FindsTheCommandByItsAlias)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"d", "--fo"};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"--force", "--format"}));
    }

    TEST(CompleteTest, SkipsGlobalOptionsBeforeTheCommandName)
    {
        // Arrange
        constexpr std::array<const char*, 3> words{"-v", "deploy", "--fo"};

        // Act
        const Result result = complete(commands, "", words, 2, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"--force", "--format"}));
    }

    TEST(CompleteTest, SuggestsTheValuesOfAnOptionInTheNextWord)
    {
        // Arrange
        constexpr std::array<const char*, 3> words{"deploy", "--format", "j"};

        // Act
        const Result result = complete(commands, "", words, 2, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"json"}));
    }

    TEST(CompleteTest, SuggestsTheValuesAfterTheEqualsSignAsWholeWords)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"deploy", "--format=j"};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"--format=json"}));
    }

    TEST(CompleteTest, SuggestsTheValuesAttachedToAShortcut)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"deploy", "-fFm"};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"-fFmd"}));
    }

    TEST(CompleteTest, SuggestsTheValuesOfAShortcutInTheNextWord)
    {
        // Arrange
        constexpr std::array<const char*, 3> words{"deploy", "-F", ""};

        // Act
        const Result result = complete(commands, "", words, 2, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"txt", "json", "md"}));
    }

    TEST(CompleteTest, SuggestsNoValuesForAFlag)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"deploy", "--force="};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_TRUE(result.suggestions.empty());
    }

    TEST(CompleteTest, AsksTheCommandForComputedValues)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"deploy", "p"};
        std::optional<Request> asked;
        const auto computeValues = [&asked](const std::string_view /*command*/, const Request& request) {
            asked = request;

            return std::vector<std::string>{"production", "staging"};
        };

        // Act
        const Result result = complete(commands, "", words, 1, computeValues);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"production"}));
        ASSERT_TRUE(asked.has_value());
        const Request request = asked.value_or(Request{});
        EXPECT_EQ(request.target, ValueTarget::Argument);
        EXPECT_EQ(request.name, "environment");
        EXPECT_EQ(request.value, "p");
    }

    TEST(CompleteTest, LeavesFilesToTheShellWithTheValuePrefix)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"deploy", "--output=out"};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_EQ(result.paths, ValueCompletion::File);
        EXPECT_EQ(result.valuePrefix, "--output=");
        EXPECT_TRUE(result.suggestions.empty());
    }

    TEST(CompleteTest, CompletesEveryValueOfTheVariadicArgument)
    {
        // Arrange
        constexpr std::array<const char*, 4> words{"deploy", "production", "a.txt", "b"};

        // Act
        const Result result = complete(commands, "", words, 3, computeEnvironments);

        // Assert
        EXPECT_EQ(result.paths, ValueCompletion::Path);
        EXPECT_EQ(result.valuePrefix, "");
    }

    TEST(CompleteTest, CountsNoOptionValueAsArgument)
    {
        // Arrange
        constexpr std::array<const char*, 4> words{"deploy", "--format", "txt", "st"};

        // Act
        const Result result = complete(commands, "", words, 3, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"staging"}));
    }

    TEST(CompleteTest, CompletesArgumentsInsteadOfOptionsAfterTheSeparator)
    {
        // Arrange
        constexpr std::array<const char*, 3> words{"deploy", "--", "-"};

        // Act
        const Result result = complete(commands, "", words, 2, computeEnvironments);

        // Assert
        EXPECT_TRUE(result.suggestions.empty());
        EXPECT_EQ(result.paths, ValueCompletion::None);
    }

    TEST(CompleteTest, SuggestsNothingForAnArgumentTheCommandDoesNotTake)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"cache:clear", "x"};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_TRUE(result.suggestions.empty());
        EXPECT_EQ(result.paths, ValueCompletion::None);
    }

    TEST(CompleteTest, SuggestsNothingForAnUnknownCommand)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"unknown", "--"};

        // Act
        const Result result = complete(commands, "", words, 1, computeEnvironments);

        // Assert
        EXPECT_TRUE(result.suggestions.empty());
    }

    TEST(CompleteTest, SuggestsTheOptionsOfTheSingleCommandFromTheFirstWord)
    {
        // Arrange
        constexpr std::array<const char*, 2> words{"production", "--fo"};

        // Act
        const Result result = complete(commands, "deploy", words, 1, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"--force", "--format"}));
    }

    TEST(CompleteTest, CompletesTheFirstArgumentOfTheSingleCommandInsteadOfCommandNames)
    {
        // Arrange
        constexpr std::array<const char*, 1> words{"st"};

        // Act
        const Result result = complete(commands, "deploy", words, 0, computeEnvironments);

        // Assert
        EXPECT_EQ(result.suggestions, (std::vector<std::string>{"staging"}));
    }

    TEST(CompleteTest, ThrowsForACurrentWordPastTheWords)
    {
        // Arrange
        constexpr std::array<const char*, 1> words{"deploy"};

        // Act
        const auto completeAfterTheWords = [&words] {
            static_cast<void>(complete(commands, "", words, 1, computeEnvironments));
        };

        // Assert
        EXPECT_THROW(completeAfterTheWords(), std::out_of_range);
    }
} // namespace Consolpp::Tests::Unit::Completion
