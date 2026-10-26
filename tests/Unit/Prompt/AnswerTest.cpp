#include "Consolpp/Prompt/Answer.hpp"

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/TextField.hpp"

#include <concepts>
#include <cstddef>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::Answer;
    using Consolpp::Prompt::ChoiceField;
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::TextField;

    TEST(AnswerTest, IsWhatAskingTheFieldReturns)
    {
        // Act
        constexpr bool text = std::same_as<Answer<TextField>, std::string>;
        constexpr bool choice = std::same_as<Answer<ChoiceField>, std::vector<std::size_t>>;
        constexpr bool confirmation = std::same_as<Answer<ConfirmationField>, bool>;

        // Assert
        EXPECT_TRUE(text);
        EXPECT_TRUE(choice);
        EXPECT_TRUE(confirmation);
    }
} // namespace Consolpp::Tests::Unit::Prompt
