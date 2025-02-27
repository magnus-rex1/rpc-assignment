#include "socket.h"

#define SERVER_PORT IPPORT_RESERVED + getuid()

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    Server server(SERVER_PORT);
    SocketAddress* client = new SocketAddress;
    const char* mess = "test message";
    UDPMessage* callMessage = new UDPMessage(mess, sizeof(mess));

    while (1) {
        server.GetRequest(callMessage, client);
    }

    delete callMessage;
    delete client;

    return 0;
}
