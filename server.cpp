#include "socket.h"
#include <iostream>

int main(int argc, char** argv)
{
    Socket socket;

    // if (argc <= 1) {
    //     std::cout << "Usage: port\n";
    //     exit(1);
    // }

    // SocketAddress* origin;
    // makeReceiverSA(origin, 8000);

    socket.UDPreceive(nullptr, nullptr);
    return 0;
}
