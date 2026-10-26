#include "Consolpp/HasUniqueCommandNames.hpp"

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
        constexpr Signature signature{std::array<Option, 0>{}, std::array<Argument, 0>{}};

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
        constexpr Definition status{.name = "status", .signature = signature};
        constexpr Definition shortStatus{.name = "d", .signature = signature};
        constexpr Definition secondDeploy{.name = "deploy", .signature = signature};

        using Deploy = FakeCommand<deploy>;
        using Status = FakeCommand<status>;
        using ShortStatus = FakeCommand<shortStatus>;
        using SecondDeploy = FakeCommand<secondDeploy>;
    } // namespace

    TEST(HasUniqueCommandNamesTest, AcceptsDistinctNamesAndAliases)
    {
        // Arrange
        using First = Deploy;
        using Second = Status;

        // Act
        constexpr bool unique = hasUniqueCommandNames<First, Second>();

        // Assert
        EXPECT_TRUE(unique);
    }

    TEST(HasUniqueCommandNamesTest, RejectsANameEqualToAnotherAlias)
    {
        // Arrange
        using First = Deploy;
        using Second = ShortStatus;

        // Act
        constexpr bool unique = hasUniqueCommandNames<First, Second>();

        // Assert
        EXPECT_FALSE(unique);
    }

    TEST(HasUniqueCommandNamesTest, RejectsTwoEqualNames)
    {
        // Arrange
        using First = Deploy;
        using Second = SecondDeploy;

        // Act
        constexpr bool unique = hasUniqueCommandNames<First, Second>();

        // Assert
        EXPECT_FALSE(unique);
    }
} // namespace Consolpp::Tests::Unit
