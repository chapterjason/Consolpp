#pragma once

#include <array>
#include <cerrno>
#include <cstddef>
#include <string>
#include <string_view>
#include <system_error>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace Consolpp::Tests::Integration::Stream
{
    class Channel
    {
        private:
            static constexpr int socketBufferSize = 262144;
            static constexpr std::size_t chunkSize = 4096;

            std::array<int, 2> descriptors{-1, -1};

            static void setBufferSize(const int descriptor, const int option)
            {
                if (setsockopt(descriptor,
                               SOL_SOCKET,
                               option,
                               &Channel::socketBufferSize,
                               sizeof(Channel::socketBufferSize))
                    != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }
            }

        public:
            Channel()
            {
                if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->descriptors.data()) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                try {
                    for (const int descriptor : this->descriptors) {
                        Channel::setBufferSize(descriptor, SO_SNDBUF);
                        Channel::setBufferSize(descriptor, SO_RCVBUF);
                    }
                } catch (const std::system_error&) {
                    close(std::get<0>(this->descriptors));
                    close(std::get<1>(this->descriptors));

                    throw;
                }
            }

            Channel(const Channel&) = delete;
            Channel(Channel&&) = delete;
            Channel& operator=(const Channel&) = delete;
            Channel& operator=(Channel&&) = delete;

            ~Channel()
            {
                for (const int descriptor : this->descriptors) {
                    close(descriptor);
                }
            }

            [[nodiscard]]
            int getWriteEnd() const
            {
                return std::get<0>(this->descriptors);
            }

            [[nodiscard]]
            int getReadEnd() const
            {
                return std::get<1>(this->descriptors);
            }

            void send(std::string_view text) const
            {
                while (!text.empty()) {
                    const ssize_t sent = ::send(std::get<0>(this->descriptors), text.data(), text.size(), 0);

                    if (sent < 0) {
                        throw std::system_error{errno, std::generic_category()};
                    }

                    text.remove_prefix(static_cast<std::size_t>(sent));
                }
            }

            void endWriting() const
            {
                shutdown(std::get<0>(this->descriptors), SHUT_WR);
            }

            [[nodiscard]]
            std::string drain() const
            {
                std::string received;
                std::array<char, Channel::chunkSize> chunk{};
                ssize_t got = 0;

                while ((got = recv(std::get<1>(this->descriptors), chunk.data(), chunk.size(), MSG_DONTWAIT)) > 0) {
                    received.append(chunk.data(), static_cast<std::size_t>(got));
                }

                return received;
            }
    };
} // namespace Consolpp::Tests::Integration::Stream
