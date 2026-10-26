#include "Screen/Emulator.hpp"

#include "Consolpp/Text/MeasureDisplayWidth.hpp"
#include "Screen/Cell.hpp"
#include "Screen/Row.hpp"

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <format>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace Consolpp::Tools::Screen
{
    namespace
    {
        constexpr char escape = '\x1b';
        constexpr char bell = '\x07';
        constexpr std::size_t tabStop = 8;

        // The number at the start of the parameters, or the fallback without one.
        std::size_t parseNumber(const std::string_view parameters, const std::size_t fallback)
        {
            std::size_t number = 0;
            const auto [stop, error] = std::from_chars(std::to_address(parameters.begin()),
                                                       std::to_address(parameters.end()),
                                                       number);

            return error == std::errc{} ? number : fallback;
        }

        // The number after the first semicolon, or the fallback without one.
        std::size_t parseSecondNumber(const std::string_view parameters, const std::size_t fallback)
        {
            const std::size_t separator = parameters.find(';');

            return separator == std::string_view::npos ? fallback
                                                       : parseNumber(parameters.substr(separator + 1), fallback);
        }

        // The length of the UTF-8 sequence the byte starts; a stray byte stands alone.
        std::size_t getSequenceLength(const unsigned char byte)
        {
            constexpr unsigned char fourBytes = 0xF0;
            constexpr unsigned char threeBytes = 0xE0;
            constexpr unsigned char twoBytes = 0xC0;

            if (byte >= fourBytes) {
                return 4;
            }

            if (byte >= threeBytes) {
                return 3;
            }

            if (byte >= twoBytes) {
                return 2;
            }

            return 1;
        }

        // The index of the byte ending a control sequence that starts at the index, or npos when it is not there.
        std::size_t findFinalByte(const std::string_view text, const std::size_t start)
        {
            constexpr unsigned char firstFinal = 0x40;
            constexpr unsigned char lastFinal = 0x7e;

            for (std::size_t index = start; index < text.size(); ++index) {
                const auto byte = static_cast<unsigned char>(text.at(index));

                if (byte >= firstFinal && byte <= lastFinal) {
                    return index;
                }
            }

            return std::string_view::npos;
        }

        // The length of an operating system command like a link, ended by BEL or ESC \; zero when it is cut off.
        std::size_t findCommandEnd(const std::string_view text)
        {
            for (std::size_t index = 2; index < text.size(); ++index) {
                if (text.at(index) == bell) {
                    return index + 1;
                }

                if (text.at(index) == escape) {
                    return index + 1 < text.size() ? index + 2 : 0;
                }
            }

            return 0;
        }

        // Adds the cells as rows of at most the columns, marking every row but the last as wrapped; a wide character
        // does not get split.
        void appendWrapped(std::vector<Row>& rows, const std::vector<Cell>& cells, const std::size_t columns)
        {
            rows.emplace_back();

            for (std::size_t position = 0; position < cells.size();) {
                const std::size_t width = std::max<std::size_t>(cells.at(position).width, 1);
                const std::size_t end = std::min(position + width, cells.size());

                if (!rows.back().cells.empty() && rows.back().cells.size() + width > columns) {
                    rows.back().wrapped = true;
                    rows.emplace_back();
                }

                rows.back().cells.insert(rows.back().cells.end(),
                                         std::next(cells.begin(), static_cast<std::ptrdiff_t>(position)),
                                         std::next(cells.begin(), static_cast<std::ptrdiff_t>(end)));
                position = end;
            }
        }
    } // namespace

    Emulator::Emulator(const std::size_t columns, const std::size_t rows)
        : columns{columns},
          rows{rows},
          buffer(rows, Row{})
    {
        if (columns == 0 || rows == 0) {
            throw std::invalid_argument{"A terminal needs at least one column and one row"};
        }
    }

    std::size_t Emulator::getTop() const
    {
        return this->buffer.size() - this->rows;
    }

    void Emulator::feedLine()
    {
        if (this->cursorRow + 1 == this->buffer.size()) {
            this->buffer.emplace_back();
        }

        ++this->cursorRow;
    }

    void Emulator::put(const std::string_view text, const std::size_t width)
    {
        if (width == 0) {
            Row& row = this->buffer.at(this->cursorRow);
            const std::size_t previous = this->wrapPending ? this->cursorColumn + 1 : this->cursorColumn;

            if (previous > 0 && previous <= row.cells.size()) {
                row.cells.at(previous - 1).text.append(text);
            }

            return;
        }

        if (this->wrapPending || (this->cursorColumn > 0 && this->cursorColumn + width > this->columns)) {
            this->buffer.at(this->cursorRow).wrapped = true;
            this->feedLine();
            this->cursorColumn = 0;
            this->wrapPending = false;
        }

        Row& row = this->buffer.at(this->cursorRow);

        if (row.cells.size() < this->cursorColumn + width) {
            row.cells.resize(this->cursorColumn + width, Cell{.text = " "});
        }

        row.cells.at(this->cursorColumn) = Cell{.text = std::string{text}, .width = width};

        for (std::size_t half = 1; half < width; ++half) {
            row.cells.at(this->cursorColumn + half) = Cell{.text = "", .width = 0};
        }

        this->cursorColumn += width;

        if (this->cursorColumn >= this->columns) {
            this->cursorColumn = this->columns - 1;
            this->wrapPending = true;
        }
    }

    void Emulator::handleControl(const std::string_view parameters, const char command)
    {
        constexpr std::string_view moving = "ABCDGHfJK";
        const std::size_t top = this->getTop();
        const std::size_t count = std::max<std::size_t>(parseNumber(parameters, 1), 1);
        Row& row = this->buffer.at(this->cursorRow);

        // Private sequences like hiding the cursor change nothing on the screen.
        if (!parameters.empty() && (parameters.front() < '0' || parameters.front() > ';')) {
            return;
        }

        if (moving.find(command) != std::string_view::npos) {
            this->wrapPending = false;
        }

        switch (command) {
            case 'A':
                this->cursorRow = std::max(top, this->cursorRow - std::min(count, this->cursorRow));
                break;
            case 'B':
                this->cursorRow = std::min(this->buffer.size() - 1, this->cursorRow + count);
                break;
            case 'C':
                this->cursorColumn = std::min(this->columns - 1, this->cursorColumn + count);
                break;
            case 'D':
                this->cursorColumn -= std::min(count, this->cursorColumn);
                break;
            case 'G':
                this->cursorColumn = std::min(this->columns, count) - 1;
                break;
            case 'H':
            case 'f':
                this->cursorRow = top + std::min(this->rows, count) - 1;
                this->cursorColumn = std::min(this->columns, std::max<std::size_t>(parseSecondNumber(parameters, 1), 1))
                                     - 1;
                break;
            case 'J':
                if (parseNumber(parameters, 0) == 0) {
                    row.cells.resize(std::min(row.cells.size(), this->cursorColumn));
                    row.wrapped = false;

                    for (std::size_t index = this->cursorRow + 1; index < this->buffer.size(); ++index) {
                        this->buffer.at(index) = Row{};
                    }
                } else {
                    for (std::size_t index = top; index < this->buffer.size(); ++index) {
                        this->buffer.at(index) = Row{};
                    }
                }

                break;
            case 'K':
                row.cells.resize(std::min(row.cells.size(), this->cursorColumn));
                row.wrapped = false;
                break;
            default:
                break;
        }
    }

    std::size_t Emulator::handle(const std::string_view text)
    {
        const char character = text.front();
        const auto byte = static_cast<unsigned char>(character);
        constexpr unsigned char firstPrintable = 0x20;
        constexpr unsigned char deleteByte = 0x7f;

        if (character == escape) {
            if (text.size() < 2) {
                return 0;
            }

            if (text.at(1) == '[') {
                const std::size_t final = findFinalByte(text, 2);

                if (final == std::string_view::npos) {
                    return 0;
                }

                this->handleControl(text.substr(2, final - 2), text.at(final));

                return final + 1;
            }

            return text.at(1) == ']' ? findCommandEnd(text) : 2;
        }

        if (character == '\r') {
            this->cursorColumn = 0;
            this->wrapPending = false;

            return 1;
        }

        if (character == '\n') {
            this->feedLine();
            this->wrapPending = false;

            return 1;
        }

        if (character == '\t') {
            this->cursorColumn = std::min(((this->cursorColumn / tabStop) + 1) * tabStop, this->columns - 1);

            return 1;
        }

        if (byte < firstPrintable || byte == deleteByte) {
            return 1;
        }

        const std::size_t length = getSequenceLength(byte);

        if (text.size() < length) {
            return 0;
        }

        const std::string_view sequence = text.substr(0, length);

        this->put(sequence, Text::measureDisplayWidth(sequence));

        return length;
    }

    void Emulator::write(const std::string_view bytes)
    {
        std::size_t used = 0;

        this->incomplete.append(bytes);

        while (used < this->incomplete.size()) {
            const std::size_t length = this->handle(std::string_view{this->incomplete}.substr(used));

            if (length == 0) {
                break;
            }

            used += length;
        }

        this->incomplete.erase(0, used);
    }

    void Emulator::resize(const std::size_t columns, const std::size_t rows)
    {
        if (columns == 0 || rows == 0) {
            throw std::invalid_argument{"A terminal needs at least one column and one row"};
        }

        // The rows below the cursor that hold nothing go, so the screen ends at the cursor as in a terminal.
        while (this->buffer.size() > this->cursorRow + 1 && this->buffer.back() == Row{}) {
            this->buffer.pop_back();
        }

        std::vector<std::vector<Cell>> lines;
        std::vector<Cell> line;
        std::size_t cursorLine = 0;
        std::size_t cursorOffset = 0;

        for (std::size_t index = 0; index < this->buffer.size(); ++index) {
            const Row& row = this->buffer.at(index);

            if (index == this->cursorRow) {
                cursorLine = lines.size();
                cursorOffset = line.size() + this->cursorColumn + (this->wrapPending ? 1 : 0);
            }

            line.insert(line.end(), row.cells.begin(), row.cells.end());

            if (!row.wrapped) {
                lines.push_back(std::move(line));
                line.clear();
            }
        }

        if (!line.empty()) {
            lines.push_back(std::move(line));
        }

        std::vector<Row> reflowed;
        std::size_t newCursorRow = 0;
        std::size_t newCursorColumn = cursorOffset;

        for (std::size_t index = 0; index < lines.size(); ++index) {
            if (index == cursorLine) {
                newCursorRow = reflowed.size();
            }

            appendWrapped(reflowed, lines.at(index), columns);
        }

        while (reflowed.at(newCursorRow).wrapped && newCursorColumn >= reflowed.at(newCursorRow).cells.size()) {
            newCursorColumn -= reflowed.at(newCursorRow).cells.size();
            ++newCursorRow;
        }

        while (reflowed.size() < rows) {
            reflowed.emplace_back();
        }

        this->buffer = std::move(reflowed);
        this->columns = columns;
        this->rows = rows;
        this->cursorRow = newCursorRow;
        this->wrapPending = newCursorColumn >= columns;
        this->cursorColumn = std::min(newCursorColumn, columns - 1);
    }

    std::string Emulator::render(const bool scrollback) const
    {
        const std::size_t top = this->getTop();
        std::string text = std::format("{}x{}, cursor in row {} column {}{}\n",
                                       this->columns,
                                       this->rows,
                                       static_cast<long long>(this->cursorRow) - static_cast<long long>(top) + 1,
                                       this->cursorColumn + 1,
                                       this->wrapPending ? ", wrap pending" : "");
        const auto appendRow = [this, &text](const Row& row, const char marker) {
            std::size_t width = 0;

            text.push_back(marker);
            text.push_back('|');

            for (const Cell& cell : row.cells) {
                text.append(cell.text);
                width += cell.width;
            }

            text.append(this->columns > width ? this->columns - width : 0, ' ');
            text.push_back(row.wrapped ? '\\' : '|');
            text.push_back('\n');
        };

        if (scrollback) {
            for (std::size_t index = 0; index < top; ++index) {
                appendRow(this->buffer.at(index), ' ');
            }

            text.append(" ");
            text.append(this->columns + 2, '=');
            text.push_back('\n');
        }

        for (std::size_t index = top; index < this->buffer.size(); ++index) {
            appendRow(this->buffer.at(index), index == this->cursorRow ? '>' : ' ');
        }

        return text;
    }
} // namespace Consolpp::Tools::Screen
