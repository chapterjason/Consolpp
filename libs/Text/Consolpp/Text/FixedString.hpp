#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <string_view>

namespace Consolpp::Text
{
    template <std::size_t Size>
    struct FixedString
    {
        public:
            std::array<char, Size> characters;

            // Implicit and taking a C array, so that a string literal can be used as a template argument.
            // NOLINTNEXTLINE(*-explicit-constructor,*-avoid-c-arrays)
            consteval FixedString(const char (&text)[Size + 1])
            {
                std::copy_n(std::begin(text), Size, this->characters.begin());
            }

            [[nodiscard]]
            constexpr std::string_view getView() const
            {
                return std::string_view{this->characters.begin(), this->characters.end()};
            }
    };

    template <std::size_t Size>
    // NOLINTNEXTLINE(*-avoid-c-arrays)
    FixedString(const char (&text)[Size]) -> FixedString<Size - 1>;
} // namespace Consolpp::Text
