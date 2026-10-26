#pragma once

#include "Consolpp/Parsing/TokenIterator.hpp"

#include <iterator>
#include <span>

namespace Consolpp::Parsing
{
    class TokenRange
    {
        private:
            std::span<const char* const> arguments;

        public:
            explicit TokenRange(const std::span<const char* const> arguments)
                : arguments{arguments}
            {
            }

            // Leaves out argv[0], the program name; argv must end with the null pointer that main receives.
            [[nodiscard]]
            static TokenRange fromArgv(const char* const* const argv)
            {
                const char* const* end = argv;

                while (*end != nullptr) {
                    end = std::next(end);
                }

                const std::span<const char* const> arguments{argv, end};

                return TokenRange{arguments.empty() ? arguments : arguments.subspan(1)};
            }

            [[nodiscard]]
            std::span<const char* const> getArguments() const
            {
                return this->arguments;
            }

            [[nodiscard]]
            TokenIterator begin() const
            {
                return TokenIterator{this->arguments.begin()};
            }

            [[nodiscard]]
            TokenIterator end() const
            {
                return TokenIterator{this->arguments.end()};
            }
    };
} // namespace Consolpp::Parsing
