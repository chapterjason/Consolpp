#pragma once

#include <cstddef>

namespace Consolpp::Stream
{
    class OutputStream;

    class OutputStreamIterator
    {
        private:
            OutputStream* writer;
            bool flushing;

        public:
            // Named as std::iterator_traits requires.
            using difference_type = std::ptrdiff_t; // NOLINT(readability-identifier-naming)

            // A flushing iterator writes like put(); otherwise the stream's operation flushes once at its end.
            OutputStreamIterator(OutputStream& writer, const bool flushing)
                : writer{&writer},
                  flushing{flushing}
            {
            }

            OutputStreamIterator& operator*()
            {
                return *this;
            }

            OutputStreamIterator& operator=(const char character);

            OutputStreamIterator& operator++()
            {
                return *this;
            }

            OutputStreamIterator operator++(int)
            {
                return *this;
            }
    };
} // namespace Consolpp::Stream
