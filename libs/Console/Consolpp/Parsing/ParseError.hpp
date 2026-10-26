#pragma once

#include "Consolpp/Parsing/ParseErrorKind.hpp"

#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <string_view>

namespace Consolpp::Parsing
{
    class ParseError : public std::exception
    {
        private:
            std::shared_ptr<const std::string> subject;
            std::shared_ptr<const std::string> message;
            ParseErrorKind kind;
            std::uint8_t dashes;

            static std::string createMessage(const ParseErrorKind kind,
                                             const std::string_view subject,
                                             const std::uint8_t dashes)
            {
                const std::string option = '"' + std::string(dashes, '-') + std::string{subject} + '"';
                const std::string argument = '"' + std::string{subject} + '"';

                switch (kind) {
                    case ParseErrorKind::UnknownOption:
                        return "The option " + option + " does not exist";
                    case ParseErrorKind::MissingValue:
                        return "The option " + option + " requires a value";
                    case ParseErrorKind::UnexpectedValue:
                        return "The option " + option + " does not accept a value";
                    case ParseErrorKind::DuplicateOption:
                        return "The option " + option + " is given more than once";
                    case ParseErrorKind::MissingArgument:
                        return "The argument " + argument + " is missing";
                    case ParseErrorKind::TooManyArguments:
                        return "The argument " + argument + " is not expected";
                }

                return "The input is invalid";
            }

        public:
            ParseError(const ParseErrorKind kind, const std::string_view subject, const std::uint8_t dashes)
                : subject{std::make_shared<const std::string>(subject)},
                  message{std::make_shared<const std::string>(ParseError::createMessage(kind, subject, dashes))},
                  kind{kind},
                  dashes{dashes}
            {
            }

            [[nodiscard]]
            const char* what() const noexcept override
            {
                return this->message->c_str();
            }

            [[nodiscard]]
            ParseErrorKind getKind() const
            {
                return this->kind;
            }

            [[nodiscard]]
            std::string_view getSubject() const
            {
                return *this->subject;
            }

            [[nodiscard]]
            std::uint8_t getDashes() const
            {
                return this->dashes;
            }
    };
} // namespace Consolpp::Parsing
