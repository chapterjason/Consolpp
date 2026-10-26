#include "Consolpp/Prompt/ChoiceField.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/Selection.hpp"
#include "Consolpp/Prompt/State.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::ChoiceField;
    using Consolpp::Prompt::ChoiceFieldOptions;
    using Consolpp::Prompt::Selection;
    using Consolpp::Prompt::State;

    namespace
    {
        constexpr std::array<std::string_view, 3> choices{"apple", "banana", "cherry"};
        constexpr std::array<std::size_t, 2> defaultIndexes{1, 2};
        constexpr std::array<std::size_t, 1> outOfRange{3};
    } // namespace

    TEST(ChoiceFieldTest, MoveDownHighlightsTheNextChoice)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices, .selection = Selection::Single}};

        // Act
        field.apply(Action::MoveDown);

        // Assert
        EXPECT_EQ(field.getHighlighted(), 1U);
    }

    TEST(ChoiceFieldTest, MoveDownOnTheLastChoiceJumpsToTheFirst)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices, .selection = Selection::Single}};

        field.apply(Action::MoveUp);

        // Act
        field.apply(Action::MoveDown);

        // Assert
        EXPECT_EQ(field.getHighlighted(), 0U);
    }

    TEST(ChoiceFieldTest, MoveUpOnTheFirstChoiceJumpsToTheLast)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices, .selection = Selection::Single}};

        // Act
        field.apply(Action::MoveUp);

        // Assert
        EXPECT_EQ(field.getHighlighted(), 2U);
    }

    TEST(ChoiceFieldTest, ToggleTogglesTheHighlightedChoiceInAMultipleSelection)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices, .selection = Selection::Multiple}};

        field.apply(Action::MoveDown);

        // Act
        field.apply(Action::Toggle);

        // Assert
        EXPECT_TRUE(field.isSelected(1));
    }

    TEST(ChoiceFieldTest, SubmitTakesTheHighlightedChoiceOfASingleSelection)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices, .selection = Selection::Single}};

        field.apply(Action::MoveDown);

        // Act
        field.apply(Action::Submit);

        // Assert
        EXPECT_EQ(field.getState(), State::Submitted);
        EXPECT_EQ(field.getChosenIndexes(), std::vector<std::size_t>{1});
    }

    TEST(ChoiceFieldTest, ChoosesTheSelectedChoicesOfAMultipleSelection)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices, .selection = Selection::Multiple}};

        field.apply(Action::Toggle);
        field.apply(Action::MoveUp);

        // Act
        field.apply(Action::Toggle);

        // Assert
        EXPECT_EQ(field.getChosenIndexes(), (std::vector<std::size_t>{0, 2}));
    }

    TEST(ChoiceFieldTest, StartsWithTheDefaults)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{
            .label = "Fruit",
            .choices = choices,
            .selection = Selection::Multiple,
            .defaultIndexes = defaultIndexes,
        }};

        // Act
        field.apply(Action::MoveDown);

        // Assert
        EXPECT_TRUE(field.isSelected(1));
        EXPECT_EQ(field.getHighlighted(), 2U);
    }

    TEST(ChoiceFieldTest, CancelEndsTheField)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices, .selection = Selection::Single}};

        // Act
        field.apply(Action::Cancel);

        // Assert
        EXPECT_EQ(field.getState(), State::Cancelled);
    }

    TEST(ChoiceFieldTest, ThrowsWithoutChoices)
    {
        // Act
        const auto create = [] {
            const ChoiceField field{
                ChoiceFieldOptions{.label = "Fruit", .choices = std::span<const std::string_view>{}}};
        };

        // Assert
        EXPECT_THROW(create(), std::invalid_argument);
    }

    TEST(ChoiceFieldTest, ThrowsForADefaultIndexOutOfRange)
    {
        // Act
        const auto create = [] {
            const ChoiceField field{
                ChoiceFieldOptions{.label = "Fruit", .choices = choices, .defaultIndexes = outOfRange}};
        };

        // Assert
        EXPECT_THROW(create(), std::invalid_argument);
    }

    TEST(ChoiceFieldTest, LeavesToggleToAMultipleSelection)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices}};

        // Act
        const bool taken = field.apply(Action::Toggle);

        // Assert
        EXPECT_FALSE(taken);
    }

    TEST(ChoiceFieldTest, ReopensASubmittedField)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices}};

        field.apply(Action::Submit);

        // Act
        field.reopen();

        // Assert
        EXPECT_EQ(field.getState(), State::Active);
    }

    TEST(ChoiceFieldTest, KeepsACancelledFieldCancelledWhenReopened)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices}};

        field.apply(Action::Cancel);

        // Act
        field.reopen();

        // Assert
        EXPECT_EQ(field.getState(), State::Cancelled);
    }

    TEST(ChoiceFieldTest, LeavesThePreviousFieldActionToTheForm)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices}};

        // Act
        const bool taken = field.apply(Action::PreviousField);

        // Assert
        EXPECT_FALSE(taken);
        EXPECT_EQ(field.getState(), State::Active);
    }
} // namespace Consolpp::Tests::Unit::Prompt
