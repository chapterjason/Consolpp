# Markup

Output, prompt labels and section lines are markup: text with XML-like tags.

```cpp
output.write("<info>Done</info> in <comment>{}</comment>\n", duration);
```

## Ready-made tags

| Tag | Style |
|---|---|
| `<info>` | Green |
| `<comment>` | Yellow |
| `<question>` | Black on cyan |
| `<error>` | White on red |
| `<warning>` | Black on yellow |

The prompt fields use `<prompt-label>`, `<prompt-default>`, `<prompt-suggestion>` and `<prompt-highlight>`. Replace them to restyle the prompts.

## style

```xml
<style foreground="red" background="#08f" bold="true">text</style>
```

- `foreground`, `background`: `default`, `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white`, `gray`, `bright-red`, `bright-green`, `bright-yellow`, `bright-blue`, `bright-magenta`, `bright-cyan`, `bright-white`, or hex `#rgb` / `#rrggbb`.
- `bold`, `underline`, `blink`, `reverse`, `conceal`: `"true"` or `"false"`.

## link

```xml
<link href="https://example.com">text</link>
```

A hyperlink where the terminal supports it, otherwise just the text. Takes the `style` attributes too.

## Escaping

- `&lt;`, `&gt;`, `&amp;`, `&quot;`, `&apos;`, and character references such as `&#x41;`.
- Arguments of `write` are escaped already.
- To insert text into markup yourself:

```cpp
#include "Consolpp/Markup/Encode.hpp"

std::string markup;

Consolpp::Markup::encode(userText, std::back_inserter(markup));
```

## Errors

Unknown tags, unknown attributes, unclosed or mismatched tags and unknown entities throw. Every string written must be complete markup on its own.

## Custom tags

```cpp
#include "Consolpp/Markup/AddTag.hpp"
#include "Consolpp/Markup/Color.hpp"
#include "Consolpp/Markup/Style.hpp"
#include "Consolpp/Markup/TagDefinition.hpp"

Consolpp::Markup::addTag(Consolpp::Markup::TagDefinition{
    .name = "highlight",
    .style =
        Consolpp::Markup::Style{
            .foreground = Consolpp::Markup::Color::black,
            .background = Consolpp::Markup::Color::brightCyan,
            .bold = true,
        },
});
```

- `addTag`, `replaceTag`, `removeTag`, `setTags` change the tags. They throw `std::invalid_argument` for a taken or unknown name; `style` and `link` cannot be redefined.
- Not thread-safe: set tags up in `main`, before any output.
- Tag names must outlive their use.
- Colors: `Color::red`, `Color::fromHex("#ff8800")`, `Color::fromRgb(255, 136, 0)`.
