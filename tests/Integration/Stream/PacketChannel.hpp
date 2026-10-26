#pragma once

#include <array>
#include <cerrno>
#include <cstddef>
#include <string>
#include <system_error>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace Consolpp::Tests::Integration::Stream
{
    // Keeps every write as its own packet, so a test sees how often the stream wrote.
    class PacketChannel
    {
        private:
            static constexpr std::size_t packetCapacity = 65536;
            static constexpr int socketBufferSize = 262144;

            std::array<int, 2> descriptors{-1, -1};

            static void setBufferSize(const int descriptor, const int option)
            {
                if (setsockopt(descriptor,
                               SOL_SOCKET,
                               option,
                               &PacketChannel::socketBufferSize,
                               sizeof(PacketChannel::socketBufferSize))
                    != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }
            }

        public:
            // macOS has no SOCK_SEQPACKET for AF_UNIX, and caps datagrams at 2048 bytes unless the buffers grow.
            PacketChannel()
            {
                if (socketpair(AF_UNIX, SOCK_DGRAM, 0, this->descriptors.data()) != 0) {
                    throw std::system_error{errno, std::generic_category()};
                }

                try {
                    for (const int descriptor : this->descriptors) {
                        PacketChannel::setBufferSize(descriptor, SO_SNDBUF);
                        PacketChannel::setBufferSize(descriptor, SO_RCVBUF);
                    }
                } catch (const std::system_error&) {
                    close(std::get<0>(this->descriptors));
                    close(std::get<1>(this->descriptors));

                    throw;
                }
            }

            PacketChannel(const PacketChannel&) = delete;
            PacketChannel(PacketChannel&&) = delete;
            PacketChannel& operator=(const PacketChannel&) = delete;
            PacketChannel& operator=(PacketChannel&&) = delete;

            ~PacketChannel()
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
            std::vector<std::string> drainPackets() const
            {
                std::vector<std::string> packets;
                std::string packet(PacketChannel::packetCapacity, '\0');
                ssize_t got = 0;

                while ((got = recv(std::get<1>(this->descriptors), packet.data(), packet.size(), MSG_DONTWAIT)) > 0) {
                    packets.emplace_back(packet.data(), static_cast<std::size_t>(got));
                }

                return packets;
            }
    };
} // namespace Consolpp::Tests::Integration::Stream
