#pragma once

#include "Consolpp/CommandList.hpp"
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/RunArgument.hpp"
#include "Consolpp/Stream/ErrorOutputStream.hpp"
#include "Consolpp/Stream/InputStream.hpp"
#include "Consolpp/Stream/OutputStream.hpp"

#include <array>
#include <concepts>
#include <stdexcept>

namespace Consolpp
{
    template <typename Type, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getRunArguments()
    {
        [[maybe_unused]]
        const auto getRunArgument = []<typename Argument>() -> RunArgument {
            if constexpr (std::same_as<Argument, const Parsing::Parameters<Type::definition>&>
                          || std::same_as<Argument, Parsing::Parameters<Type::definition>&>) {
                return RunArgument::Parameters;
            } else if constexpr (std::same_as<Argument, Stream::InputStream&>) {
                return RunArgument::Input;
            } else if constexpr (std::same_as<Argument, Stream::OutputStream&>) {
                return RunArgument::Output;
            } else if constexpr (std::same_as<Argument, Stream::ErrorOutputStream&>) {
                return RunArgument::ErrorOutput;
            } else if constexpr (std::same_as<Argument, Event::Loop&>) {
                return RunArgument::Loop;
            } else if constexpr (std::same_as<Argument, const CommandList&>) {
                return RunArgument::Commands;
            } else if constexpr (std::same_as<Argument, const Program&>) {
                return RunArgument::Program;
            } else {
                throw std::invalid_argument{"run or interact takes a parameter that is not passed to commands"};
            }
        };

        return std::array<RunArgument, sizeof...(Arguments)>{getRunArgument.template operator()<Arguments>()...};
    }

    // run must be a single function returning int, a member or a static one; overloads and templates cannot be read.
    template <typename Type, typename Owner, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getRunArguments(int (Owner::* /*run*/)(Arguments...))
    {
        return Consolpp::getRunArguments<Type, Arguments...>();
    }

    template <typename Type, typename Owner, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getRunArguments(int (Owner::* /*run*/)(Arguments...) const)
    {
        return Consolpp::getRunArguments<Type, Arguments...>();
    }

    template <typename Type, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getRunArguments(int (* /*run*/)(Arguments...))
    {
        return Consolpp::getRunArguments<Type, Arguments...>();
    }

    // interact must be a single function returning void, a member or a static one, like run.
    template <typename Type, typename Owner, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getInteractArguments(
        void (Owner::* /*interact*/)(Arguments...))
    {
        return Consolpp::getRunArguments<Type, Arguments...>();
    }

    template <typename Type, typename Owner, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getInteractArguments(
        void (Owner::* /*interact*/)(Arguments...) const)
    {
        return Consolpp::getRunArguments<Type, Arguments...>();
    }

    template <typename Type, typename... Arguments>
    consteval std::array<RunArgument, sizeof...(Arguments)> getInteractArguments(void (* /*interact*/)(Arguments...))
    {
        return Consolpp::getRunArguments<Type, Arguments...>();
    }
} // namespace Consolpp
