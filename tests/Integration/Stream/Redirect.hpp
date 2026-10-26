#pragma once

#include <cerrno>
#include <system_error>

#include <unistd.h>

namespace Consolpp::Tests::Integration::Stream
{
    class Redirect
    {
        private:
            int target;
            int saved;

        public:
            Redirect(const int target, const int replacement)
                : target{target},
                  saved{dup(target)}
            {
                if (this->saved < 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                if (dup2(replacement, target) < 0) {
                    const int error = errno;

                    close(this->saved);

                    throw std::system_error{error, std::generic_category()};
                }
            }

            Redirect(const Redirect&) = delete;
            Redirect(Redirect&&) = delete;
            Redirect& operator=(const Redirect&) = delete;
            Redirect& operator=(Redirect&&) = delete;

            ~Redirect()
            {
                dup2(this->saved, this->target);
                close(this->saved);
            }
    };
} // namespace Consolpp::Tests::Integration::Stream
