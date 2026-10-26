#pragma once

#include "Consolpp/Text/CodePointRange.hpp"

#include <array>

namespace Consolpp::Text
{
    // Code points with Default_Ignorable_Code_Point, generated from DerivedCoreProperties.txt of Unicode 18.0.0.
    inline constexpr std::array<CodePointRange, 17> defaultIgnorableRanges{
        {
            {.first = 0xAD, .last = 0xAD},
            {.first = 0x34F, .last = 0x34F},
            {.first = 0x61C, .last = 0x61C},
            {.first = 0x115F, .last = 0x1160},
            {.first = 0x17B4, .last = 0x17B5},
            {.first = 0x180B, .last = 0x180F},
            {.first = 0x200B, .last = 0x200F},
            {.first = 0x202A, .last = 0x202E},
            {.first = 0x2060, .last = 0x206F},
            {.first = 0x3164, .last = 0x3164},
            {.first = 0xFE00, .last = 0xFE0F},
            {.first = 0xFEFF, .last = 0xFEFF},
            {.first = 0xFFA0, .last = 0xFFA0},
            {.first = 0xFFF0, .last = 0xFFF8},
            {.first = 0x1BCA0, .last = 0x1BCA3},
            {.first = 0x1D173, .last = 0x1D17A},
            {.first = 0xE0000, .last = 0xE0FFF},
        },
    };
} // namespace Consolpp::Text
