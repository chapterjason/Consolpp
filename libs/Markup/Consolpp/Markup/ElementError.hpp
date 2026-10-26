#pragma once

#include "Consolpp/Markup/ElementErrorKind.hpp"
#include "Consolpp/Markup/PositionedError.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace Consolpp::Markup
{
    class ElementError : public PositionedError
    {
        private:
            ElementErrorKind kind;

            static std::string createMessage(const ElementErrorKind kind,
                                             const std::string_view subject,
                                             const std::size_t position)
            {
                const std::string quoted = "\"" + std::string{subject} + "\"";
                const std::string at = " at position " + std::to_string(position);

                switch (kind) {
                    case ElementErrorKind::UnknownElement:
                        return "The element " + quoted + at + " does not exist";
                    case ElementErrorKind::UnknownAttribute:
                        return "The attribute " + quoted + at + " is not allowed on its element";
                    case ElementErrorKind::MissingAttribute:
                        return "The attribute " + quoted + " is missing on the element" + at;
                    case ElementErrorKind::InvalidAttributeValue:
                        return "The attribute value " + quoted + at + " is invalid";
                }

                return "The element" + at + " is invalid";
            }

        public:
            ElementError(const ElementErrorKind kind, const std::string_view subject, const std::size_t position)
                : PositionedError{subject, ElementError::createMessage(kind, subject, position), position},
                  kind{kind}
            {
            }

            [[nodiscard]]
            ElementErrorKind getKind() const
            {
                return this->kind;
            }
    };
} // namespace Consolpp::Markup
