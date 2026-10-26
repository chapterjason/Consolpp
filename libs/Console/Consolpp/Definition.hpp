#pragma once

#include "Consolpp/Signature.hpp"

#include <array>
#include <cstddef>
#include <string_view>

namespace Consolpp
{
    template <std::size_t OptionCount,
              std::size_t ArgumentCount,
              std::size_t AliasCount = 0,
              std::size_t UsageCount = 0>
    struct Definition
    {
        public:
            std::string_view name;
            std::string_view description{};
            std::array<std::string_view, AliasCount> aliases{};
            std::string_view help{};
            std::array<std::string_view, UsageCount> usages{};
            bool hidden{false};
            Signature<OptionCount, ArgumentCount> signature;
    };
} // namespace Consolpp
