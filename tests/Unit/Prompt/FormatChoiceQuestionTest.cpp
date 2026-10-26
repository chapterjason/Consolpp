#include "Consolpp/Prompt/FormatChoiceQuestion.hpp"

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/Selection.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::ChoiceField;
    using Consolpp::Prompt::ChoiceFieldOptions;
    using Consolpp::Prompt::formatChoiceQuestion;
    using Consolpp::Prompt::Selection;

    namespace
    {
        constexpr std::array<std::string_view, 3> choices{"apple", "banana", "cherry"};
        constexpr std::array<std::size_t, 2> defaultIndexes{0, 2};
    } // namespace

    TEST(FormatChoiceQuestionTest, FormatsTheLabelWithTheDefaultChoices)
    {
        // Arrange
        const ChoiceField field{ChoiceFieldOptions{
            .label = "Fruits",
            .choices = choices,
            .selection = Selection::Multiple,
            .defaultIndexes = defaultIndexes,
        }};

        // Act
        const std::string line = formatChoiceQuestion(field);

        // Assert
        EXPECT_EQ(line, " <prompt-label>Fruits</prompt-label> [<prompt-default>apple, cherry</prompt-default>]:");
    }
} // namespace Consolpp::Tests::Unit::Prompt
