#pragma once

#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Consolpp::Prompt
{
    class Autocompletion
    {
        private:
            std::function<void(std::string_view, std::vector<std::string>&)> find;

        public:
            explicit Autocompletion(std::function<void(std::string_view, std::vector<std::string>&)> find);

            static Autocompletion fromValues(std::span<const std::string_view> values);

            void findCompletions(std::string_view text, std::vector<std::string>& completions) const;
    };
} // namespace Consolpp::Prompt
