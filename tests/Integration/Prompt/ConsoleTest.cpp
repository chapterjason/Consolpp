#include "Consolpp/Prompt/Console.hpp"

#include "../Stream/PseudoTerminal.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Keyboard/Modifiers.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/Autocompletion.hpp"
#include "Consolpp/Prompt/CancelledError.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/Echo.hpp"
#include "Consolpp/Prompt/Form.hpp"
#include "Consolpp/Prompt/FormAnswer.hpp"
#include "Consolpp/Prompt/KeyBinding.hpp"
#include "Consolpp/Prompt/Selection.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Prompt
{
    using Consolpp::Event::Loop;
    using Consolpp::Keyboard::Key;
    using Consolpp::Keyboard::KeyCode;
    using Consolpp::Keyboard::Modifiers;
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::Autocompletion;
    using Consolpp::Prompt::CancelledError;
    using Consolpp::Prompt::ChoiceField;
    using Consolpp::Prompt::ChoiceFieldOptions;
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::ConfirmationFieldOptions;
    using Consolpp::Prompt::Console;
    using Consolpp::Prompt::Echo;
    using Consolpp::Prompt::Form;
    using Consolpp::Prompt::FormAnswer;
    using Consolpp::Prompt::KeyBinding;
    using Consolpp::Prompt::Selection;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;
    using Consolpp::Stream::FlushMode;
    using Consolpp::Stream::OutputStream;
    using Consolpp::Terminal::Capabilities;
    using Integration::Stream::PseudoTerminal;

    namespace
    {
        constexpr unsigned short columns = 80;
        constexpr std::array<std::string_view, 2> names{"consolpp", "conductor"};
        constexpr std::array<std::string_view, 3> fruits{"apple", "banana", "cherry"};
    } // namespace

    TEST(ConsoleTest, AsksATextField)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const TextField field{TextFieldOptions{.label = "Name"}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("ab\x7f"
                      "c\r");

        // Act
        const std::string answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, "ac");
        EXPECT_NE(terminal.drain().find(" Name: ac"), std::string::npos);
    }

    TEST(ConsoleTest, TakesTheDefaultValueForAnEmptyAnswer)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const TextField field{TextFieldOptions{.label = "Name", .defaultValue = "consolpp"}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("\r");

        // Act
        const std::string answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, "consolpp");
    }

    TEST(ConsoleTest, CompletesTheMatchChosenWithTheArrows)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const TextField field{TextFieldOptions{.label = "Name", .autocompletion = Autocompletion::fromValues(names)}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("co\x1b[B\t\r");

        // Act
        const std::string answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, "conductor");
    }

    TEST(ConsoleTest, NeverWritesAHiddenText)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const TextField field{TextFieldOptions{.label = "Secret", .echo = Echo::Hidden}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("hunter2\r");

        // Act
        const std::string answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, "hunter2");
        EXPECT_EQ(terminal.drain().find("hunter2"), std::string::npos);
    }

    TEST(ConsoleTest, AsksASingleChoiceFieldThatWrapsAround)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = fruits}};
        const std::vector<std::size_t> expected{2};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("\x1b[A\r");

        // Act
        const std::vector<std::size_t> answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, expected);
    }

    TEST(ConsoleTest, AsksAMultipleChoiceField)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const ChoiceField field{
            ChoiceFieldOptions{.label = "Fruits", .choices = fruits, .selection = Selection::Multiple}};
        const std::vector<std::size_t> expected{0, 2};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type(" \x1b[A \r");

        // Act
        const std::vector<std::size_t> answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, expected);
    }

    TEST(ConsoleTest, ThrowsWhenCancelled)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = fruits}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("\x1b[B\x1b");

        // Act
        const auto ask = [&console, &field] {
            static_cast<void>(console.ask(field));
        };

        // Assert
        EXPECT_THROW(ask(), CancelledError);
    }

    TEST(ConsoleTest, ThrowsWhenCancelledWithControlC)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const TextField field{TextFieldOptions{.label = "Name"}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("a\x03");

        // Act
        const auto ask = [&console, &field] {
            static_cast<void>(console.ask(field));
        };

        // Assert
        EXPECT_THROW(ask(), CancelledError);
    }

    TEST(ConsoleTest, InsertsABracketedPasteWithoutSubmitting)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const TextField field{TextFieldOptions{.label = "Name"}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("\x1b[200~ab\rc\x1b[201~\r");

        // Act
        const std::string answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, "abc");
    }

    TEST(ConsoleTest, ResolvesKeysThroughTheGivenBindings)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        constexpr std::array<KeyBinding, 1> bindings{
            {
                {
                    .key = Key{.code = KeyCode::Character, .character = U's', .modifiers = Modifiers{.control = true}},
                    .action = Action::Submit,
                },
            },
        };
        Console console{loop, terminal.getTerminal(), output, bindings};
        const TextField field{TextFieldOptions{.label = "Name"}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("a\r\x13");

        // Act
        const std::string answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, "a");
    }

    TEST(ConsoleTest, AsksAConfirmation)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const ConfirmationField field{ConfirmationFieldOptions{.label = "Continue?", .defaultValue = true}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("n\r");

        // Act
        const bool answer = console.ask(field);

        // Assert
        EXPECT_FALSE(answer);
        EXPECT_NE(terminal.drain().find(" Continue?: no"), std::string::npos);
    }

    TEST(ConsoleTest, TypesTheConfirmationKeysIntoATextField)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const TextField field{TextFieldOptions{.label = "Name"}};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("yn\r");

        // Act
        const std::string answer = console.ask(field);

        // Assert
        EXPECT_EQ(answer, "yn");
    }

    TEST(ConsoleTest, AsksTheFieldsOfAFormOneAfterAnother)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const Form form{
            TextField{TextFieldOptions{.label = "Name"}},
            ConfirmationField{ConfirmationFieldOptions{.label = "Continue?"}},
        };

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("ab\ry\r");

        // Act
        const std::vector<FormAnswer> answers = console.ask(form.getFormFields());

        // Assert
        ASSERT_EQ(answers.size(), 2U);
        EXPECT_EQ(std::get<std::string>(answers.at(0)), "ab");
        EXPECT_TRUE(std::get<bool>(answers.at(1)));
        EXPECT_NE(terminal.drain().find(" Name: ab\n Continue?: yes\n"), std::string::npos);
    }

    TEST(ConsoleTest, GoesBackToThePreviousFieldOfAForm)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const Form form{
            TextField{TextFieldOptions{.label = "Name"}},
            ConfirmationField{ConfirmationFieldOptions{.label = "Continue?"}},
        };

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("ab\r\x1b[Zc\rn\r");

        // Act
        const std::vector<FormAnswer> answers = console.ask(form.getFormFields());

        // Assert
        ASSERT_EQ(answers.size(), 2U);
        EXPECT_EQ(std::get<std::string>(answers.at(0)), "abc");
        EXPECT_FALSE(std::get<bool>(answers.at(1)));
    }

    TEST(ConsoleTest, ThrowsWhenAFieldOfAFormIsCancelled)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const Form form{
            TextField{TextFieldOptions{.label = "Name"}},
            ConfirmationField{ConfirmationFieldOptions{.label = "Continue?"}},
        };

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("ab\r\x03");

        // Act
        const auto ask = [&console, &form] {
            static_cast<void>(console.ask(form.getFormFields()));
        };

        // Assert
        EXPECT_THROW(ask(), CancelledError);
    }

    TEST(ConsoleTest, AsksAChoiceFieldInAForm)
    {
        // Arrange
        Loop loop;
        const PseudoTerminal terminal;
        OutputStream output{terminal.getTerminal(), FlushMode::Buffered, false, Capabilities{}};
        Console console{loop, terminal.getTerminal(), output};
        const Form form{
            TextField{TextFieldOptions{.label = "Name"}},
            ChoiceField{ChoiceFieldOptions{.label = "Fruit", .choices = fruits}},
        };
        const std::vector<std::size_t> expected{1};

        terminal.setColumns(columns);
        terminal.switchToRaw();
        terminal.type("ab\r\x1b[B\r");

        // Act
        const std::vector<FormAnswer> answers = console.ask(form.getFormFields());

        // Assert
        ASSERT_EQ(answers.size(), 2U);
        EXPECT_EQ(std::get<std::vector<std::size_t>>(answers.at(1)), expected);
        EXPECT_NE(terminal.drain().find("\x1b[?25l"), std::string::npos);
    }
} // namespace Consolpp::Tests::Integration::Prompt
