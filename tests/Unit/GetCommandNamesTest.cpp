#include "Consolpp/GetCommandNames.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Signature.hpp"

#include <array>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    namespace
    {
        constexpr Signature signature{
            std::array<Option, 0>{},
            std::array<Argument, 0>{},
        };

        template <const auto& CommandDefinition>
        class FakeCommand
        {
            private:
                int exitCode{0};

            public:
                static constexpr const auto& definition = CommandDefinition;

                [[nodiscard]]
                int run() const
                {
                    return this->exitCode;
                }
        };

        constexpr Definition deploy{
            .name = "deploy",
            .aliases = std::array<std::string_view, 1>{"d"},
            .signature = signature,
        };
        constexpr Definition status{
            .name = "status",
            .aliases = std::array<std::string_view, 1>{"s"},
            .signature = signature,
        };
        constexpr Definition debug{
            .name = "debug",
            .aliases = std::array<std::string_view, 1>{"x"},
            .hidden = true,
            .signature = signature,
        };

        using Deploy = FakeCommand<deploy>;
        using Status = FakeCommand<status>;
        using Debug = FakeCommand<debug>;
    } // namespace

    TEST(GetCommandNamesTest, ListsTheNamesBeforeTheAliases)
    {
        // Arrange
        constexpr std::array<std::string_view, 4> expected{"deploy", "status", "d", "s"};

        // Act
        constexpr auto names = getCommandNames<false, Deploy, Status>();

        // Assert
        EXPECT_EQ(names, expected);
    }

    TEST(GetCommandNamesTest, LeavesHiddenCommandsOut)
    {
        // Arrange
        constexpr std::array<std::string_view, 2> expected{"deploy", "d"};

        // Act
        constexpr auto names = getCommandNames<false, Deploy, Debug>();

        // Assert
        EXPECT_EQ(names, expected);
    }

    TEST(GetCommandNamesTest, IncludesHiddenCommandsWhenAsked)
    {
        // Arrange
        constexpr std::array<std::string_view, 4> expected{"deploy", "debug", "d", "x"};

        // Act
        constexpr auto names = getCommandNames<true, Deploy, Debug>();

        // Assert
        EXPECT_EQ(names, expected);
    }
} // namespace Consolpp::Tests::Unit
