#include "Consolpp/Screen/SectionStack.hpp"

#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Screen/CountRows.hpp"
#include "Consolpp/Screen/CursorPlacement.hpp"
#include "Consolpp/Screen/Section.hpp"
#include "Consolpp/Stream/BufferingScope.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/SynchronizedUpdate.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <span>
#include <system_error>
#include <utility>
#include <vector>

namespace Consolpp::Screen
{
    SectionStack::SectionStack(Stream::OutputStream& output)
        : output{&output},
          heightLimit{[](const std::size_t rows) {
              return rows > 1 ? rows - 1 : rows;
          }},
          reflowWidth{[](const std::size_t columns) {
              return columns / 2;
          }},
          terminal{output.isTerminal()}
    {
    }

    SectionStack::~SectionStack()
    {
        if (!this->terminal || this->cursorPlacement != CursorPlacement::Top || this->drawnRows == 0) {
            return;
        }

        try {
            this->output->writeRaw("\x1b[");
            *this->output << this->drawnRows;
            this->output->writeRaw("B");
            this->output->flush();
            // A destructor cannot throw; like the stream at its end, a failed write is dropped.
            // NOLINTNEXTLINE(bugprone-empty-catch)
        } catch (const std::system_error&) {
        }
    }

    Section& SectionStack::createSection()
    {
        return this->sections.emplace_back(*this, Section::Key{});
    }

    std::size_t SectionStack::getWidth() const
    {
        return this->output->getColumns();
    }

    std::size_t SectionStack::countRows(const std::size_t width) const
    {
        std::size_t rows = 0;
        std::size_t position = 0;

        for (const Section& section : this->sections) {
            for (const Markup::ParsedMarkup<Markup::ElementStyle>& line : section.getVisibleLines()) {
                if (position >= this->hiddenLines) {
                    rows += Screen::countRows(line.getText(), width);
                }

                ++position;
            }
        }

        return rows;
    }

    std::size_t SectionStack::findHiddenLines(const std::size_t width) const
    {
        if (!this->heightLimit) {
            return 0;
        }

        const std::size_t limit = this->heightLimit(this->output->getRows());
        const std::size_t narrowest = this->reflowWidth ? std::max<std::size_t>(this->reflowWidth(width), 1) : width;
        std::size_t rows = 0;
        std::size_t hidden = 0;

        for (const Section& section : this->sections) {
            for (const Markup::ParsedMarkup<Markup::ElementStyle>& line : section.getVisibleLines()) {
                rows += Screen::countRows(line.getText(), narrowest);
            }
        }

        for (const Section& section : this->sections) {
            for (const Markup::ParsedMarkup<Markup::ElementStyle>& line : section.getVisibleLines()) {
                if (rows <= limit) {
                    return hidden;
                }

                rows -= Screen::countRows(line.getText(), narrowest);
                ++hidden;
            }
        }

        return hidden;
    }

    void SectionStack::drawLines(const std::span<const Markup::ParsedMarkup<Markup::ElementStyle>> lines)
    {
        for (const Markup::ParsedMarkup<Markup::ElementStyle>& line : lines) {
            *this->output << line << '\n';
        }
    }

    void SectionStack::redraw(const std::size_t previousRows)
    {
        const std::size_t width = this->getWidth();
        const bool top = this->cursorPlacement == CursorPlacement::Top;
        std::size_t position = 0;

        this->hiddenLines = this->findHiddenLines(width);

        const std::size_t rows = this->countRows(width);

        if (this->drawnRows == 0 && rows == 0) {
            return;
        }

        {
            const Terminal::SynchronizedUpdate update{*this->output};

            // Before the first draw the cursor may stand behind other text, which stays.
            if (top && this->drawnRows > 0) {
                this->output->writeRaw("\r\x1b[0J");
            } else if (!top && previousRows > 0) {
                this->output->writeRaw("\r\x1b[");
                *this->output << previousRows;
                this->output->writeRaw("A\x1b[0J");
            }

            for (const Section& section : this->sections) {
                for (const Markup::ParsedMarkup<Markup::ElementStyle>& line : section.getVisibleLines()) {
                    if (position >= this->hiddenLines) {
                        *this->output << line << '\n';
                    }

                    ++position;
                }
            }

            if (top && rows > 0) {
                this->output->writeRaw("\x1b[");
                *this->output << rows;
                this->output->writeRaw("A");
            }
        }

        this->drawnRows = rows;
        this->output->flush();
    }

    template <std::invocable<> Change, std::invocable<> WithoutTerminal>
    void SectionStack::update(const Change& change, const WithoutTerminal& withoutTerminal)
    {
        const Stream::BufferingScope buffering{*this->output};

        if (!this->terminal) {
            withoutTerminal();

            return;
        }

        const std::size_t previousRows = this->countRows(this->getWidth());

        change();
        this->redraw(previousRows);
    }

    void SectionStack::append(Section& section, std::vector<Markup::ParsedMarkup<Markup::ElementStyle>> lines)
    {
        this->update(
            [&section, &lines] {
                section.lines.insert(section.lines.end(),
                                     std::make_move_iterator(lines.begin()),
                                     std::make_move_iterator(lines.end()));
            },
            [this, &lines] {
                this->drawLines(lines);
                this->output->flush();
            });
    }

    void SectionStack::overwrite(Section& section, std::vector<Markup::ParsedMarkup<Markup::ElementStyle>> lines)
    {
        this->update(
            [&section, &lines] {
                section.lines = std::move(lines);
            },
            [this, &lines] {
                this->drawLines(lines);
                this->output->flush();
            });
    }

    void SectionStack::clear(Section& section)
    {
        this->update(
            [&section] {
                section.lines.clear();
            },
            [] {
            });
    }

    void SectionStack::setMaxHeight(Section& section, const std::size_t maxHeight)
    {
        const auto set = [&section, maxHeight] {
            section.maxHeight = maxHeight;
        };

        this->update(set, set);
    }

    void SectionStack::setHeightLimit(std::function<std::size_t(std::size_t)> limit)
    {
        const auto set = [this, &limit] {
            this->heightLimit = std::move(limit);
        };

        this->update(set, set);
    }

    void SectionStack::setReflowWidth(std::function<std::size_t(std::size_t)> width)
    {
        const auto set = [this, &width] {
            this->reflowWidth = std::move(width);
        };

        this->update(set, set);
    }

    void SectionStack::setCursorPlacement(const CursorPlacement placement)
    {
        if (placement == this->cursorPlacement) {
            return;
        }

        if (this->terminal && this->drawnRows > 0) {
            this->output->writeRaw("\x1b[");
            *this->output << this->drawnRows;
            this->output->writeRaw(placement == CursorPlacement::Below ? "B" : "A");
            this->output->flush();
        }

        this->cursorPlacement = placement;
    }
} // namespace Consolpp::Screen
