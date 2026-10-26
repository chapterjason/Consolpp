#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Text
{
    // Characters stored in place, without the heap.
    template <std::size_t Capacity>
    class InplaceString
    {
        private:
            std::array<char, Capacity> characters{};
            std::size_t length{0};

        public:
            // Named as std::back_insert_iterator requires.
            using value_type = char; // NOLINT(readability-identifier-naming)

            // Throws std::length_error when the capacity is reached.
            constexpr void push_back(const char character) // NOLINT(readability-identifier-naming)
            {
                if (this->length == Capacity) {
                    throw std::length_error{"The capacity is reached"};
                }

                this->characters.at(this->length) = character;
                ++this->length;
            }

            [[nodiscard]]
            constexpr std::string_view getView() const
            {
                return std::string_view{this->characters.data(), this->length};
            }
    };
} // namespace Consolpp::Text
