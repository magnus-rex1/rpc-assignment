#include "socket.h"
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv)
{
    // Socket socket;

    if (argc <= 1) {
        std::cout << "Usage: <machine> <port>\n";
        exit(1);
    }

    int port;
    sscanf(argv[1], "%d", &port);

    SocketAddress dest;
    makeDestSA(&dest, "127.0.0.1", port);

    // socket.UDPsend(nullptr, &dest);

    Client client;
    while (1) {
        std::string input;
        std::cout << "Enter a message: ";
        std::getline(std::cin, input);
        UDPMessage* callMessage = new UDPMessage(input.c_str(), input.length());
        UDPMessage* replyMessage; // = new UDPMessage("1", 1);
        client.DoOperation(callMessage, replyMessage, &dest);

        std::cout << replyMessage << std::endl;
        delete callMessage;
        // delete replyMessage;
    }

    return 0;
}
