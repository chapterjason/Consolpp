#pragma once

#include "Consolpp/Stream/ReadErrorKind.hpp"

#include <exception>
#include <memory>
#include <string>
#include <string_view>

namespace Consolpp::Stream
{
    class ReadError : public std::exception
    {
        private:
            std::shared_ptr<const std::string> subject;
            ReadErrorKind kind;

        public:
            ReadError(const ReadErrorKind kind, const std::string_view subject)
                : subject{std::make_shared<const std::string>(subject)},
                  kind{kind}
            {
            }

            [[nodiscard]]
            const char* what() const noexcept override
            {
                switch (this->kind) {
                    case ReadErrorKind::EndOfInput:
                        return "The input ended";
                    case ReadErrorKind::InvalidValue:
                        return "The value is invalid";
                    case ReadErrorKind::OutOfRange:
                        return "The value is out of range";
                }

                return "The input could not be read";
            }

            [[nodiscard]]
            ReadErrorKind getKind() const
            {
                return this->kind;
            }

            [[nodiscard]]
            std::string_view getSubject() const
            {
                return *this->subject;
            }
    };
} // namespace Consolpp::Stream
