#include "Consolpp/Command.hpp"

#include "Consolpp/Argument.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Definition.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Signature.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <array>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    using Consolpp::Parsing::Parameters;

    namespace
    {
        constexpr Signature signature{
            std::array{Option{.name = "force", .shortcut = 'f'}},
            std::array{Argument{.name = "source"}},
        };

        constexpr Signature otherSignature{
            std::array{Option{.name = "loud"}},
            std::array<Argument, 0>{},
        };

        constexpr Definition otherDefinition{.name = "deploy", .signature = otherSignature};

        class ValidCommand
        {
            private:
                int exitCode{0};

            public:
                static constexpr Definition definition{.name = "deploy", .signature = signature};

                [[nodiscard]]
                int run(const Parameters<definition>& /*parameters*/,
                        Consolpp::Stream::InputStream& /*input*/,
                        Consolpp::Stream::OutputStream& /*output*/,
                        Consolpp::Stream::ErrorOutputStream& /*errorOutput*/,
                        Event::Loop& /*loop*/) const
                {
                    return this->exitCode;
                }
        };

        class CommandWithoutDefinition
        {
            private:
                int exitCode{0};

            public:
                [[nodiscard]]
                int run() const
                {
                    return this->exitCode;
                }
        };

        class CommandWithoutRun
        {
            public:
                static constexpr Definition definition{.name = "deploy", .signature = signature};
        };

        class CommandRunningWithoutExitCode
        {
            private:
                int calls{0};

            public:
                static constexpr Definition definition{.name = "deploy", .signature = signature};

                void run()
                {
                    ++this->calls;
                }
        };

        class CommandRunningWithAnotherSignature
        {
            private:
                int exitCode{0};

            public:
                static constexpr Definition definition{.name = "deploy", .signature = signature};

                [[nodiscard]]
                int run(const Parameters<otherDefinition>& /*parameters*/) const
                {
                    return this->exitCode;
                }
        };

        class CommandWithAnInvalidDefinition
        {
            private:
                int exitCode{0};

            public:
                static constexpr Definition definition{.name = "", .signature = signature};

                [[nodiscard]]
                int run() const
                {
                    return this->exitCode;
                }
        };

        class CommandRunningWithoutParameters
        {
            private:
                int exitCode{0};

            public:
                static constexpr Definition definition{.name = "deploy", .signature = signature};

                [[nodiscard]]
                int run() const
                {
                    return this->exitCode;
                }
        };

        constexpr Signature computedSignature{
            std::array<Option, 0>{},
            std::array{Argument{.name = "environment", .completion = ValueCompletion::Computed}},
        };

        class CommandComputingValues
        {
            public:
                static constexpr Definition definition{.name = "deploy", .signature = computedSignature};

                [[nodiscard]]
                static int run()
                {
                    return 0;
                }

                [[nodiscard]]
                static std::vector<std::string> complete(const Completion::Request& /*request*/)
                {
                    return std::vector<std::string>{"production"};
                }
        };

        class CommandWithComputedValuesButNoComplete
        {
            public:
                static constexpr Definition definition{.name = "deploy", .signature = computedSignature};

                [[nodiscard]]
                static int run()
                {
                    return 0;
                }
        };

        class CommandCompletingWithAnOutput
        {
            public:
                static constexpr Definition definition{.name = "deploy", .signature = computedSignature};

                [[nodiscard]]
                static int run()
                {
                    return 0;
                }

                [[nodiscard]]
                static std::vector<std::string> complete(const Completion::Request& /*request*/,
                                                         Stream::OutputStream& /*output*/)
                {
                    return std::vector<std::string>{};
                }
        };

        class CommandRunningWithAnUnknownParameter
        {
            private:
                int exitCode{0};

            public:
                static constexpr Definition definition{.name = "deploy", .signature = signature};

                [[nodiscard]]
                int run(int& /*value*/) const
                {
                    return this->exitCode;
                }
        };
    } // namespace

    TEST(CommandTest, IsSatisfiedByAValidDefinitionAndARunReturningTheExitCode)
    {
        // Arrange
        using Type = ValidCommand;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(CommandTest, IsNotSatisfiedWithoutADefinition)
    {
        // Arrange
        using Type = CommandWithoutDefinition;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_FALSE(satisfied);
    }

    TEST(CommandTest, IsNotSatisfiedWithoutARun)
    {
        // Arrange
        using Type = CommandWithoutRun;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_FALSE(satisfied);
    }

    TEST(CommandTest, IsNotSatisfiedByARunWithoutExitCode)
    {
        // Arrange
        using Type = CommandRunningWithoutExitCode;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_FALSE(satisfied);
    }

    TEST(CommandTest, IsNotSatisfiedByARunTakingTheInputOfAnotherSignature)
    {
        // Arrange
        using Type = CommandRunningWithAnotherSignature;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_FALSE(satisfied);
    }

    TEST(CommandTest, IsNotSatisfiedByAnInvalidDefinition)
    {
        // Arrange
        using Type = CommandWithAnInvalidDefinition;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_FALSE(satisfied);
    }

    TEST(CommandTest, IsSatisfiedByARunTakingNoParameters)
    {
        // Arrange
        using Type = CommandRunningWithoutParameters;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(CommandTest, IsNotSatisfiedByARunTakingAnUnknownParameter)
    {
        // Arrange
        using Type = CommandRunningWithAnUnknownParameter;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_FALSE(satisfied);
    }

    TEST(CommandTest, IsSatisfiedByComputedValuesWithAComplete)
    {
        // Arrange
        using Type = CommandComputingValues;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_TRUE(satisfied);
    }

    TEST(CommandTest, IsNotSatisfiedByComputedValuesWithoutAComplete)
    {
        // Arrange
        using Type = CommandWithComputedValuesButNoComplete;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_FALSE(satisfied);
    }

    TEST(CommandTest, IsNotSatisfiedByACompleteTakingTheOutput)
    {
        // Arrange
        using Type = CommandCompletingWithAnOutput;

        // Act
        constexpr bool satisfied = Command<Type>;

        // Assert
        EXPECT_FALSE(satisfied);
    }
} // namespace Consolpp::Tests::Unit
