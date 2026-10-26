#pragma once

#include "Consolpp/Environment/GetValue.hpp"

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace Consolpp::Tests::Integration
{
    // Unsets every variable Consolpp reads, sets the given ones, and restores all of them when it ends.
    class EnvironmentScope
    {
        private:
            static constexpr std::array<const char*, 14> names{
                "NO_COLOR",
                "FORCE_COLOR",
                "TERM_PROGRAM",
                "COLORTERM",
                "TERM",
                "TERMINAL_EMULATOR",
                "IDEA_INITIAL_DIRECTORY",
                "KONSOLE_VERSION",
                "COLUMNS",
                "LINES",
                "WAYLAND_DISPLAY",
                "DISPLAY",
                "SSH_CONNECTION",
                "PATH",
            };

            std::array<std::pair<const char*, std::optional<std::string>>, names.size()> previous;

            void restore() const
            {
                for (const auto& [name, value] : this->previous) {
                    if (value.has_value()) {
                        setenv(name, value->c_str(), 1); // NOLINT(concurrency-mt-unsafe)
                    } else {
                        unsetenv(name); // NOLINT(concurrency-mt-unsafe)
                    }
                }
            }

        public:
            explicit EnvironmentScope(const std::map<std::string, std::string, std::less<>>& variables)
            {
                // The tests run on one thread, so nothing reads the environment while it changes.
                for (std::size_t index = 0; index < names.size(); ++index) {
                    const char* const name = names.at(index);
                    const std::optional<std::string_view> value = Environment::getValue(name);

                    this->previous.at(index).first = name;

                    if (value.has_value()) {
                        this->previous.at(index).second = std::string{*value};
                    }
                }

                try {
                    for (const char* const name : names) {
                        if (unsetenv(name) != 0) { // NOLINT(concurrency-mt-unsafe)
                            throw std::system_error{errno, std::generic_category()};
                        }
                    }

                    for (const auto& [name, value] : variables) {
                        if (setenv(name.c_str(), value.c_str(), 1) != 0) { // NOLINT(concurrency-mt-unsafe)
                            throw std::system_error{errno, std::generic_category()};
                        }
                    }
                } catch (const std::system_error&) {
                    this->restore();

                    throw;
                }
            }

            EnvironmentScope(const EnvironmentScope&) = delete;
            EnvironmentScope(EnvironmentScope&&) = delete;
            EnvironmentScope& operator=(const EnvironmentScope&) = delete;
            EnvironmentScope& operator=(EnvironmentScope&&) = delete;

            ~EnvironmentScope()
            {
                this->restore();
            }
    };
} // namespace Consolpp::Tests::Integration
