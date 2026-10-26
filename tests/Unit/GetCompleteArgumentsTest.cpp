#include "Consolpp/GetCompleteArguments.hpp"

#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/RunArgument.hpp"

#include <array>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit
{
    namespace
    {
        class CommandCompletingWithEverything
        {
            private:
                std::vector<std::string> values{"production"};

            public:
                [[nodiscard]]
                std::vector<std::string> complete(const Completion::Request& /*request*/,
                                                  const Program& /*program*/,
                                                  const CommandList& /*commands*/) const
                {
                    return this->values;
                }
        };

        class CommandCompletingWithTheRequest
        {
            public:
                [[nodiscard]]
                static std::vector<std::string> complete(const Completion::Request& /*request*/)
                {
                    return std::vector<std::string>{};
                }
        };
    } // namespace

    TEST(GetCompleteArgumentsTest, MapsTheParametersAfterTheRequestInTheOrderCompleteTakesThem)
    {
        // Arrange
        constexpr auto complete = &CommandCompletingWithEverything::complete;

        // Act
        constexpr auto arguments = Consolpp::getCompleteArguments(complete);

        // Assert
        EXPECT_EQ(arguments, (std::array{RunArgument::Program, RunArgument::Commands}));
    }

    TEST(GetCompleteArgumentsTest, MapsNothingForACompleteTakingOnlyTheRequest)
    {
        // Arrange
        constexpr auto complete = &CommandCompletingWithTheRequest::complete;

        // Act
        constexpr auto arguments = Consolpp::getCompleteArguments(complete);

        // Assert
        EXPECT_TRUE(arguments.empty());
    }
} // namespace Consolpp::Tests::Unit
