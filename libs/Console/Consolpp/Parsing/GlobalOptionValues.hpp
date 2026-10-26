#pragma once

#include "Consolpp/Stream/Verbosity.hpp"

#include <optional>

namespace Consolpp::Parsing
{
    // What the global options in argv ask for; help and version are read separately.
    struct GlobalOptionValues
    {
        public:
            std::optional<Stream::Verbosity> verbosity{};
            bool interactionDisabled{false};
    };
} // namespace Consolpp::Parsing
