#pragma once

#include "Screen/Cell.hpp"

#include <vector>

namespace Consolpp::Tools::Screen
{
    struct Row
    {
        public:
            std::vector<Cell> cells{};
            // The text ran on into the next row; a reflow joins the two again.
            bool wrapped{false};

            bool operator==(const Row& other) const = default;
    };
} // namespace Consolpp::Tools::Screen
