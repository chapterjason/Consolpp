# Screen

## Sections

Parts of the output that are rewritten in place, e.g. a progress line below a log.

```cpp
#include "Consolpp/Screen/Section.hpp"
#include "Consolpp/Screen/SectionStack.hpp"

Consolpp::Screen::SectionStack sections{output};
Consolpp::Screen::Section& log = sections.createSection();
Consolpp::Screen::Section& progress = sections.createSection();

log.setMaxHeight(4);

for (const File& file : files) {
    upload(file);
    log.append(std::format("<info>uploaded</info> {}", file.path));
    progress.overwrite(std::format("{}/{} files", ++done, files.size()));
}
```

- Sections are stacked in the order they are created.
- `append(markup)` adds a line, `overwrite(markup)` replaces all lines, `clear()` removes them.
- `setMaxHeight(n)` shows only the last `n` lines; `0` shows all.
- The lines are markup. Text from outside must be escaped, see [Markup](MARKUP.md).
- When the stack ends, the cursor moves below it, so following output does not overwrite it. Write nothing else to the output while the stack exists.
- `SectionStack::setHeightLimit(...)`: the rows all sections may take together. By default the terminal height minus one.

## Layout

Elements measure themselves and render at a width: on their own or inside a section.

```cpp
#include "Consolpp/Layout/Columns.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Layout/Paragraph.hpp"
#include "Consolpp/Layout/Stack.hpp"
#include "Consolpp/Text/Alignment.hpp"

const Consolpp::Layout::Paragraph name{"<info>Name</info>"};
const Consolpp::Layout::Paragraph description{"A long description that wraps", Consolpp::Text::Alignment::Left};
const std::array<const Consolpp::Layout::Element*, 2> cells{&name, &description};
const Consolpp::Layout::Columns row{cells, 2};

output << row;
```

- `Paragraph{markup, alignment}`: wrapped text, `Alignment::Left`, `Center` or `Right`.
- `Columns{children, gap}`: side by side, `gap` spaces apart.
- `Stack{children, gap}`: one below the other, `gap` blank lines apart.
- Children and markup must outlive the element.
- `output << element` and `section.append(element)` render at the natural width, at most the available width.
- Own elements derive from `Layout::Element` and implement `measure()` and `render(width, canvas)`.

## Terminal

| Type | Does while it exists |
|---|---|
| `Terminal::HiddenCursor{output}` | Hides the cursor. |
| `Terminal::RawMode{descriptor}` | Puts the terminal into raw mode. Throws `std::system_error` when it cannot. |

Both restore the terminal when they end, also when an exception passes.
