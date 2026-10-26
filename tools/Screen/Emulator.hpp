#pragma once

#include "Screen/Cell.hpp"
#include "Screen/Row.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Tools::Screen
{
    // A small terminal that knows what Consolpp writes: text with automatic wrapping, carriage return, line feed,
    // cursor movement and erasing. Styles, links and other sequences are skipped. A resize reflows the text the way
    // terminals like Windows Terminal or VTE do: wrapped rows are joined and wrapped again at the new width.
    class Emulator
    {
        private:
            std::size_t columns;
            std::size_t rows;
            // The scrollback followed by the rows of the screen.
            std::vector<Row> buffer;
            std::size_t cursorRow{0};
            std::size_t cursorColumn{0};
            // The last column was written; the next character goes to the next row.
            bool wrapPending{false};
            // The start of a sequence or character the next write completes.
            std::string incomplete;

            [[nodiscard]]
            std::size_t getTop() const;

            void feedLine();

            void put(std::string_view text, std::size_t width);

            void handleControl(std::string_view parameters, char command);

            // The number of bytes used; zero when the text ends before the sequence or character does.
            std::size_t handle(std::string_view text);

        public:
            Emulator(std::size_t columns, std::size_t rows);

            void write(std::string_view bytes);

            void resize(std::size_t columns, std::size_t rows);

            // The screen as text between borders, the cursor row marked; with the scrollback above it.
            [[nodiscard]]
            std::string render(bool scrollback) const;
    };
} // namespace Consolpp::Tools::Screen
