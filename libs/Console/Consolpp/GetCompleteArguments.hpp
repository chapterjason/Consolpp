#pragma once

#include "Consolpp/CommandList.hpp"
#include "Consolpp/Completion/Request.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/RunArgument.hpp"

#include <array>
#include <concepts>
#include <stdexcept>
#include <string>
#include <vector>

namespace Consolpp
{
    // complete takes the request first, then any of the commands and the program; it cannot write to the output, which
    // carries the answer to the shell.
    template <typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getCompleteArguments()
    {
        [[maybe_unused]]
        const auto getCompleteArgument = []<typename Argument>() -> RunArgument {
            if constexpr (std::same_as<Argument, const CommandList&>) {
                return RunArgument::Commands;
            } else if constexpr (std::same_as<Argument, const Program&>) {
                return RunArgument::Program;
            } else {
                throw std::invalid_argument{"complete takes a parameter that is not passed to it"};
            }
        };

        return std::array<RunArgument, sizeof...(Arguments)>{getCompleteArgument.template operator()<Arguments>()...};
    }

    // complete must be a single function returning the values, a member or a static one, like run.
    template <typename Owner, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getCompleteArguments(
        std::vector<std::string> (Owner::* /*complete*/)(const Completion::Request&, Arguments...))
    {
        return Consolpp::getCompleteArguments<Arguments...>();
    }

    template <typename Owner, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getCompleteArguments(
        std::vector<std::string> (Owner::* /*complete*/)(const Completion::Request&, Arguments...) const)
    {
        return Consolpp::getCompleteArguments<Arguments...>();
    }

    template <typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getCompleteArguments(
        std::vector<std::string> (* /*complete*/)(const Completion::Request&, Arguments...))
    {
        return Consolpp::getCompleteArguments<Arguments...>();
    }
} // namespace Consolpp
