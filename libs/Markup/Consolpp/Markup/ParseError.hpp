#pragma once

#include "Consolpp/Markup/ParseErrorKind.hpp"
#include "Consolpp/Markup/PositionedError.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace Consolpp::Markup
{
    class ParseError : public PositionedError
    {
        private:
            ParseErrorKind kind;

            static std::string createMessage(const ParseErrorKind kind,
                                             const std::string_view subject,
                                             const std::size_t position)
            {
                const std::string quoted = "\"" + std::string{subject} + "\"";
                const std::string at = " at position " + std::to_string(position);

                switch (kind) {
                    case ParseErrorKind::MalformedTag:
                        return "The tag " + quoted + at + " is malformed";
                    case ParseErrorKind::DuplicateAttribute:
                        return "The attribute " + quoted + at + " is given more than once";
                    case ParseErrorKind::UnexpectedClosingTag:
                        return "The closing tag " + quoted + at + " has no opening tag";
                    case ParseErrorKind::MismatchedClosingTag:
                        return "The closing tag " + quoted + at + " does not match the opening tag";
                    case ParseErrorKind::UnclosedElement:
                        return "The element " + quoted + at + " is not closed";
                }

                return "The markup" + at + " cannot be parsed";
            }

        public:
            ParseError(const ParseErrorKind kind, const std::string_view subject, const std::size_t position)
                : PositionedError{subject, ParseError::createMessage(kind, subject, position), position},
                  kind{kind}
            {
            }

            [[nodiscard]]
            ParseErrorKind getKind() const
            {
                return this->kind;
            }
    };
} // namespace Consolpp::Markup
