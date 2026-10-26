#include "Consolpp/Stream/LineIterator.hpp"

#include "Consolpp/Stream/InputStream.hpp"

#include <optional>
#include <string_view>

namespace Consolpp::Stream
{
    LineIterator::LineIterator(InputStream& reader)
        : reader{&reader}
    {
        this->advance();
    }

    void LineIterator::advance()
    {
        const std::optional<std::string_view> next = this->reader->readLine();

        this->exhausted = !next.has_value();
        this->line = next.value_or(std::string_view{});
    }
} // namespace Consolpp::Stream
