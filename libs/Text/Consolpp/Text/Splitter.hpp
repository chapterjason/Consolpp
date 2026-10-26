#pragma once

#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace Consolpp::Text
{
    class Splitter
    {
        private:
            std::string_view remaining;
            char separator;
            bool ended{false};

        public:
            constexpr Splitter(const std::string_view text, const char separator)
                : remaining{text},
                  separator{separator}
            {
            }

            [[nodiscard]]
            constexpr bool hasNext() const
            {
                return !this->ended;
            }

            constexpr std::string_view takeNext()
            {
                if (this->ended) {
                    throw std::out_of_range{"There is no next part"};
                }

                const std::size_t position = this->remaining.find(this->separator);
                const std::string_view part = this->remaining.substr(0, position);

                if (position == std::string_view::npos) {
                    this->remaining = std::string_view{};
                    this->ended = true;
                } else {
                    this->remaining.remove_prefix(position + 1);
                }

                return part;
            }
    };
} // namespace Consolpp::Text
