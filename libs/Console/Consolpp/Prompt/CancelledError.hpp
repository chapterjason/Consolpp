#pragma once

#include <exception>

namespace Consolpp::Prompt
{
    // Thrown when the user cancels a prompt or the input ends before it is submitted.
    class CancelledError : public std::exception
    {
        public:
            [[nodiscard]]
            const char* what() const noexcept override
            {
                return "The prompt was cancelled";
            }
    };
} // namespace Consolpp::Prompt
