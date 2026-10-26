#pragma once

#include "Consolpp/Markup/DecodeErrorKind.hpp"
#include "Consolpp/Markup/PositionedError.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace Consolpp::Markup
{
    class DecodeError : public PositionedError
    {
        private:
            DecodeErrorKind kind;

            static std::string createMessage(const DecodeErrorKind kind,
                                             const std::string_view subject,
                                             const std::size_t position)
            {
                const std::string quoted = "\"" + std::string{subject} + "\"";
                const std::string at = " at position " + std::to_string(position);

                switch (kind) {
                    case DecodeErrorKind::UnknownEntity:
                        return "The entity " + quoted + at + " does not exist";
                    case DecodeErrorKind::UnterminatedEntity:
                        return "The entity" + at + " has no closing \";\"";
                    case DecodeErrorKind::InvalidCharacterReference:
                        return "The character reference " + quoted + at + " is invalid";
                }

                return "The text" + at + " cannot be decoded";
            }

        public:
            DecodeError(const DecodeErrorKind kind, const std::string_view subject, const std::size_t position)
                : PositionedError{subject, DecodeError::createMessage(kind, subject, position), position},
                  kind{kind}
            {
            }

            [[nodiscard]]
            DecodeErrorKind getKind() const
            {
                return this->kind;
            }
    };
} // namespace Consolpp::Markup
