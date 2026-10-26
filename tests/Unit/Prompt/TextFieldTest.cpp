#include "Consolpp/Prompt/TextField.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/Autocompletion.hpp"
#include "Consolpp/Prompt/ControlCharacters.hpp"
#include "Consolpp/Prompt/State.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"

#include <array>
#include <stdexcept>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::Autocompletion;
    using Consolpp::Prompt::ControlCharacters;
    using Consolpp::Prompt::State;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;

    namespace
    {
        constexpr std::array<std::string_view, 4> names{"consolpp", "console-tools", "conductor", "status"};

        void type(TextField& field, const std::u32string_view characters)
        {
            for (const char32_t character : characters) {
                field.type(character);
            }
        }
    } // namespace

    TEST(TextFieldTest, TypesCharactersAtTheCursor)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        field.type(U'a');
        field.type(U'c');
        field.apply(Action::MoveLeft);

        // Act
        field.type(U'b');

        // Assert
        EXPECT_EQ(field.getText(), "abc");
        EXPECT_EQ(field.getCursor(), 2U);
    }

    TEST(TextFieldTest, EraseBackwardRemovesTheWholeGraphemeBeforeTheCursor)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        field.type(U'e');
        field.type(U'\u0301');

        // Act
        field.apply(Action::EraseBackward);

        // Assert
        EXPECT_EQ(field.getText(), "");
        EXPECT_EQ(field.getCursor(), 0U);
    }

    TEST(TextFieldTest, EraseForwardRemovesTheGraphemeAfterTheCursor)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        field.type(U'a');
        field.type(U'b');
        field.type(U'c');
        field.apply(Action::MoveToStart);

        // Act
        field.apply(Action::EraseForward);

        // Assert
        EXPECT_EQ(field.getText(), "bc");
        EXPECT_EQ(field.getCursor(), 0U);
    }

    TEST(TextFieldTest, SubmitEndsTheField)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        field.type(U'a');
        field.type(U'b');
        field.type(U'c');

        // Act
        field.apply(Action::Submit);

        // Assert
        EXPECT_EQ(field.getState(), State::Submitted);
    }

    TEST(TextFieldTest, CancelEndsTheField)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        field.type(U'a');
        field.type(U'b');
        field.type(U'c');

        // Act
        field.apply(Action::Cancel);

        // Assert
        EXPECT_EQ(field.getState(), State::Cancelled);
    }

    TEST(TextFieldTest, TakesTheDefaultValueForAnEmptyText)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .defaultValue = "consolpp"}};

        // Act
        field.apply(Action::Submit);

        // Assert
        EXPECT_EQ(field.getValue(), "consolpp");
    }

    TEST(TextFieldTest, SuggestsTheRestOfTheFirstCompletion)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .autocompletion = Autocompletion::fromValues(names)}};

        field.type(U'c');

        // Act
        field.type(U'o');

        // Assert
        EXPECT_EQ(field.getSuggestion(), "nsolpp");
    }

    TEST(TextFieldTest, CompleteTakesTheShownSuggestion)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .autocompletion = Autocompletion::fromValues(names)}};

        field.type(U'c');
        field.type(U'o');

        // Act
        field.apply(Action::Complete);

        // Assert
        EXPECT_EQ(field.getText(), "consolpp");
        EXPECT_EQ(field.getCursor(), 8U);
    }

    TEST(TextFieldTest, MoveDownShowsTheNextMatchAndKeepsTheText)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .autocompletion = Autocompletion::fromValues(names)}};

        field.type(U'c');
        field.type(U'o');

        // Act
        field.apply(Action::MoveDown);

        // Assert
        EXPECT_EQ(field.getText(), "co");
        EXPECT_EQ(field.getSuggestion(), "nsole-tools");
    }

    TEST(TextFieldTest, MoveUpOnTheFirstMatchShowsTheLast)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .autocompletion = Autocompletion::fromValues(names)}};

        field.type(U'c');
        field.type(U'o');

        // Act
        field.apply(Action::MoveUp);

        // Assert
        EXPECT_EQ(field.getSuggestion(), "nductor");
    }

    TEST(TextFieldTest, CompleteTakesTheMatchMovedTo)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .autocompletion = Autocompletion::fromValues(names)}};

        field.type(U'c');
        field.type(U'o');
        field.apply(Action::MoveDown);
        field.apply(Action::MoveDown);

        // Act
        field.apply(Action::Complete);

        // Assert
        EXPECT_EQ(field.getText(), "conductor");
    }

    TEST(TextFieldTest, TypingShowsTheFirstMatchAgain)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .autocompletion = Autocompletion::fromValues(names)}};

        field.type(U'c');
        field.type(U'o');
        field.apply(Action::MoveDown);

        // Act
        field.type(U'n');

        // Assert
        EXPECT_EQ(field.getSuggestion(), "solpp");
    }

    TEST(TextFieldTest, PasteInsertsAtTheCursor)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        type(field, U"abc");
        field.apply(Action::MoveLeft);

        // Act
        field.paste("xy");

        // Assert
        EXPECT_EQ(field.getText(), "abxyc");
        EXPECT_EQ(field.getCursor(), 4U);
    }

    TEST(TextFieldTest, PasteDropsControlCharactersByDefault)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        // Act
        field.paste("a\r\nb\x1b"
                    "c");

        // Assert
        EXPECT_EQ(field.getText(), "abc");
    }

    TEST(TextFieldTest, SuggestsTheFirstValueForAnEmptyText)
    {
        // Arrange
        TextFieldOptions options{.label = "Name", .autocompletion = Autocompletion::fromValues(names)};

        // Act
        const TextField field{options};

        // Assert
        EXPECT_EQ(field.getSuggestion(), "consolpp");
    }

    TEST(TextFieldTest, PasteReplacesControlCharacters)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        TextField::setControlCharacters(ControlCharacters::Replace);

        // Act
        field.paste("a\r\nb\tc\x1b"
                    "d");

        // Assert
        TextField::setControlCharacters(ControlCharacters::Drop);
        EXPECT_EQ(field.getText(),
                  "a b c\xEF\xBF\xBD"
                  "d");
    }

    TEST(TextFieldTest, PasteKeepsControlCharacters)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        TextField::setControlCharacters(ControlCharacters::Keep);

        // Act
        field.paste("a\r\nb");

        // Assert
        TextField::setControlCharacters(ControlCharacters::Drop);
        EXPECT_EQ(field.getText(), "a\r\nb");
    }

    TEST(TextFieldTest, SetsTheTemplateOfEveryTextField)
    {
        // Arrange
        const std::string_view previous = TextField::getQuestionTemplate();

        // Act
        TextField::setQuestionTemplate("{label}?");

        // Assert
        EXPECT_EQ(TextField::getQuestionTemplate(), "{label}?");
        TextField::setQuestionTemplate(previous);
    }

    TEST(TextFieldTest, ThrowsForAPlaceholderTheTemplateDoesNotAllow)
    {
        // Arrange
        const std::string_view markupTemplate{"{answer}?"};

        // Act
        const auto set = [markupTemplate] {
            TextField::setQuestionTemplate(markupTemplate);
        };

        // Assert
        EXPECT_THROW(set(), std::invalid_argument);
    }

    TEST(TextFieldTest, ThrowsForAnInputTemplateWithoutTheInput)
    {
        // Arrange
        const std::string_view markupTemplate{" &gt; "};

        // Act
        const auto set = [markupTemplate] {
            TextField::setInputTemplate(markupTemplate);
        };

        // Assert
        EXPECT_THROW(set(), std::invalid_argument);
    }

    TEST(TextFieldTest, ReopensASubmittedField)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        field.apply(Action::Submit);

        // Act
        field.reopen();

        // Assert
        EXPECT_EQ(field.getState(), State::Active);
    }

    TEST(TextFieldTest, KeepsACancelledFieldCancelledWhenReopened)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        field.apply(Action::Cancel);

        // Act
        field.reopen();

        // Assert
        EXPECT_EQ(field.getState(), State::Cancelled);
    }

    TEST(TextFieldTest, LeavesThePreviousFieldActionToTheForm)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};

        // Act
        const bool taken = field.apply(Action::PreviousField);

        // Assert
        EXPECT_FALSE(taken);
        EXPECT_EQ(field.getState(), State::Active);
    }
} // namespace Consolpp::Tests::Unit::Prompt
