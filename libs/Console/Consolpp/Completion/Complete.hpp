#pragma once

#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/ComputeValues.hpp"
#include "Consolpp/Completion/Result.hpp"

#include <cstddef>
#include <span>
#include <string_view>

namespace Consolpp::Completion
{
    // The completion of the word at current, whose suggestions start with what is typed of it; the words follow the
    // program name. The single command takes all words; empty without one. Throws std::out_of_range when current is not
    // a word.
    Result complete(const CommandList& commands,
                    std::string_view singleCommand,
                    std::span<const char* const> words,
                    std::size_t current,
                    const ComputeValues& computeValues);
} // namespace Consolpp::Completion
