#include "Consolpp/Prompt/FormatChoiceLines.hpp"

#include "Consolpp/Prompt/Action.hpp"
#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ChoiceFieldOptions.hpp"
#include "Consolpp/Prompt/Selection.hpp"

#include <array>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Action;
    using Consolpp::Prompt::ChoiceField;
    using Consolpp::Prompt::ChoiceFieldOptions;
    using Consolpp::Prompt::formatChoiceLines;
    using Consolpp::Prompt::Selection;

    namespace
    {
        constexpr std::array<std::string_view, 2> choices{"a<b>", "c"};
    } // namespace

    TEST(FormatChoiceLinesTest, HighlightsAChoiceAndEncodesTheChoices)
    {
        // Arrange
        const ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices}};
        std::string formatted;

        // Act
        formatChoiceLines(field, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "  <prompt-highlight>&gt; a&lt;b&gt;</prompt-highlight>\n    c");
    }

    TEST(FormatChoiceLinesTest, MarksTheSelectionOfAMultipleSelection)
    {
        // Arrange
        ChoiceField field{ChoiceFieldOptions{.label = "Fruit", .choices = choices, .selection = Selection::Multiple}};
        std::string formatted;

        field.apply(Action::MoveDown);
        field.apply(Action::Toggle);

        // Act
        formatChoiceLines(field, std::back_inserter(formatted));

        // Assert
        EXPECT_EQ(formatted, "    [ ] a&lt;b&gt;\n  <prompt-highlight>&gt; [x] c</prompt-highlight>");
    }
} // namespace Consolpp::Tests::Unit::Prompt
