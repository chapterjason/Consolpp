#pragma once

#include "Screen/DecodeBytes.hpp"
#include "Screen/Entry.hpp"
#include "Screen/EntryKind.hpp"

#include <charconv>
#include <chrono>
#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <system_error>

namespace Consolpp::Tools::Screen
{
    // Reverses formatEntry(). Throws std::invalid_argument for a line in another form.
    inline Entry parseEntry(const std::string_view line)
    {
        std::string_view rest = line;
        const auto takeNumber = [&rest]() -> std::size_t {
            std::size_t number = 0;
            const char* const first = std::to_address(rest.begin());
            const auto [stop, error] = std::from_chars(first, std::to_address(rest.end()), number);

            if (error != std::errc{}) {
                throw std::invalid_argument{"An entry must start with a number"};
            }

            rest.remove_prefix(static_cast<std::size_t>(stop - first));

            return number;
        };
        const auto takeWord = [&rest](const std::string_view word) {
            if (!rest.starts_with(word)) {
                return false;
            }

            rest.remove_prefix(word.size());

            return true;
        };
        Entry entry{.time = std::chrono::microseconds{takeNumber()}};

        if (takeWord(" output ")) {
            Screen::decodeBytes(rest, std::back_inserter(entry.bytes));

            return entry;
        }

        if (!takeWord(" resize ")) {
            throw std::invalid_argument{"An entry must be an output or a resize"};
        }

        entry.kind = EntryKind::Resize;
        entry.columns = takeNumber();

        if (!takeWord(" ")) {
            throw std::invalid_argument{"A resize must give the columns and the rows"};
        }

        entry.rows = takeNumber();

        return entry;
    }
} // namespace Consolpp::Tools::Screen
