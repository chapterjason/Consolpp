#include "Consolpp/Formatting/CollectListEntries.hpp"

#include "Consolpp/CommandError.hpp"
#include "Consolpp/CommandErrorKind.hpp"
#include "Consolpp/DefinitionView.hpp"
#include "Consolpp/Formatting/GetNamespace.hpp"
#include "Consolpp/Formatting/ListEntry.hpp"

#include <algorithm>
#include <span>
#include <string_view>
#include <vector>

namespace Consolpp::Formatting
{
    std::vector<ListEntry> collectListEntries(const std::span<const DefinitionView> commands,
                                              const std::string_view namespaceName)
    {
        std::vector<ListEntry> entries;
        const auto add = [&entries, namespaceName](const std::string_view name, const DefinitionView& command) {
            const bool inNamespace = name.size() > namespaceName.size()
                                     && name.starts_with(namespaceName)
                                     && name.at(namespaceName.size()) == ':';

            if (namespaceName.empty() || inNamespace) {
                entries.push_back(ListEntry{.name = name, .command = &command});
            }
        };

        for (const DefinitionView& command : commands) {
            if (command.hidden) {
                continue;
            }

            add(command.name, command);

            for (const std::string_view alias : command.aliases) {
                add(alias, command);
            }
        }

        if (!namespaceName.empty() && entries.empty()) {
            throw CommandError{CommandErrorKind::UnknownNamespace, namespaceName};
        }

        std::ranges::sort(entries, [](const ListEntry& left, const ListEntry& right) {
            const std::string_view leftNamespace = Formatting::getNamespace(left.name);
            const std::string_view rightNamespace = Formatting::getNamespace(right.name);

            if (leftNamespace.empty() != rightNamespace.empty()) {
                return leftNamespace.empty();
            }

            if (leftNamespace != rightNamespace) {
                return leftNamespace < rightNamespace;
            }

            return left.name < right.name;
        });

        return entries;
    }
} // namespace Consolpp::Formatting
