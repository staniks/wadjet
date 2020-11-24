#pragma once

#include <wadjet/socket.hpp>

#include <cstdint>
#include <cstddef>
#include <string_view>
#include <stdexcept>
#include <cstring>

namespace telemetry_example {

struct telemetry_packet
{
    inline telemetry_packet() : telemetry_size(0)
    {
        std::memset(telemetry, 0, telemetry_max_length);
    }

    inline telemetry_packet(std::string_view str) : telemetry_size(str.size())
    {
        if(telemetry_size > telemetry_packet::telemetry_max_length)
            throw std::runtime_error{"telemetry string too large"};

        std::memset(telemetry, 0, telemetry_max_length);
        std::memcpy(telemetry, str.data(), str.size());
    }

    inline std::string_view to_string()
    {
        return std::string_view{telemetry, telemetry_size};
    }

    inline static constexpr size_t telemetry_max_length = std::numeric_limits<uint8_t>::max();

    uint8_t telemetry_size;
    char    telemetry[telemetry_max_length];
};

} // namespace telemetry_example