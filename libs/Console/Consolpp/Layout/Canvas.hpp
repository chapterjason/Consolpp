#pragma once

#include "Consolpp/Styling/MarkupLine.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Layout
{
    // What elements draw into: lines of markup, each self-contained, with their display width.
    class Canvas
    {
        private:
            std::string text;
            std::vector<Styling::MarkupLine> lines;

        public:
            // Starts an empty line that the following appends go into.
            void addLine();

            // The markup must be self-contained and as wide as given.
            void append(std::string_view markup, std::size_t width);

            void appendSpaces(std::size_t count);

            // Escapes the text, as border characters may be markup characters; every character takes one column.
            void appendText(std::string_view text);

            [[nodiscard]]
            std::size_t getCount() const
            {
                return this->lines.size();
            }

            // Throws std::out_of_range for an index past the last line.
            [[nodiscard]]
            std::string_view getLine(std::size_t index) const;

            // Throws std::out_of_range for an index past the last line.
            [[nodiscard]]
            std::size_t getWidth(std::size_t index) const;

            void clear();
    };
} // namespace Consolpp::Layout
