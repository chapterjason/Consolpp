#include "Consolpp/Stream/InputStream.hpp"

#include "Channel.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Keyboard/KeyCode.hpp"
#include "Consolpp/Keyboard/Modifiers.hpp"
#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/ConfirmationFieldOptions.hpp"
#include "Consolpp/Prompt/DefaultKeyBindings.hpp"
#include "Consolpp/Prompt/FindAction.hpp"
#include "Consolpp/Prompt/Form.hpp"
#include "Consolpp/Prompt/InteractionDisabledError.hpp"
#include "Consolpp/Prompt/TextField.hpp"
#include "Consolpp/Prompt/TextFieldOptions.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Stream/ReadError.hpp"
#include "Consolpp/Stream/ReadErrorKind.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"
#include "PseudoTerminal.hpp"

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Integration::Stream
{
    using Consolpp::Keyboard::Key;
    using Consolpp::Keyboard::KeyCode;
    using Consolpp::Keyboard::Modifiers;
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::defaultKeyBindings;
    using Consolpp::Prompt::findAction;
    using Consolpp::Stream::InputStream;
    using Consolpp::Stream::ReadError;
    using Consolpp::Stream::ReadErrorKind;

    namespace
    {
        using CaughtError = std::optional<std::tuple<ReadErrorKind, std::string>>;
    } // namespace

    TEST(InputStreamTest, ReadsLinesSeparatedByNewlines)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::vector<std::string> expected{"first", "second"};
        std::vector<std::string> received;

        channel.send("first\nsecond\n");
        channel.endWriting();

        // Act
        for (std::optional<std::string_view> line = text.readLine(); line.has_value(); line = text.readLine()) {
            received.emplace_back(*line);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(InputStreamTest, ReturnsTheLastLineWithoutNewline)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::vector<std::string> expected{"first", "last"};
        std::vector<std::string> received;

        channel.send("first\nlast");
        channel.endWriting();

        // Act
        for (std::optional<std::string_view> line = text.readLine(); line.has_value(); line = text.readLine()) {
            received.emplace_back(*line);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(InputStreamTest, StripsCarriageReturns)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::vector<std::string> expected{"first", "second"};
        std::vector<std::string> received;

        channel.send("first\r\nsecond\r\n");
        channel.endWriting();

        // Act
        for (std::optional<std::string_view> line = text.readLine(); line.has_value(); line = text.readLine()) {
            received.emplace_back(*line);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(InputStreamTest, KeepsEmptyLines)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::vector<std::string> expected{"first", "", "third"};
        std::vector<std::string> received;

        channel.send("first\n\nthird\n");
        channel.endWriting();

        // Act
        for (std::optional<std::string_view> line = text.readLine(); line.has_value(); line = text.readLine()) {
            received.emplace_back(*line);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(InputStreamTest, ReturnsNothingForEmptyInput)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};

        channel.endWriting();

        // Act
        const std::optional<std::string_view> line = text.readLine();

        // Assert
        EXPECT_FALSE(line.has_value());
    }

    TEST(InputStreamTest, JoinsALineSplitAcrossReads)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::string first(65000, 'a');
        const std::string second(1000, 'b');
        const std::vector<std::string> expected{first, second};
        std::vector<std::string> received;

        channel.send(first + "\n" + second + "\n");
        channel.endWriting();

        // Act
        for (std::optional<std::string_view> line = text.readLine(); line.has_value(); line = text.readLine()) {
            received.emplace_back(*line);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(InputStreamTest, ReadsALineLongerThanTheBuffer)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::string longLine(200000, 'x');
        const std::vector<std::string> expected{longLine, "after"};
        std::vector<std::string> received;

        channel.send(longLine + "\nafter\n");
        channel.endWriting();

        // Act
        for (std::optional<std::string_view> line = text.readLine(); line.has_value(); line = text.readLine()) {
            received.emplace_back(*line);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(InputStreamTest, ReadWordSkipsWhitespace)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::vector<std::string> expected{"first", "second"};
        std::vector<std::string> received;

        channel.send("  first \t second\n");
        channel.endWriting();

        // Act
        for (std::optional<std::string_view> word = text.readWord(); word.has_value(); word = text.readWord()) {
            received.emplace_back(*word);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(InputStreamTest, ReadWordReturnsNothingForOnlyWhitespace)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};

        channel.send(" \n\t ");
        channel.endWriting();

        // Act
        const std::optional<std::string_view> word = text.readWord();

        // Assert
        EXPECT_FALSE(word.has_value());
    }

    TEST(InputStreamTest, ReadWordJoinsAWordSplitAcrossReads)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const std::string longWord(70000, 'x');
        const std::vector<std::string> expected{longWord, "end"};
        std::vector<std::string> received;

        channel.send(longWord + " end");
        channel.endWriting();

        // Act
        for (std::optional<std::string_view> word = text.readWord(); word.has_value(); word = text.readWord()) {
            received.emplace_back(*word);
        }

        // Assert
        EXPECT_EQ(received, expected);
    }

    TEST(InputStreamTest, ExtractsIntegers)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        constexpr int expectedFirst = 42;
        constexpr int expectedSecond = -7;
        int first = 0;
        int second = 0;

        channel.send("42 -7");
        channel.endWriting();

        // Act
        text >> first >> second;

        // Assert
        EXPECT_EQ(first, expectedFirst);
        EXPECT_EQ(second, expectedSecond);
    }

    TEST(InputStreamTest, ExtractsFloatingPointNumbers)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        constexpr double expected = 1.5;
        double value = 0.0;

        channel.send("1.5");
        channel.endWriting();

        // Act
        text >> value;

        // Assert
        EXPECT_EQ(value, expected);
    }

    TEST(InputStreamTest, ExtractsBooleans)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        bool first = false;
        bool second = true;

        channel.send("true false");
        channel.endWriting();

        // Act
        text >> first >> second;

        // Assert
        EXPECT_TRUE(first);
        EXPECT_FALSE(second);
    }

    TEST(InputStreamTest, ExtractsTheNextCharacterAfterWhitespace)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        char character = '\0';

        channel.send("  xy");
        channel.endWriting();

        // Act
        text >> character;

        // Assert
        EXPECT_EQ(character, 'x');
    }

    TEST(InputStreamTest, ExtractsAWordIntoAString)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        std::string word;

        channel.send("name rest");
        channel.endWriting();

        // Act
        text >> word;

        // Assert
        EXPECT_EQ(word, "name");
    }

    TEST(InputStreamTest, ExtractingThrowsAtTheEndOfInput)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const CaughtError expected{std::in_place, ReadErrorKind::EndOfInput, ""};
        CaughtError caught;
        int value = 0;

        channel.endWriting();

        // Act
        try {
            text >> value;
        } catch (const ReadError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()});
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(InputStreamTest, ExtractingThrowsForAnInvalidValue)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const CaughtError expected{std::in_place, ReadErrorKind::InvalidValue, "abc"};
        CaughtError caught;
        int value = 0;

        channel.send("abc");
        channel.endWriting();

        // Act
        try {
            text >> value;
        } catch (const ReadError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()});
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(InputStreamTest, ExtractingThrowsForAValueOutOfRange)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        const CaughtError expected{std::in_place, ReadErrorKind::OutOfRange, "300"};
        CaughtError caught;
        std::uint8_t value = 0;

        channel.send("300");
        channel.endWriting();

        // Act
        try {
            text >> value;
        } catch (const ReadError& error) {
            caught.emplace(error.getKind(), std::string{error.getSubject()});
        }

        // Assert
        EXPECT_EQ(caught, expected);
    }

    TEST(InputStreamTest, ReadLineAfterAnExtractionReturnsTheRestOfTheLine)
    {
        // Arrange
        const Channel channel;
        InputStream text{channel.getReadEnd()};
        constexpr std::optional<std::string_view> expected{" rest"};
        int number = 0;

        channel.send("42 rest\nnext\n");
        channel.endWriting();

        text >> number;

        // Act
        const std::optional<std::string_view> line = text.readLine();

        // Assert
        EXPECT_EQ(line, expected);
    }

    TEST(InputStreamTest, AskReturnsTheAnswerOfTheField)
    {
        // Arrange
        Consolpp::Event::Loop loop;
        const PseudoTerminal terminal;
        Consolpp::Stream::OutputStream output{terminal.getTerminal(),
                                              Consolpp::Stream::FlushMode::Buffered,
                                              false,
                                              Consolpp::Terminal::Capabilities{}};
        InputStream input{terminal.getTerminal(), output, loop};
        const Consolpp::Prompt::TextField field{Consolpp::Prompt::TextFieldOptions{.label = "Name"}};

        terminal.switchToRaw();
        terminal.type("ab\r");

        // Act
        const std::string answer = input.ask(field);

        // Assert
        EXPECT_EQ(answer, "ab");
    }

    TEST(InputStreamTest, AskTakesTheInputAlreadyReadAsTyped)
    {
        // Arrange
        Consolpp::Event::Loop loop;
        const PseudoTerminal terminal;
        Consolpp::Stream::OutputStream output{terminal.getTerminal(),
                                              Consolpp::Stream::FlushMode::Buffered,
                                              false,
                                              Consolpp::Terminal::Capabilities{}};
        InputStream input{terminal.getTerminal(), output, loop};
        const Consolpp::Prompt::TextField field{Consolpp::Prompt::TextFieldOptions{.label = "Name"}};

        terminal.switchToRaw();
        terminal.type("first\nab\r");
        static_cast<void>(input.readLine());

        // Act
        const std::string answer = input.ask(field);

        // Assert
        EXPECT_EQ(answer, "ab");
    }

    TEST(InputStreamTest, AskThrowsWithoutOutputAndLoop)
    {
        // Arrange
        InputStream input{-1};
        const Consolpp::Prompt::TextField field{Consolpp::Prompt::TextFieldOptions{.label = "Name"}};

        // Act
        const auto ask = [&input, &field] {
            static_cast<void>(input.ask(field));
        };

        // Assert
        EXPECT_THROW(ask(), std::logic_error);
    }

    TEST(InputStreamTest, BindKeyChangesWhatABoundKeyTriggers)
    {
        // Arrange
        const Key key{.code = KeyCode::Tab};

        // Act
        InputStream::bindKey(key, Action::Submit);

        // Assert
        const std::optional<Action> action = findAction(InputStream::getKeyBindings(), key);
        InputStream::setKeyBindings(defaultKeyBindings);
        EXPECT_EQ(action, Action::Submit);
    }

    TEST(InputStreamTest, BindKeyAddsAnUnboundKey)
    {
        // Arrange
        const Key key{.code = KeyCode::Left, .modifiers = Modifiers{.shift = true, .alt = true}};

        // Act
        InputStream::bindKey(key, Action::MoveToStart);

        // Assert
        const std::optional<Action> action = findAction(InputStream::getKeyBindings(), key);
        InputStream::setKeyBindings(defaultKeyBindings);
        EXPECT_EQ(action, Action::MoveToStart);
    }

    TEST(InputStreamTest, IsInteractiveForATerminal)
    {
        // Arrange
        const PseudoTerminal terminal;

        // Act
        const InputStream input{terminal.getTerminal()};

        // Assert
        EXPECT_TRUE(input.isInteractive());
    }

    TEST(InputStreamTest, IsNotInteractiveForAPipe)
    {
        // Arrange
        const Channel channel;

        // Act
        const InputStream input{channel.getReadEnd()};

        // Assert
        EXPECT_FALSE(input.isInteractive());
    }

    TEST(InputStreamTest, AskTakesTheDefaultWhileInteractionIsDisabled)
    {
        // Arrange
        Consolpp::Event::Loop loop;
        const PseudoTerminal terminal;
        Consolpp::Stream::OutputStream output{terminal.getTerminal(),
                                              Consolpp::Stream::FlushMode::Buffered,
                                              false,
                                              Consolpp::Terminal::Capabilities{}};
        InputStream input{terminal.getTerminal(), output, loop};
        const Consolpp::Prompt::TextField field{
            Consolpp::Prompt::TextFieldOptions{.label = "Name", .defaultValue = "consolpp"}};

        input.disableInteraction();

        // Act
        const std::string answer = input.ask(field);

        // Assert
        EXPECT_EQ(answer, "consolpp");
    }

    TEST(InputStreamTest, AskThrowsWithoutADefaultWhileInteractionIsDisabled)
    {
        // Arrange
        Consolpp::Event::Loop loop;
        const PseudoTerminal terminal;
        Consolpp::Stream::OutputStream output{terminal.getTerminal(),
                                              Consolpp::Stream::FlushMode::Buffered,
                                              false,
                                              Consolpp::Terminal::Capabilities{}};
        InputStream input{terminal.getTerminal(), output, loop};
        const Consolpp::Prompt::ConfirmationField field{Consolpp::Prompt::ConfirmationFieldOptions{.label = "Sure?"}};

        input.disableInteraction();

        // Act
        const auto ask = [&input, &field] {
            static_cast<void>(input.ask(field));
        };

        // Assert
        EXPECT_THROW(ask(), Consolpp::Prompt::InteractionDisabledError);
    }

    TEST(InputStreamTest, AskLeavesTheKeysTypedAheadToTheNextField)
    {
        // Arrange
        Consolpp::Event::Loop loop;
        const PseudoTerminal terminal;
        Consolpp::Stream::OutputStream output{terminal.getTerminal(),
                                              Consolpp::Stream::FlushMode::Buffered,
                                              false,
                                              Consolpp::Terminal::Capabilities{}};
        InputStream input{terminal.getTerminal(), output, loop};
        const Consolpp::Prompt::TextField field{Consolpp::Prompt::TextFieldOptions{.label = "Name"}};

        terminal.switchToRaw();
        terminal.type("ab\rcd\r");
        static_cast<void>(input.ask(field));

        // Act
        const std::string answer = input.ask(field);

        // Assert
        EXPECT_EQ(answer, "cd");
    }

    TEST(InputStreamTest, AskReturnsTheAnswersOfAFormInTheirTypes)
    {
        // Arrange
        Consolpp::Event::Loop loop;
        const PseudoTerminal terminal;
        Consolpp::Stream::OutputStream output{terminal.getTerminal(),
                                              Consolpp::Stream::FlushMode::Buffered,
                                              false,
                                              Consolpp::Terminal::Capabilities{}};
        InputStream input{terminal.getTerminal(), output, loop};
        const Consolpp::Prompt::Form form{
            Consolpp::Prompt::TextField{Consolpp::Prompt::TextFieldOptions{.label = "Name"}},
            Consolpp::Prompt::ConfirmationField{Consolpp::Prompt::ConfirmationFieldOptions{.label = "Sure?"}},
        };

        terminal.switchToRaw();
        terminal.type("ab\rn\r");

        // Act
        const std::tuple<std::string, bool> answers = input.ask(form);

        // Assert
        EXPECT_EQ(answers, (std::tuple<std::string, bool>{"ab", false}));
    }

    TEST(InputStreamTest, AskTakesTheDefaultsOfAFormWhileInteractionIsDisabled)
    {
        // Arrange
        Consolpp::Event::Loop loop;
        const PseudoTerminal terminal;
        Consolpp::Stream::OutputStream output{terminal.getTerminal(),
                                              Consolpp::Stream::FlushMode::Buffered,
                                              false,
                                              Consolpp::Terminal::Capabilities{}};
        InputStream input{terminal.getTerminal(), output, loop};
        const Consolpp::Prompt::Form form{
            Consolpp::Prompt::TextField{Consolpp::Prompt::TextFieldOptions{.label = "Name", .defaultValue = "app"}},
            Consolpp::Prompt::ConfirmationField{
                Consolpp::Prompt::ConfirmationFieldOptions{.label = "Sure?", .defaultValue = true}},
        };

        input.disableInteraction();

        // Act
        const std::tuple<std::string, bool> answers = input.ask(form);

        // Assert
        EXPECT_EQ(answers, (std::tuple<std::string, bool>{"app", true}));
    }

    TEST(InputStreamTest, AskThrowsForAFormFieldWithoutADefaultWhileInteractionIsDisabled)
    {
        // Arrange
        Consolpp::Event::Loop loop;
        const PseudoTerminal terminal;
        Consolpp::Stream::OutputStream output{terminal.getTerminal(),
                                              Consolpp::Stream::FlushMode::Buffered,
                                              false,
                                              Consolpp::Terminal::Capabilities{}};
        InputStream input{terminal.getTerminal(), output, loop};
        const Consolpp::Prompt::Form form{
            Consolpp::Prompt::TextField{Consolpp::Prompt::TextFieldOptions{.label = "Name", .defaultValue = "app"}},
            Consolpp::Prompt::ConfirmationField{Consolpp::Prompt::ConfirmationFieldOptions{.label = "Sure?"}},
        };

        input.disableInteraction();

        // Act
        const auto ask = [&input, &form] {
            static_cast<void>(input.ask(form));
        };

        // Assert
        EXPECT_THROW(ask(), Consolpp::Prompt::InteractionDisabledError);
    }
} // namespace Consolpp::Tests::Integration::Stream
