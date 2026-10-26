#pragma once

#include "Consolpp/CommandErrorKind.hpp"
#include "Consolpp/SuggestionRange.hpp"

#include <exception>
#include <memory>
#include <span>
#include <string>
#include <string_view>

namespace Consolpp
{
    class CommandError : public std::exception
    {
        private:
            std::shared_ptr<const std::string> subject;
            std::shared_ptr<const std::string> message;
            std::span<const std::string_view> candidates;
            bool (*isSuggestion)(std::string_view, std::string_view);
            CommandErrorKind kind;

            static std::string createMessage(const CommandErrorKind kind, const std::string_view subject)
            {
                const std::string quoted = '"' + std::string{subject} + '"';

                switch (kind) {
                    case CommandErrorKind::UnknownCommand:
                        return "The command " + quoted + " does not exist";
                    case CommandErrorKind::UnknownNamespace:
                        return "No command is in the namespace " + quoted;
                }

                return "The command is invalid";
            }

        public:
            CommandError(const CommandErrorKind kind, const std::string_view subject)
                : subject{std::make_shared<const std::string>(subject)},
                  message{std::make_shared<const std::string>(CommandError::createMessage(kind, subject))},
                  isSuggestion{nullptr},
                  kind{kind}
            {
            }

            // The suggestions are only computed while they are iterated.
            CommandError(const CommandErrorKind kind,
                         const std::string_view subject,
                         const std::span<const std::string_view> candidates,
                         bool (*const isSuggestion)(std::string_view, std::string_view))
                : subject{std::make_shared<const std::string>(subject)},
                  message{std::make_shared<const std::string>(CommandError::createMessage(kind, subject))},
                  candidates{candidates},
                  isSuggestion{isSuggestion},
                  kind{kind}
            {
            }

            [[nodiscard]]
            const char* what() const noexcept override
            {
                return this->message->c_str();
            }

            [[nodiscard]]
            CommandErrorKind getKind() const
            {
                return this->kind;
            }

            [[nodiscard]]
            std::string_view getSubject() const
            {
                return *this->subject;
            }

            [[nodiscard]]
            SuggestionRange createSuggestionRange() const
            {
                return SuggestionRange{*this->subject, this->candidates, this->isSuggestion};
            }
    };
} // namespace Consolpp
