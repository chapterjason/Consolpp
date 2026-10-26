#include "Consolpp/Completion/FormatBashScript.hpp"

#include <iterator>
#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Completion
{
    using Consolpp::Completion::formatBashScript;

    TEST(FormatBashScriptTest, DefinesTheFunctionAndRegistersItForTheProgram)
    {
        // Arrange
        std::string script;

        // Act
        formatBashScript("tool", std::back_inserter(script));

        // Assert
        EXPECT_NE(script.find("\n_tool_complete()\n{\n"), std::string::npos);
        EXPECT_TRUE(script.ends_with("\n}\n\ncomplete -F _tool_complete 'tool'\n"));
    }

    TEST(FormatBashScriptTest, AsksTheProgramWithTheWordsAfterItsName)
    {
        // Arrange
        std::string script;

        // Act
        formatBashScript("tool", std::back_inserter(script));

        // Assert
        EXPECT_NE(script.find(R"bash("${words[0]}" _complete "$((current - 1))" -- "${words[@]:1}")bash"),
                  std::string::npos);
    }

    TEST(FormatBashScriptTest, ReplacesCharactersAFunctionNameCannotHold)
    {
        // Arrange
        std::string script;

        // Act
        formatBashScript("my-tool.v2", std::back_inserter(script));

        // Assert
        EXPECT_NE(script.find("\n_my_tool_v2_complete()\n"), std::string::npos);
        EXPECT_TRUE(script.ends_with("complete -F _my_tool_v2_complete 'my-tool.v2'\n"));
    }

    TEST(FormatBashScriptTest, QuotesASingleQuoteInTheProgramName)
    {
        // Arrange
        std::string script;

        // Act
        formatBashScript("it's", std::back_inserter(script));

        // Assert
        EXPECT_TRUE(script.ends_with(R"(complete -F _it_s_complete 'it'\''s')"
                                     "\n"));
    }
} // namespace Consolpp::Tests::Unit::Completion
