#include "Consolpp/Markup/EncodingIterator.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace Consolpp::Tests::Unit::Markup
{
    using Consolpp::Markup::EncodingIterator;

    TEST(EncodingIteratorTest, EncodesEveryCharacterWrittenThroughIt)
    {
        // Arrange
        constexpr std::string_view text = "a<b & 'c'";
        std::string encoded;

        // Act
        std::ranges::copy(text, EncodingIterator{std::back_inserter(encoded)});

        // Assert
        EXPECT_EQ(encoded, "a&lt;b &amp; &apos;c&apos;");
    }

    TEST(EncodingIteratorTest, BaseReturnsTheWrappedIterator)
    {
        // Arrange
        std::string encoded;
        EncodingIterator iterator{std::back_inserter(encoded)};

        // Act
        *iterator = '>';
        *iterator.base() = '!';

        // Assert
        EXPECT_EQ(encoded, "&gt;!");
    }
} // namespace Consolpp::Tests::Unit::Markup
