#pragma once

#include <cstddef>
#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace Consolpp::Markup
{
    // An error about a subject at a position of the markup; the derived errors give its kind and message.
    class PositionedError : public std::exception
    {
        private:
            std::shared_ptr<const std::string> subject;
            std::shared_ptr<const std::string> message;
            std::size_t position;

        protected:
            PositionedError(const std::string_view subject, std::string message, const std::size_t position)
                : subject{std::make_shared<const std::string>(subject)},
                  message{std::make_shared<const std::string>(std::move(message))},
                  position{position}
            {
            }

        public:
            [[nodiscard]]
            const char* what() const noexcept override
            {
                return this->message->c_str();
            }

            [[nodiscard]]
            std::string_view getSubject() const
            {
                return *this->subject;
            }

            [[nodiscard]]
            std::size_t getPosition() const
            {
                return this->position;
            }
    };
} // namespace Consolpp::Markup
