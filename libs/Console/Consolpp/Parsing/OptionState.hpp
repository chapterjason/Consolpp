#pragma once

#include "Consolpp/Parsing/OptionPresence.hpp"

#include <cstdint>
#include <string_view>

namespace Consolpp::Parsing
{
    struct OptionState
    {
        public:
            std::string_view value;
            OptionPresence presence{OptionPresence::Absent};
            bool valued{false};
            std::uint32_t firstPosition{0};
    };
} // namespace Consolpp::Parsing
