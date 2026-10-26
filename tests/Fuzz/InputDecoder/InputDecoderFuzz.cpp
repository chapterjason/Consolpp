#include "Consolpp/Keyboard/Key.hpp"
#include "Consolpp/Terminal/InputDecoder.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

// libFuzzer calls this with arbitrary bytes; the decoder must take any of them without failing.
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* const data, const std::size_t size)
{
    Consolpp::Terminal::InputDecoder decoder;
    const auto ignore = [](const Consolpp::Keyboard::Key& /*key*/) {
    };
    const auto ignoreText = [](const std::string_view /*text*/) {
    };
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::span<const char> bytes{reinterpret_cast<const char*>(data), size};

    decoder.decode(bytes, ignore, ignoreText);
    decoder.flush(ignore, ignoreText);

    return 0;
}
