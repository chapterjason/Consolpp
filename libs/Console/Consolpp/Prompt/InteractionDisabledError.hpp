#pragma once

#include <exception>

namespace Consolpp::Prompt
{
    // Thrown when a field is asked while interaction is disabled and the field has no default value to take.
    class InteractionDisabledError : public std::exception
    {
        public:
            [[nodiscard]]
            const char* what() const noexcept override
            {
                return "Interaction is disabled and the field has no default value";
            }
    };
} // namespace Consolpp::Prompt
