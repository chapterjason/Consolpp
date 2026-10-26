# Text

`Consolpp::text`, header-only and `constexpr`. Text is UTF-8 in `std::string_view`.

## Display width

```cpp
#include "Consolpp/Text/MeasureDisplayWidth.hpp"

const std::size_t columns = Consolpp::Text::measureDisplayWidth("日本語"); // 6
```

The columns a text takes in a terminal: wide characters and emoji take two, combining marks none. The text must be valid UTF-8 without escape sequences.

## Wrapping

```cpp
#include "Consolpp/Text/WrapRange.hpp"
#include "Consolpp/Text/WrappedLine.hpp"

for (const Consolpp::Text::WrappedLine line : Consolpp::Text::WrapRange{text, 40}) {
    output.write("{}\n", text.substr(line.start, line.end - line.start));
}
```

Every text has at least one line. `line.width` is its display width.

## Graphemes

```cpp
#include "Consolpp/Text/Grapheme/Range.hpp"

for (const std::string_view grapheme : Consolpp::Text::Grapheme::Range{text}) {
}
```

What the user sees as one character. The text must be valid UTF-8.

## UTF-8

| Function | Does |
|---|---|
| `Utf8::isValid(text)` | Whether the text is valid UTF-8. |
| `Utf8::decode(text)` | The first code point and its length. |
| `Utf8::encode(codePoint, output)` | Writes the code point to the output iterator. |
| `replaceControlCharacters(text)` | Control characters and invalid UTF-8 become U+FFFD; for untrusted text. |

## More

| Function | Does |
|---|---|
| `Base64::encode(bytes, output)`, `Base64::decode(text, output)` | RFC 4648 Base64. |
| `compareNaturalIgnoringCase(left, right)` | Natural order, `file2` before `file10`. |
| `levenshteinDistance<MaxLength>(from, to)` | Edit distance, e.g. for "did you mean". |
| `formatDecimal(value, output)` | Writes the number to the output iterator. |

The output iterators take `char`, e.g. `std::back_inserter(string)`.
