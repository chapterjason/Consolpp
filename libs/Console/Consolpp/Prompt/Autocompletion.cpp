#include "Consolpp/Prompt/Autocompletion.hpp"

#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Consolpp::Prompt
{
    Autocompletion::Autocompletion(std::function<void(std::string_view, std::vector<std::string>&)> find)
        : find{std::move(find)}
    {
    }

    Autocompletion Autocompletion::fromValues(const std::span<const std::string_view> values)
    {
        return Autocompletion{[values](const std::string_view text, std::vector<std::string>& completions) {
            completions.clear();

            for (const std::string_view value : values) {
                if (value.starts_with(text)) {
                    completions.emplace_back(value);
                }
            }
        }};
    }

    void Autocompletion::findCompletions(const std::string_view text, std::vector<std::string>& completions) const
    {
        this->find(text, completions);
    }
} // namespace Consolpp::Prompt
