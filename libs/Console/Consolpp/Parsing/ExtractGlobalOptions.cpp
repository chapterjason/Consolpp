#include "Consolpp/Parsing/ExtractGlobalOptions.hpp"

#include "Consolpp/Option.hpp"
#include "Consolpp/Parsing/GlobalOptionValues.hpp"
#include "Consolpp/Parsing/HasValueInNextArgument.hpp"
#include "Consolpp/Parsing/Token.hpp"
#include "Consolpp/Parsing/TokenIterator.hpp"
#include "Consolpp/Parsing/TokenKind.hpp"
#include "Consolpp/Stream/Verbosity.hpp"

#include <span>
#include <string_view>
#include <vector>

namespace Consolpp::Parsing
{
    namespace
    {
        // Whether the argument is one of the global options; it changes the values when it is.
        bool applyGlobalOption(const std::string_view argument, GlobalOptionValues& values)
        {
            if (argument == "-q" || argument == "--quiet") {
                values.verbosity = Stream::Verbosity::Quiet;
                values.interactionDisabled = true;
            } else if (argument == "--silent") {
                values.verbosity = Stream::Verbosity::Silent;
                values.interactionDisabled = true;
            } else if (argument == "-v" || argument == "--verbose") {
                values.verbosity = Stream::Verbosity::Verbose;
            } else if (argument == "-vv") {
                values.verbosity = Stream::Verbosity::VeryVerbose;
            } else if (argument == "-vvv") {
                values.verbosity = Stream::Verbosity::Debug;
            } else if (argument == "-n" || argument == "--no-interaction") {
                values.interactionDisabled = true;
            } else {
                return false;
            }

            return true;
        }
    } // namespace

    GlobalOptionValues extractGlobalOptions(const std::span<const Option> options,
                                            const std::span<const char* const> arguments,
                                            std::vector<const char*>& remaining)
    {
        GlobalOptionValues values;
        bool valueNext = false;
        bool afterSeparator = false;

        for (auto argument = arguments.begin(); argument != arguments.end(); ++argument) {
            const Token token = *TokenIterator{argument};

            if (afterSeparator || valueNext) {
                valueNext = false;
                remaining.push_back(*argument);

                continue;
            }

            if (token.getKind() == TokenKind::Separator) {
                afterSeparator = true;
                remaining.push_back(*argument);

                continue;
            }

            if (applyGlobalOption(*argument, values)) {
                continue;
            }

            valueNext = Parsing::hasValueInNextArgument(options, token);
            remaining.push_back(*argument);
        }

        return values;
    }
} // namespace Consolpp::Parsing
