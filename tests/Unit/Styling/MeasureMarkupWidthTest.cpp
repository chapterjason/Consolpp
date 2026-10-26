#include "Consolpp/Styling/MeasureMarkupWidth.hpp"

#include <cstddef>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Styling
{
    using Consolpp::Styling::measureMarkupWidth;

    TEST(MeasureMarkupWidthTest, MeasuresTheTextBeforeTheOffset)
    {
        // Arrange
        constexpr std::string_view markup{"<info>a&lt;</info>bc"};
        const std::size_t offset = markup.find('c');

        // Act
        const std::size_t width = measureMarkupWidth(markup, offset);

        // Assert
        EXPECT_EQ(width, 3U);
    }

    TEST(MeasureMarkupWidthTest, CountsWideCharactersTwice)
    {
        // Arrange
        constexpr std::string_view markup{"中<info>x</info>"};

        // Act
        const std::size_t width = measureMarkupWidth(markup, markup.size());

        // Assert
        EXPECT_EQ(width, 3U);
    }
} // namespace Consolpp::Tests::Unit::Styling
