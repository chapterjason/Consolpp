#pragma once

#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Screen/CursorPlacement.hpp"
#include "Consolpp/Screen/Section.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"

#include <concepts>
#include <cstddef>
#include <deque>
#include <functional>
#include <span>
#include <vector>

namespace Consolpp::Screen
{
    // Lines that leave the window cannot be cleared anymore. A narrower window makes the terminal wrap the drawn lines
    // before the stack can draw them again, so the stack keeps them within the height limit at the reflow width and
    // leaves out its first lines as needed.
    class SectionStack
    {
        private:
            Stream::OutputStream* output;
            std::deque<Section> sections;
            // Gets the rows of the window and gives the rows all sections may take together; empty for no limit.
            std::function<std::size_t(std::size_t)> heightLimit;
            // Gets the columns of the window and gives the width the lines must still fit at; empty for the current.
            std::function<std::size_t(std::size_t)> reflowWidth;
            CursorPlacement cursorPlacement{CursorPlacement::Top};
            // The first lines of the stack the last draw left out to keep within the height limit.
            std::size_t hiddenLines{0};
            // The rows the last draw took at its width.
            std::size_t drawnRows{0};
            // Decorated output may still be a file or pipe, where moving the cursor writes garbage.
            bool terminal;

            friend class Section;

            [[nodiscard]]
            std::size_t getWidth() const;

            // The rows the lines not left out take at the width.
            [[nodiscard]]
            std::size_t countRows(std::size_t width) const;

            // The first lines of the stack to leave out so that the rest keeps within the height limit at the reflow
            // width.
            [[nodiscard]]
            std::size_t findHiddenLines(std::size_t width) const;

            void drawLines(std::span<const Markup::ParsedMarkup<Markup::ElementStyle>> lines);

            // Clears the old lines, which take the given rows at the current width, and draws all lines.
            void redraw(std::size_t previousRows);

            // Applies the change and draws all lines again; without a terminal, only withoutTerminal runs.
            template <std::invocable<> Change, std::invocable<> WithoutTerminal>
            void update(const Change& change, const WithoutTerminal& withoutTerminal);

            void append(Section& section, std::vector<Markup::ParsedMarkup<Markup::ElementStyle>> lines);

            void overwrite(Section& section, std::vector<Markup::ParsedMarkup<Markup::ElementStyle>> lines);

            void clear(Section& section);

            void setMaxHeight(Section& section, std::size_t maxHeight);

        public:
            explicit SectionStack(Stream::OutputStream& output);

            SectionStack(const SectionStack&) = delete;
            SectionStack(SectionStack&&) = delete;
            SectionStack& operator=(const SectionStack&) = delete;
            SectionStack& operator=(SectionStack&&) = delete;

            // Moves the cursor below the stack, so the output after it does not overwrite it.
            ~SectionStack();

            Section& createSection();

            // The limit gets the rows of the window and returns the rows all sections may take together. Without a
            // limit the stack grows freely. The default leaves the row of the cursor free.
            void setHeightLimit(std::function<std::size_t(std::size_t)> limit);

            // The function gets the columns of the window and returns the narrowest width the lines must still fit
            // into the height limit at. Without one, the lines are counted at the current width only. The default is
            // half the columns, which holds for any narrowing that keeps at least half the width.
            void setReflowWidth(std::function<std::size_t(std::size_t)> width);

            // Moves the cursor to the new place right away. The default is CursorPlacement::Top.
            void setCursorPlacement(CursorPlacement placement);
    };
} // namespace Consolpp::Screen
