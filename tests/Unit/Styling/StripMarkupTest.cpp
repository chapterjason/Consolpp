#include "Consolpp/Styling/StripMarkup.hpp"

#include "Consolpp/Markup/ElementError.hpp"

#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Markup::ElementError;
    using Consolpp::Styling::stripMarkup;

    TEST(StripMarkupTest, WritesOnlyTheText)
    {
        // Arrange
        constexpr std::string_view markup = R"(a<info>b</info><link href="https://example.com">c</link>)";
        std::string written;

        // Act
        stripMarkup(markup, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "abc");
    }

    TEST(StripMarkupTest, ResolvesEntities)
    {
        // Arrange
        constexpr std::string_view markup = "&lt;caf&#233;&gt;";
        std::string written;

        // Act
        stripMarkup(markup, std::back_inserter(written));

        // Assert
        EXPECT_EQ(written, "<café>");
    }

    TEST(StripMarkupTest, WritesNothingForInvalidMarkup)
    {
        // Arrange
        constexpr std::string_view markup = "fine <bogus>x</bogus>";
        std::string written;
        bool thrown = false;

        // Act
        try {
            stripMarkup(markup, std::back_inserter(written));
        } catch (const ElementError&) {
            thrown = true;
        }

        // Assert
        EXPECT_TRUE(thrown);
        EXPECT_TRUE(written.empty());
    }
} // namespace Consolpp::Tests::Unit::Styling
