#pragma once

#include "Consolpp/Layout/Canvas.hpp"
#include "Consolpp/Layout/Element.hpp"
#include "Consolpp/Markup/ElementStyle.hpp"
#include "Consolpp/Markup/ParsedMarkup.hpp"
#include "Consolpp/Styling/ParseMarkup.hpp"

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

namespace Consolpp::Screen
{
    class SectionStack;

    class Section
    {
        private:
            // Only SectionStack can create one, so that every section belongs to the SectionStack that draws it.
            class Key
            {
                private:
                    Key() = default;

                    friend class SectionStack;
            };

            SectionStack* stack;
            std::vector<Markup::ParsedMarkup<Markup::ElementStyle>> lines;
            std::size_t maxHeight{0};

            friend class SectionStack;

        public:
            Section(SectionStack& stack, const Key /*key*/);

            Section(const Section&) = delete;
            Section(Section&&) = delete;
            Section& operator=(const Section&) = delete;
            Section& operator=(Section&&) = delete;

            ~Section() = default;

            void append(std::string_view markup);

            void append(const Layout::Canvas& canvas);

            // At its natural width, at most the available width of the output.
            void append(const Layout::Element& element);

            void overwrite(std::string_view markup);

            void overwrite(const Layout::Canvas& canvas);

            // At its natural width, at most the available width of the output.
            void overwrite(const Layout::Element& element);

            void clear();

            // Shows only the last lines; zero shows all of them.
            void setMaxHeight(std::size_t maxHeight);

            [[nodiscard]]
            std::span<const Markup::ParsedMarkup<Markup::ElementStyle>> getVisibleLines() const;
    };
} // namespace Consolpp::Screen
