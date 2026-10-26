#include "Consolpp/Prompt/FormatAnswerLine.hpp"

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/Echo.hpp"
#include "Consolpp/Prompt/Selection.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"

#include <array>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::ChoiceField;
    using Consolpp::Prompt::ChoiceFieldOptions;
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::ConfirmationFieldOptions;
    using Consolpp::Prompt::Echo;
    using Consolpp::Prompt::formatAnswerLine;
    using Consolpp::Prompt::Selection;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;

    namespace
    {
        constexpr std::array<std::string_view, 3> choices{"apple", "b&b", "cherry"};
        constexpr std::array<std::size_t, 2> chosen{0, 1};
    } // namespace

    TEST(FormatAnswerLineTest, ShowsTheDefaultValueOfAnEmptyText)
    {
        // Arrange
        const TextField field{TextFieldOptions{.label = "Name", .defaultValue = "a<b"}};
        std::string formatted;

        // Act
        formatAnswerLine(field, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, " <prompt-label>Name</prompt-label>: a&lt;b");
    }

    TEST(FormatAnswerLineTest, MasksAMaskedText)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Password", .echo = Echo::Masked}};
        std::string formatted;

        field.type(U'a');
        field.type(U'b');

        // Act
        formatAnswerLine(field, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, " <prompt-label>Password</prompt-label>: **");
    }

    TEST(FormatAnswerLineTest, LeavesOutAHiddenText)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Password", .echo = Echo::Hidden}};
        std::string formatted;

        field.type(U'a');

        // Act
        formatAnswerLine(field, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, " <prompt-label>Password</prompt-label>: ");
    }

    TEST(FormatAnswerLineTest, JoinsTheChosenChoices)
    {
        // Arrange
        const ChoiceField field{ChoiceFieldOptions{
            .label = "Fruits",
            .choices = choices,
            .selection = Selection::Multiple,
            .defaultIndexes = chosen,
        }};
        std::string formatted;

        // Act
        formatAnswerLine(field, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, " <prompt-label>Fruits</prompt-label>: apple, b&amp;b");
    }

    TEST(FormatAnswerLineTest, ShowsTheConfirmationAsAWord)
    {
        // Arrange
        const ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};
        std::string formatted;

        // Act
        formatAnswerLine(field, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, " <prompt-label>Continue?</prompt-label>: yes");
    }
} // namespace Consolpp::Tests::Unit::Prompt
