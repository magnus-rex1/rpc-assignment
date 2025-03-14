#include "socket.h"

#define SERVER_PORT IPPORT_RESERVED + getuid()

int main(void)
{
    Server server(SERVER_PORT);

    SocketAddress* client = new SocketAddress;
    UDPMessage callMessage(1024);

    while (1) {
        std::cout << "Waiting for client message:\n";
        server.GetRequest(&callMessage, client);

        if (strcmp((const char*)callMessage.GetMessage(), "q") == 0) {
            std::cout << "Closing server, received a \'q\' for quit.\n";
            break;
        }

        std::string input;
        std::cout << "Enter reply message: ";
        std::getline(std::cin, input);

        UDPMessage reply((unsigned char*)input.c_str(), input.length());
        server.SendReply(&reply, client);
    }

    delete client;
    // server is destroyed by the OS when main returns

    return 0;
}
