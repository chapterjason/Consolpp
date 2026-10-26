#include "Consolpp/Prompt/Field.hpp"

#include "Consolpp/Prompt/ChoiceField.hpp"
#include "Consolpp/Prompt/ConfirmationField.hpp"
#include "Consolpp/Prompt/TextField.hpp"

#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Prompt
{
    using Consolpp::Prompt::ChoiceField;
    using Consolpp::Prompt::ConfirmationField;
    using Consolpp::Prompt::Field;
    using Consolpp::Prompt::TextField;

    TEST(FieldTest, IsSatisfiedByTheFields)
    {
        // Act
        constexpr bool satisfied = Field<TextField> && Field<ChoiceField> && Field<ConfirmationField>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(FieldTest, IsNotSatisfiedByAnyOtherType)
    {
        // Act
        constexpr bool satisfied = Field<std::string>;

        // Assert
        EXPECT_FALSE(satisfied);
    }
} // namespace Consolpp::Tests::Unit::Prompt
