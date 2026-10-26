#include "Consolpp/HasComputedValues.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <array>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    TEST(HasComputedValuesTest, FindsAnOptionWithComputedValues)
    {
        // Arrange
        constexpr Definition definition{
            .name = "deploy",
            .signature =
                Signature{
                    std::array{
                        Option{
                            .name = "environment",
                            .value = OptionValue::Required,
                            .completion = ValueCompletion::Computed,
                        },
                    },
                    std::array<Argument, 0>{},
                },
        };

        // Act
        constexpr bool computed = Consolpp::hasComputedValues(definition);

        // Assert
        EXPECT_TRUE(computed);
    }

    TEST(HasComputedValuesTest, FindsAnArgumentWithComputedValues)
    {
        // Arrange
        constexpr Definition definition{
            .name = "deploy",
            .signature =
                Signature{
                    std::array<Option, 0>{},
                    std::array{Argument{.name = "environment", .completion = ValueCompletion::Computed}},
                },
        };

        // Act
        constexpr bool computed = Consolpp::hasComputedValues(definition);

        // Assert
        EXPECT_TRUE(computed);
    }

    TEST(HasComputedValuesTest, FindsNoneForOtherCompletionModes)
    {
        // Arrange
        constexpr Definition definition{
            .name = "deploy",
            .signature =
                Signature{
                    std::array<Option, 0>{},
                    std::array{Argument{.name = "file", .completion = ValueCompletion::File}},
                },
        };

        // Act
        constexpr bool computed = Consolpp::hasComputedValues(definition);

        // Assert
        EXPECT_FALSE(computed);
    }
} // namespace Consolpp::Tests::Unit
