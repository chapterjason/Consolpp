#pragma once

#include "Consolpp/Event/Handler.hpp"
#include "Consolpp/Event/SourceKind.hpp"

#include <chrono>
#include <concepts>
#include <utility>

namespace Consolpp::Event
{
    class Loop;

    class Registration
    {
        private:
            Handler handler;
            Loop* loop;
            Registration* previous{nullptr};
            Registration* next{nullptr};
            SourceKind kind;
            // The descriptor or signal number; for a timer the identifier the loop gives it.
            int target;
            std::chrono::milliseconds interval;

            void attach();

            friend class Loop;

        public:
            // A registration must end before the Loop it belongs to.
            template <typename Callable>
                requires std::constructible_from<Handler, Callable>
            Registration(Loop& loop,
                         const SourceKind kind,
                         const int target,
                         const std::chrono::milliseconds interval,
                         Callable&& callable)
                : handler{std::forward<Callable>(callable)},
                  loop{&loop},
                  kind{kind},
                  target{target},
                  interval{interval}
            {
                this->attach();
            }

            Registration(const Registration&) = delete;
            Registration(Registration&&) = delete;
            Registration& operator=(const Registration&) = delete;
            Registration& operator=(Registration&&) = delete;

            ~Registration();
    };
} // namespace Consolpp::Event
