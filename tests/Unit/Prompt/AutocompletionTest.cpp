#include "Consolpp/Prompt/Autocompletion.hpp"

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Autocompletion;

    namespace
    {
        constexpr std::array<std::string_view, 3> values{"deploy", "deploy:all", "status"};
    } // namespace

    TEST(AutocompletionTest, FromValuesFindsTheValuesStartingWithTheText)
    {
        // Arrange
        const Autocompletion autocompletion = Autocompletion::fromValues(values);
        const std::vector<std::string> expected{"deploy", "deploy:all"};
        std::vector<std::string> completions{"old"};

        // Act
        autocompletion.findCompletions("dep", completions);

        // Assert
        EXPECT_EQ(completions, expected);
    }

    TEST(AutocompletionTest, UsesTheGivenFunction)
    {
        // Arrange
        const Autocompletion autocompletion{[](const std::string_view text, std::vector<std::string>& completions) {
            completions.assign(1, std::string{text} + "/");
        }};
        const std::vector<std::string> expected{"src/"};
        std::vector<std::string> completions;

        // Act
        autocompletion.findCompletions("src", completions);

        // Assert
        EXPECT_EQ(completions, expected);
    }
} // namespace Consolpp::Tests::Unit::Prompt
