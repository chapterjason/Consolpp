#include "Consolpp/BuiltIn/ManCommand.hpp"

#include "Consolpp/CommandList.hpp"
#include "Consolpp/Formatting/FormatManPage.hpp"
#include "Consolpp/Parsing/Parameters.hpp"
#include "Consolpp/Program.hpp"
#include "Consolpp/Stream/FlushMode.hpp"
#include "Consolpp/Stream/OutputStream.hpp"
#include "Consolpp/Terminal/Capabilities.hpp"

#include <cerrno>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

#include <fcntl.h>
#include <unistd.h>

namespace Consolpp::BuiltIn
{
    int ManCommand::run(const Parsing::Parameters<definition>& parameters,
                        Stream::OutputStream& output,
                        const CommandList& commands,
                        const Program& program)
    {
        constexpr int permissions = 0644;
        const std::optional<std::string_view> file = parameters.getArgument<"file">();
        std::string page;

        Formatting::formatManPage(program, commands.definitions, std::back_inserter(page));

        if (!file.has_value()) {
            output.writeRaw(page);

            return 0;
        }

        const std::string path{*file};
        const int descriptor = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, permissions);

        if (descriptor < 0) {
            throw std::system_error{errno, std::generic_category(), path};
        }

        try {
            Stream::OutputStream target{descriptor, Stream::FlushMode::Buffered, false, Terminal::Capabilities{}};

            target.writeRaw(page);
            target.flush();
        } catch (...) {
            close(descriptor);

            throw;
        }

        if (close(descriptor) != 0) {
            throw std::system_error{errno, std::generic_category(), path};
        }

        return 0;
    }
} // namespace Consolpp::BuiltIn
