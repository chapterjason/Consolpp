#pragma once

#include "Consolpp/Parsing/TokenKind.hpp"

#include <cstdint>
#include <optional>
#include <string_view>

namespace Consolpp::Parsing
{
    class Token
    {
        private:
            const char* start;
            std::uint32_t argumentLength;
            std::uint32_t nameLength;
            TokenKind kind;
            std::uint8_t dashes;

        public:
            Token(const TokenKind kind, const std::string_view argument, const std::uint8_t dashes)
                : start{argument.data()},
                  argumentLength{static_cast<std::uint32_t>(argument.size())},
                  nameLength{static_cast<std::uint32_t>(argument.size() - dashes)},
                  kind{kind},
                  dashes{dashes}
            {
            }

            Token(const TokenKind kind,
                  const std::string_view argument,
                  const std::uint8_t dashes,
                  const std::uint32_t nameLength)
                : start{argument.data()},
                  argumentLength{static_cast<std::uint32_t>(argument.size())},
                  nameLength{nameLength},
                  kind{kind},
                  dashes{dashes}
            {
            }

            [[nodiscard]]
            TokenKind getKind() const
            {
                return this->kind;
            }

            [[nodiscard]]
            std::string_view getArgument() const
            {
                return std::string_view{this->start, this->argumentLength};
            }

            [[nodiscard]]
            std::string_view getName() const
            {
                std::string_view name{this->start, this->dashes + this->nameLength};

                name.remove_prefix(this->dashes);

                return name;
            }

            [[nodiscard]]
            std::optional<std::string_view> getValue() const
            {
                const std::uint32_t valueStart = this->dashes + this->nameLength + 1U;

                if (valueStart > this->argumentLength) {
                    return std::nullopt;
                }

                std::string_view value{this->start, this->argumentLength};

                value.remove_prefix(valueStart);

                return value;
            }
    };
} // namespace Consolpp::Parsing
