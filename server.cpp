#include "socket.h"
#include <iostream>

#define SERVER_PORT IPPORT_RESERVED + getuid()

int main(int argc, char** argv)
{
    // Socket socket;

    // if (argc <= 1) {
    //     std::cout << "Usage: port\n";
    //     exit(1);
    // }

    // SocketAddress* origin;
    // makeReceiverSA(origin, 8000);

    // socket.UDPreceive(nullptr, nullptr);
    Server server(SERVER_PORT);
    SocketAddress* client = new SocketAddress;
    const char* mess = "test message";
    UDPMessage* callMessage = new UDPMessage(mess, sizeof(mess));

    while (1) {
        server.GetRequest(callMessage, client);
        // server.UDPreceive(&callMessage, origin);
    }

    delete callMessage;
    delete client;

    return 0;
}
