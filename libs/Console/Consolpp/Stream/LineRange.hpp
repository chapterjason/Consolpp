#pragma once

#include "Consolpp/Stream/LineIterator.hpp"

#include <iterator>

namespace Consolpp::Stream
{
    class InputStream;

    class LineRange
    {
        private:
            InputStream* reader;

        public:
            explicit LineRange(InputStream& reader)
                : reader{&reader}
            {
            }

            [[nodiscard]]
            LineIterator begin() const
            {
                return LineIterator{*this->reader};
            }

            [[nodiscard]]
            static std::default_sentinel_t end()
            {
                return std::default_sentinel;
            }
    };
} // namespace Consolpp::Stream
