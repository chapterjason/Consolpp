#include "Consolpp/GetRunArguments.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/CommandList.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/RunArgument.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    using Consolpp::Parsing::Parameters;

    namespace
    {
        class CommandTakingEverything
        {
            private:
                int exitCode{0};

            public:
                static constexpr Definition definition{
                    .name = "deploy",
                    .signature = Signature{std::array<Option, 0>{}, std::array<Argument, 0>{}},
                };

                [[nodiscard]]
                int run(Event::Loop& /*loop*/,
                        Consolpp::Stream::OutputStream& /*output*/,
                        const Parameters<definition>& /*parameters*/,
                        Consolpp::Stream::ErrorOutputStream& /*errorOutput*/,
                        Consolpp::Stream::InputStream& /*input*/,
                        const CommandList& /*commands*/) const
                {
                    return this->exitCode;
                }
        };

        class CommandTakingNothing
        {
            private:
                int calls{0};

            public:
                static constexpr Definition definition{
                    .name = "status",
                    .signature = Signature{std::array<Option, 0>{}, std::array<Argument, 0>{}},
                };

                [[nodiscard]]
                int run()
                {
                    ++this->calls;

                    return this->calls;
                }
        };
    } // namespace

    TEST(GetRunArgumentsTest, MapsEveryParameterInTheOrderRunTakesThem)
    {
        // Arrange
        using Type = CommandTakingEverything;
        constexpr std::array<RunArgument, 6> expected{
            RunArgument::Loop,
            RunArgument::Output,
            RunArgument::Parameters,
            RunArgument::ErrorOutput,
            RunArgument::Input,
            RunArgument::Commands,
        };

        // Act
        constexpr std::array<RunArgument, 6> runArguments = getRunArguments<Type>(&Type::run);

        // Assert
        EXPECT_EQ(runArguments, expected);
    }

    TEST(GetRunArgumentsTest, MapsNothingForARunWithoutParameters)
    {
        // Arrange
        using Type = CommandTakingNothing;

        // Act
        constexpr std::array<RunArgument, 0> runArguments = getRunArguments<Type>(&Type::run);

        // Assert
        EXPECT_TRUE(runArguments.empty());
    }
} // namespace Consolpp::Tests::Unit
