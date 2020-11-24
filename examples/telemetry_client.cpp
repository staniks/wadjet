#include "telemetry_common.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <span>

using namespace telemetry_example;

class telemetry_client
{
public:
    telemetry_client(const char* server_ip, uint16_t server_port);

    void send_telemetry(std::string_view text);

private:
    wadjet::socket_api     api;
    wadjet::socket         socket;
    wadjet::socket_address server_address;
};

telemetry_client::telemetry_client(const char* server_ip, uint16_t server_port) :
    socket(wadjet::socket_protocol::ipv4, wadjet::socket_flags::dual_stack),
    server_address(socket.protocol(), server_ip, server_port)
{
}

void telemetry_client::send_telemetry(std::string_view text)
{
    telemetry_packet packet{text};

    // This isn't recommended due to potential padding and endianness issues. In real-life scenario,
    // you'll probably want a proper serialization to byte stream rather than reinterpret_cast.
    const auto packet_span = std::span{(const char*)&packet, sizeof(packet)};

    auto send_error = socket.send(server_address, packet_span);
    if(send_error != wadjet::error_code::none)
    {
        // Propagate error in form of exception.
        throw wadjet::exception{send_error};
    }
}

int main(int argc, char** argv)
{
    if(argc < 3 + 1)
    {
        std::cerr << "usage: example_client <server ip address> <server port> <telemetry string>"
                  << std::endl;
        return 1;
    }

    const char* ip_string        = argv[1];
    const char* port_string      = argv[2];
    const char* telemetry_string = argv[3];

    // Very error prone, but this is just an example.
    uint16_t port = static_cast<uint16_t>(atoi(port_string));

    try
    {
        telemetry_client client{ip_string, port};

        client.send_telemetry(telemetry_string);
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
