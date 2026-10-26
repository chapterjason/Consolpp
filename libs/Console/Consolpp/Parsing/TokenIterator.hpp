#pragma once

#include "Consolpp/Parsing/Token.hpp"

#include <span>

namespace Consolpp::Parsing
{
    class TokenIterator
    {
        private:
            std::span<const char* const>::iterator current;

        public:
            explicit TokenIterator(const std::span<const char* const>::iterator current)
                : current{current}
            {
            }

            [[nodiscard]]
            Token operator*() const;

            TokenIterator& operator++()
            {
                ++this->current;

                return *this;
            }

            [[nodiscard]]
            bool operator==(const TokenIterator& other) const = default;
    };
} // namespace Consolpp::Parsing
