#include "Consolpp/Text/ReplaceControlCharacters.hpp"

#include <string>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Text
{
    using Consolpp::Text::replaceControlCharacters;

    TEST(ReplaceControlCharactersTest, ReplacesControlCharactersAndInvalidUtf8)
    {
        // Act
        const std::string replaced = replaceControlCharacters("a\x1b"
                                                              "b\xFF"
                                                              "cä");

        // Assert
        EXPECT_EQ(replaced,
                  "a\xEF\xBF\xBD"
                  "b\xEF\xBF\xBD"
                  "cä");
    }
} // namespace Consolpp::Tests::Unit::Text
