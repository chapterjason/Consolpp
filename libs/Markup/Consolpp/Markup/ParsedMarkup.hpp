#pragma once

#include "Consolpp/Markup/Decode.hpp"
#include "Consolpp/Markup/Element.hpp"
#include "Consolpp/Markup/ParsedMarkupLineRange.hpp"
#include "Consolpp/Markup/ParsedMarkupSlice.hpp"
#include "Consolpp/Markup/Parser.hpp"
#include "Consolpp/Markup/Run.hpp"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace Consolpp::Markup
{
    // The visible text with decoded entities and a run with the context of every part of it.
    template <typename Context>
    class ParsedMarkup
    {
        private:
            // Contexts may point into the markup, so it is kept, shared with the lines and stable when moved.
            std::shared_ptr<const std::string> source;
            std::string text;
            std::vector<Run<Context>> runs;

        public:
            // Throws ParseError or DecodeError for invalid markup and whatever the resolver throws.
            template <typename Resolve>
                requires std::convertible_to<std::invoke_result_t<Resolve&, const Element&>, Context>
            static ParsedMarkup fromMarkup(const std::string_view markup, const Context& root, Resolve resolve)
            {
                ParsedMarkup parsed;

                parsed.source = std::make_shared<const std::string>(markup);
                Parser{*parsed.source}.parse(root,
                                             resolve,
                                             [&parsed](const std::string_view escapedText, const Context& context) {
                                                 const std::size_t start = parsed.text.size();

                                                 Markup::decode(escapedText, std::back_inserter(parsed.text));
                                                 parsed.runs.push_back(Run<Context>{
                                                     .start = start,
                                                     .length = parsed.text.size() - start,
                                                     .context = context,
                                                 });
                                             });

                return parsed;
            }

            [[nodiscard]]
            std::vector<ParsedMarkup> splitLines() const
            {
                std::vector<ParsedMarkup> lines;
                std::size_t start = 0;

                while (true) {
                    const std::size_t end = std::min(this->text.find('\n', start), this->text.size());
                    ParsedMarkup line;

                    line.source = this->source;
                    line.text = this->text.substr(start, end - start);

                    for (const Run<Context>& run : this->runs) {
                        const std::size_t runStart = std::max(run.start, start);
                        const std::size_t runEnd = std::min(run.start + run.length, end);

                        if (runStart < runEnd) {
                            line.runs.push_back(Run<Context>{
                                .start = runStart - start,
                                .length = runEnd - runStart,
                                .context = run.context,
                            });
                        }
                    }

                    lines.push_back(std::move(line));

                    if (end == this->text.size()) {
                        break;
                    }

                    start = end + 1;
                }

                return lines;
            }

            // Throws std::out_of_range for a range outside the text.
            [[nodiscard]]
            ParsedMarkupSlice<Context> getSlice(const std::size_t start, const std::size_t end) const
            {
                if (start > end || end > this->text.size()) {
                    throw std::out_of_range{"The slice lies outside the text"};
                }

                return ParsedMarkupSlice<Context>{.markup = this, .start = start, .end = end};
            }

            // The lines wrapped at the display width; they must not outlive the markup.
            [[nodiscard]]
            ParsedMarkupLineRange<Context> getLines(const std::size_t width) const
            {
                return ParsedMarkupLineRange<Context>{this, this->text, width};
            }

            [[nodiscard]]
            std::string_view getText() const
            {
                return this->text;
            }

            [[nodiscard]]
            std::span<const Run<Context>> getRuns() const
            {
                return this->runs;
            }
    };
} // namespace Consolpp::Markup
