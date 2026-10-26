#include "Consolpp/Signature.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <array>
#include <cstddef>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{

    namespace
    {
        // A failed check inside the consteval constructor is a substitution failure here, not a hard error.
        template <auto CreateOptions, auto CreateArguments>
        concept Constructible = requires {
            typename std::integral_constant<std::size_t,
                                            Signature{CreateOptions(), CreateArguments()}.getOptions().size()>;
        };

        constexpr auto noOptions = [] {
            return std::array<Option, 0>{};
        };

        constexpr auto noArguments = [] {
            return std::array<Argument, 0>{};
        };

        constexpr std::array<std::string_view, 2> twoValues{"first", "second"};
    } // namespace

    TEST(SignatureTest, KeepsTheOptionsInOrder)
    {
        // Arrange
        constexpr std::array options{Option{.name = "first"}, Option{.name = "second"}};
        constexpr std::array<Argument, 0> arguments{};

        // Act
        constexpr Signature signature{options, arguments};

        // Assert
        EXPECT_EQ(std::get<0>(signature.getOptions()).name, "first");
        EXPECT_EQ(std::get<1>(signature.getOptions()).name, "second");
    }

    TEST(SignatureTest, KeepsTheArgumentsInOrder)
    {
        // Arrange
        constexpr std::array<Option, 0> options{};
        constexpr std::array arguments{Argument{.name = "first"}, Argument{.name = "second"}};

        // Act
        constexpr Signature signature{options, arguments};

        // Assert
        EXPECT_EQ(std::get<0>(signature.getArguments()).name, "first");
        EXPECT_EQ(std::get<1>(signature.getArguments()).name, "second");
    }

    TEST(SignatureTest, AcceptsAnEmptySignature)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_TRUE(constructible);
    }

    TEST(SignatureTest, AcceptsAFullSignature)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{
                Option{.name = "loud", .shortcut = 'l'},
                Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
                Option{.name = "tag", .value = OptionValue::Optional, .repeatable = true},
                Option{.name = "force"},
            };
        };
        constexpr auto arguments = [] {
            return std::array{
                Argument{.name = "source"},
                Argument{.name = "target", .required = false},
                Argument{.name = "extra", .required = false, .variadic = true},
            };
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_TRUE(constructible);
    }

    TEST(SignatureTest, RejectsAnEmptyOptionName)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = ""}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsAnOptionNameStartingWithADash)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "-loud"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsAnOptionNameContainingAnEqualsSign)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "out=put"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsADashAsShortcut)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "loud", .shortcut = '-'}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsTheHelpOption)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "help"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsTheHelpShortcut)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "host", .shortcut = 'h'}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsARepeatableOptionWithoutAValue)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "tag", .repeatable = true}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsTwoOptionsWithTheSameName)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "loud"}, Option{.name = "loud"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsTwoOptionsWithTheSameShortcut)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "loud", .shortcut = 'l'}, Option{.name = "version", .shortcut = 'l'}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, AcceptsSeveralOptionsWithoutShortcut)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "loud"}, Option{.name = "dry-run"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_TRUE(constructible);
    }

    TEST(SignatureTest, RejectsAFlagClashingWithItsNegation)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "color"}, Option{.name = "no-color"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsAnEmptyArgumentName)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = ""}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsAVariadicArgumentBeforeTheLast)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "files", .variadic = true}, Argument{.name = "target"}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsARequiredArgumentAfterAnOptionalOne)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "source", .required = false}, Argument{.name = "target"}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsTwoArgumentsWithTheSameName)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "file"}, Argument{.name = "file"}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, AcceptsDefaultsWhereTheyAreAllowed)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{
                Option{.name = "output", .value = OptionValue::Required, .defaultValue = "out.txt"},
                Option{
                    .name = "include",
                    .value = OptionValue::Required,
                    .repeatable = true,
                    .defaultValues = twoValues,
                },
                Option{.name = "color", .defaultEnabled = true},
            };
        };
        constexpr auto arguments = [] {
            return std::array{
                Argument{.name = "source"},
                Argument{.name = "target", .required = false, .defaultValue = "."},
            };
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_TRUE(constructible);
    }

    TEST(SignatureTest, AcceptsDefaultValuesForAnOptionalVariadicArgument)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{
                Argument{.name = "files", .required = false, .variadic = true, .defaultValues = twoValues},
            };
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_TRUE(constructible);
    }

    TEST(SignatureTest, RejectsADefaultValueForAFlag)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "force", .defaultValue = "yes"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsADefaultValueForARepeatableOption)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{
                Option{.name = "include", .value = OptionValue::Required, .repeatable = true, .defaultValue = "a"},
            };
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsDefaultValuesForAnOptionThatIsNotRepeatable)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "output", .value = OptionValue::Required, .defaultValues = twoValues}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsADefaultStateForAnOptionWithAValue)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "output", .value = OptionValue::Required, .defaultEnabled = true}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsADefaultValueForARequiredArgument)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "source", .defaultValue = "."}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsADefaultValueForAVariadicArgument)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "files", .required = false, .variadic = true, .defaultValue = "."}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsDefaultValuesForAnArgumentThatIsNotVariadic)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "target", .required = false, .defaultValues = twoValues}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsDefaultValuesForARequiredVariadicArgument)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "files", .variadic = true, .defaultValues = twoValues}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsTheNameOfAGlobalOption)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "silent"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsTheShortcutOfAGlobalOption)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "never", .shortcut = 'n'}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, AcceptsSuggestedValuesForAnOptionTakingAValue)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "format", .value = OptionValue::Required, .suggestedValues = twoValues}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_TRUE(constructible);
    }

    TEST(SignatureTest, RejectsSuggestedValuesForAFlag)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "force", .suggestedValues = twoValues}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsACompletionModeForAFlag)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "force", .completion = ValueCompletion::File}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsSuggestedValuesAndACompletionModeForAnOption)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{
                Option{
                    .name = "format",
                    .value = OptionValue::Required,
                    .suggestedValues = twoValues,
                    .completion = ValueCompletion::File,
                },
            };
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsSuggestedValuesAndACompletionModeForAnArgument)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{
                Argument{.name = "file", .suggestedValues = twoValues, .completion = ValueCompletion::Path},
            };
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, AcceptsMarkupInTheDescriptions)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "force", .description = "<info>Overwrite</info>"}};
        };
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "source", .description = "The &lt;source&gt;"}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_TRUE(constructible);
    }

    TEST(SignatureTest, RejectsInvalidMarkupInAnOptionDescription)
    {
        // Arrange
        constexpr auto options = [] {
            return std::array{Option{.name = "force", .description = "<info>Overwrite"}};
        };
        constexpr auto arguments = noArguments;

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }

    TEST(SignatureTest, RejectsInvalidMarkupInAnArgumentDescription)
    {
        // Arrange
        constexpr auto options = noOptions;
        constexpr auto arguments = [] {
            return std::array{Argument{.name = "source", .description = "The source</info>"}};
        };

        // Act
        constexpr bool constructible = Constructible<options, arguments>;

        // Assert
        EXPECT_FALSE(constructible);
    }
} // namespace Consolpp::Tests::Unit
