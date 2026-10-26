#include "Consolpp/Formatting/FormatSynopsis.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/SynopsisForm.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/OptionValue.hpp"
#include "Consolpp/Signature.hpp"

#include <array>
#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Formatting
{
    using Consolpp::Formatting::formatSynopsis;
    using Consolpp::Formatting::SynopsisForm;

    namespace
    {
        constexpr Definition full{
            .name = "deploy",
            .signature = Signature{std::array{
                                       Option{.name = "force", .shortcut = 'f'},
                                       Option{.name = "output", .shortcut = 'o', .value = OptionValue::Required},
                                       Option{.name = "tag", .value = OptionValue::Optional},
                                   },
                                   std::array{
                                       Argument{.name = "source"},
                                       Argument{.name = "target", .required = false},
                                       Argument{.name = "rest", .required = false, .variadic = true},
                                   }},
        };

        constexpr Definition bare{
            .name = "status",
            .signature = Signature{std::array<Option, 0>{}, std::array<Argument, 0>{}},
        };
    } // namespace

    TEST(FormatSynopsisTest, WritesEveryOptionInTheLongForm)
    {
        // Arrange
        constexpr DefinitionView command = DefinitionView::fromDefinition(full);
        std::string written;

        // Act
        formatSynopsis(command, SynopsisForm::Long, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written,
                  "deploy [-h|--help] [-f|--force|--no-force] [-o|--output OUTPUT] [--tag [TAG]] [--] <source> "
                  "[<target> [<rest>...]]");
    }

    TEST(FormatSynopsisTest, WritesOptionsAsAPlaceholderInTheShortForm)
    {
        // Arrange
        constexpr DefinitionView command = DefinitionView::fromDefinition(full);
        std::string written;

        // Act
        formatSynopsis(command, SynopsisForm::Short, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "deploy [options] [--] <source> [<target> [<rest>...]]");
    }

    TEST(FormatSynopsisTest, WritesOnlyTheHelpForACommandWithoutSignature)
    {
        // Arrange
        constexpr DefinitionView command = DefinitionView::fromDefinition(bare);
        std::string written;

        // Act
        formatSynopsis(command, SynopsisForm::Long, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "status [-h|--help]");
    }
} // namespace Consolpp::Tests::Unit::Formatting
