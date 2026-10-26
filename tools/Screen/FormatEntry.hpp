#pragma once

#include "Screen/EncodeBytes.hpp"
#include "Screen/Entry.hpp"
#include "Screen/EntryKind.hpp"

#include <format>
#include <iterator>
#include <string>

namespace Consolpp::Tools::Screen
{
    // One line without the newline: "<microseconds> output <bytes>" or "<microseconds> resize <columns> <rows>".
    inline std::string formatEntry(const Entry& entry)
    {
        if (entry.kind == EntryKind::Resize) {
            return std::format("{} resize {} {}", entry.time.count(), entry.columns, entry.rows);
        }

        std::string line = std::format("{} output ", entry.time.count());

        Screen::encodeBytes(entry.bytes, std::back_inserter(line));

        return line;
    }
} // namespace Consolpp::Tools::Screen
