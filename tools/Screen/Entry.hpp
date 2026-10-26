#pragma once

#include "Screen/EntryKind.hpp"

#include <chrono>
#include <cstddef>
#include <string>

namespace Consolpp::Tools::Screen
{
    // Something that happened to the recorded terminal: bytes the program wrote, or a new size of the window.
    struct Entry
    {
        public:
            // Since the recording started.
            std::chrono::microseconds time{0};
            EntryKind kind{EntryKind::Output};
            std::string bytes{};
            std::size_t columns{0};
            std::size_t rows{0};

            bool operator==(const Entry& other) const = default;
    };
} // namespace Consolpp::Tools::Screen
