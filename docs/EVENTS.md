# Events

`Consolpp::Event::Loop`, taken by a command as `run` parameter. Created in `main`.

```cpp
#include "Consolpp/Event/Loop.hpp"
#include "Consolpp/Event/Registration.hpp"

#include <csignal>

static int run(Consolpp::Stream::OutputStream& output, Consolpp::Event::Loop& loop)
{
    const Consolpp::Event::Registration interrupt = loop.registerSignal(SIGINT, [&output, &loop] {
        output.write("\n<comment>Stopping.</comment>\n");
        loop.stop();
    });
    const Consolpp::Event::Registration tick = loop.registerTimer(std::chrono::seconds{1}, [&output] {
        output.write(".");
        output.flush();
    });

    loop.run();

    return 0;
}
```

## Sources

| Method | Calls the handler |
|---|---|
| `registerSignal(signal, handler)` | When the signal arrives, e.g. `SIGINT`, `SIGWINCH`. |
| `registerTimer(interval, handler)` | Every interval. |
| `registerTimeout(delay, handler)` | Once, after the delay. |
| `registerImmediate(handler)` | Once, in the next round of the loop, without waiting for another source. |
| `registerReadable(descriptor, handler)` | When the descriptor can be read. |
| `registerWritable(descriptor, handler)` | When the descriptor can be written. |

Handlers take no arguments and return nothing.

## Running

- `run()` runs until `stop()`.
- `runOnce(timeout)` waits at most the timeout and returns whether a source was ready.

## Rules

- A source stays registered while its `Registration` exists. Keep it in a variable; a discarded one ends at once.
- A registration must end before its loop.
- A handler may end its own registration only as its last action.
- Not thread-safe: register, end registrations and run on one thread.
- On Linux a registered signal is blocked in the loop's thread; other threads must block it too, or they still receive it.
- Flush the output before `run()`, so the user sees it while the loop waits.
