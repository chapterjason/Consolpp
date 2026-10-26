#pragma once

#include "Consolpp/Completion/Result.hpp"
#include "Consolpp/ValueCompletion.hpp"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Consolpp::Completion
{
    // The answer to a completion request, one line each: "words" and the suggestions, or "files", "directories" or
    // "paths" and the value prefix. Throws std::invalid_argument for Computed paths.
    template <std::output_iterator<char> Output>
    constexpr Output formatResult(const Result& result, Output output)
    {
        const auto writeLine = [&output](const std::string_view line) {
            output = std::ranges::copy(line, output).out;
            *output = '\n';
            ++output;
        };

        switch (result.paths) {
            case ValueCompletion::None:
                writeLine("words");

                for (const std::string& suggestion : result.suggestions) {
                    writeLine(suggestion);
                }

                return output;
            case ValueCompletion::File:
                writeLine("files");
                break;
            case ValueCompletion::Directory:
                writeLine("directories");
                break;
            case ValueCompletion::Path:
                writeLine("paths");
                break;
            case ValueCompletion::Computed:
                throw std::invalid_argument{"Computed values are suggestions, not paths"};
        }

        writeLine(result.valuePrefix);

        return output;
    }
} // namespace Consolpp::Completion
