#pragma once

namespace Consolpp::Terminal
{
    // JetBrains terminals and Konsole up to 20.11.00 print OSC 8 links as text; an unreadable version counts as old.
    bool detectHyperlinks();
} // namespace Consolpp::Terminal
