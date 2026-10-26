#include "Consolpp/Completion/FormatResult.hpp"

#include "Consolpp/Completion/Result.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <iterator>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Completion
{
    using Consolpp::Completion::formatResult;
    using Consolpp::Completion::Result;

    TEST(FormatResultTest, WritesTheSuggestionsAfterWords)
    {
        // Arrange
        const Result result{.suggestions = {"deploy", "deploy:all"}};
        std::string answer;

        // Act
        formatResult(result, std::back_inserter(answer));

        // Assert
        EXPECT_EQ(answer, "words\ndeploy\ndeploy:all\n");
    }

    TEST(FormatResultTest, WritesOnlyWordsWithoutSuggestions)
    {
        // Arrange
        const Result result{};
        std::string answer;

        // Act
        formatResult(result, std::back_inserter(answer));

        // Assert
        EXPECT_EQ(answer, "words\n");
    }

    TEST(FormatResultTest, WritesTheKindOfPathsAndTheValuePrefix)
    {
        // Arrange
        const Result result{.paths = ValueCompletion::Directory, .valuePrefix = "--target="};
        std::string answer;

        // Act
        formatResult(result, std::back_inserter(answer));

        // Assert
        EXPECT_EQ(answer, "directories\n--target=\n");
    }

    TEST(FormatResultTest, ThrowsForComputedPaths)
    {
        // Arrange
        const Result result{.paths = ValueCompletion::Computed};
        std::string answer;

        // Act
        const auto format = [&result, &answer] {
            formatResult(result, std::back_inserter(answer));
        };

        // Assert
        EXPECT_THROW(format(), std::invalid_argument);
    }
} // namespace Consolpp::Tests::Unit::Completion
