#include "socket.h"
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    if (argc <= 1) {
        std::cout << "Usage: <machine> <port>\n";
        exit(1);
    }

    int port;
    sscanf(argv[1], "%d", &port);

    SocketAddress dest;
    const char* localhost = "127.0.0.1";
    makeDestSA(&dest, localhost, port);

    Client client;
    while (1) {
        std::string input;
        std::cout << "Enter a message: ";
        std::getline(std::cin, input);
        UDPMessage* callMessage = new UDPMessage(input.c_str(), input.length());
        UDPMessage* replyMessage = new UDPMessage("", 0);
        client.DoOperation(callMessage, replyMessage, &dest);

        delete callMessage;
        delete replyMessage;
    }

    return 0;
}
