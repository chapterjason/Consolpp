# Installation

## CPM

```cmake
CPMAddPackage(
    NAME Consolpp
    GIT_REPOSITORY git@github.com:chapterjason/Consolpp.git
    GIT_TAG <tag-or-commit>
)

target_link_libraries(app PRIVATE Consolpp::console)
```

Requires C++20, CMake 3.25+, GCC or Clang, Linux or macOS.

## Targets

| Target | Contains | Links |
|---|---|---|
| `Consolpp::console` | Commands, parsing, streams, prompts, screen, layout, terminal, events | `markup`, `text` |
| `Consolpp::markup` | Markup parsing, tags, styles, colors | `text` |
| `Consolpp::text` | UTF-8, graphemes, display width, wrapping, Base64 | |

Link only what you use; `Consolpp::console` brings the other two.

## Includes

Headers are named after the type or function they hold. The folder is the namespace.

| Target | Include | Namespace |
|---|---|---|
| `Consolpp::console` | `"Consolpp/CommandManager.hpp"`, `"Consolpp/Stream/OutputStream.hpp"` | `Consolpp`, `Consolpp::Stream` |
| `Consolpp::markup` | `"Consolpp/Markup/AddTag.hpp"` | `Consolpp::Markup` |
| `Consolpp::text` | `"Consolpp/Text/Utf8/Encode.hpp"` | `Consolpp::Text::Utf8` |
