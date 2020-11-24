#include "telemetry_common.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <span>

using namespace telemetry_example;

class telemetry_server
{
public:
    telemetry_server(uint16_t server_port);

    void run();

private:
    wadjet::socket_api api;
    wadjet::socket     socket;
};

telemetry_server::telemetry_server(uint16_t port) :
    socket(wadjet::socket_protocol::ipv4, wadjet::socket_flags::dual_stack)
{
    auto bind_error = socket.bind(wadjet::socket_address::any(socket.protocol(), port));
    if(bind_error != wadjet::error_code::none)
    {
        // Propagate error in form of exception.
        throw wadjet::exception{bind_error};
    }
}

void telemetry_server::run()
{
    for(;;)
    {
        telemetry_packet packet;

        // This isn't recommended due to potential padding and endianness issues. In real-life
        // scenario, you'll probably want a proper serialization to byte stream rather than
        // reinterpret_cast.
        auto result = socket.recv(std::span<char>{(char*)&packet, sizeof(packet)});
        if(!result && result.error() != wadjet::error_code::socket_would_block)
        {
            // Propagate error in form of exception.
            throw wadjet::exception{result.error()};
        }
        else if(result)
        {
            if(result->payload.size() != sizeof(packet))
                throw std::runtime_error{"received malformed or incomplete unix time"};
            else
            {
                std::cout << "received telemetry packet: " << packet.to_string() << std::endl;
            }
        }
    }
}

int main(int argc, char** argv)
{
    if(argc < 1 + 1)
    {
        std::cerr << "usage: example_server <port>" << std::endl;
        return 1;
    }

    const char* port_string = argv[1];

    // Very error prone, but this is just an example.
    uint16_t port = static_cast<uint16_t>(atoi(port_string));

    try
    {
        telemetry_server server{port};

        server.run();
    }
    catch(const wadjet::exception& e)
    {
        std::cerr << e.what() << ", underlying error: " << e.error().underlying_code << std::endl;
        return 1;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
