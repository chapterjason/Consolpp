#pragma once

namespace Consolpp::Text
{
    // The control characters of Unicode (general category Cc): C0, DEL and C1.
    constexpr bool isControl(const char32_t character)
    {
        constexpr char32_t lastC0 = 0x1F;
        constexpr char32_t deleteCharacter = 0x7F;
        constexpr char32_t lastC1 = 0x9F;

        return character <= lastC0 || (character >= deleteCharacter && character <= lastC1);
    }
} // namespace Consolpp::Text
