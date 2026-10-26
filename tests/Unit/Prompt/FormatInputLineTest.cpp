#include "Consolpp/Prompt/FormatInputLine.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/Autocompletion.hpp"
#include "Consolpp/Prompt/ControlCharacters.hpp"
#include "Consolpp/Prompt/Echo.hpp"
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
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::Autocompletion;
    using Consolpp::Prompt::ControlCharacters;
    using Consolpp::Prompt::Echo;
    using Consolpp::Prompt::formatInputLine;
    using Consolpp::Prompt::TextField;
    using Consolpp::Prompt::TextFieldOptions;

    namespace
    {
        constexpr std::size_t wide = 20;
        constexpr std::size_t narrow = 4;
        constexpr std::array<std::string_view, 2> names{"conductor", "consolpp"};
    } // namespace

    TEST(FormatInputLineTest, EncodesTheText)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};
        std::string beforeCursor;
        std::string afterCursor;

        field.type(U'<');
        field.type(U'a');

        // Act
        formatInputLine(field, wide, std::back_inserter(beforeCursor), std::back_inserter(afterCursor));

        // Assert
        EXPECT_EQ(beforeCursor, "&lt;a");
        EXPECT_EQ(afterCursor, "");
    }

    TEST(FormatInputLineTest, SplitsAtTheCursor)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};
        std::string beforeCursor;
        std::string afterCursor;

        field.type(U'a');
        field.type(U'b');
        field.type(U'c');
        field.apply(Action::MoveLeft);
        field.apply(Action::MoveLeft);

        // Act
        formatInputLine(field, wide, std::back_inserter(beforeCursor), std::back_inserter(afterCursor));

        // Assert
        EXPECT_EQ(beforeCursor, "a");
        EXPECT_EQ(afterCursor, "bc");
    }

    TEST(FormatInputLineTest, MasksEveryGrapheme)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .echo = Echo::Masked}};
        std::string beforeCursor;
        std::string afterCursor;

        field.type(U'a');
        field.type(U'b');
        field.type(U'c');
        field.apply(Action::MoveLeft);

        // Act
        formatInputLine(field, wide, std::back_inserter(beforeCursor), std::back_inserter(afterCursor));

        // Assert
        EXPECT_EQ(beforeCursor, "**");
        EXPECT_EQ(afterCursor, "*");
    }

    TEST(FormatInputLineTest, WritesNothingForHiddenText)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .echo = Echo::Hidden}};
        std::string beforeCursor;
        std::string afterCursor;

        field.type(U'a');
        field.type(U'b');
        field.type(U'c');

        // Act
        formatInputLine(field, wide, std::back_inserter(beforeCursor), std::back_inserter(afterCursor));

        // Assert
        EXPECT_EQ(beforeCursor, "");
        EXPECT_EQ(afterCursor, "");
    }

    TEST(FormatInputLineTest, ScrollsWithTheCursor)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};
        std::string beforeCursor;
        std::string afterCursor;

        field.type(U'a');
        field.type(U'b');
        field.type(U'c');
        field.type(U'd');
        field.type(U'e');
        field.type(U'f');

        // Act
        formatInputLine(field, narrow, std::back_inserter(beforeCursor), std::back_inserter(afterCursor));

        // Assert
        EXPECT_EQ(beforeCursor, "def");
        EXPECT_EQ(afterCursor, "");
    }

    TEST(FormatInputLineTest, AddsTheMutedSuggestion)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name", .autocompletion = Autocompletion::fromValues(names)}};
        std::string beforeCursor;
        std::string afterCursor;

        field.type(U'c');
        field.type(U'o');
        field.type(U'n');
        field.type(U'd');

        // Act
        formatInputLine(field, wide, std::back_inserter(beforeCursor), std::back_inserter(afterCursor));

        // Assert
        EXPECT_EQ(beforeCursor, "cond");
        EXPECT_EQ(afterCursor, "<prompt-suggestion>uctor</prompt-suggestion>");
    }

    TEST(FormatInputLineTest, ShowsKeptControlCharactersAsReplacementCharacters)
    {
        // Arrange
        TextField field{TextFieldOptions{.label = "Name"}};
        std::string beforeCursor;
        std::string afterCursor;

        TextField::setControlCharacters(ControlCharacters::Keep);
        field.paste("a\x1b"
                    "b");
        TextField::setControlCharacters(ControlCharacters::Drop);

        // Act
        formatInputLine(field, wide, std::back_inserter(beforeCursor), std::back_inserter(afterCursor));

        // Assert
        EXPECT_EQ(beforeCursor,
                  "a\xEF\xBF\xBD"
                  "b");
        EXPECT_EQ(afterCursor, "");
    }
} // namespace Consolpp::Tests::Unit::Prompt
