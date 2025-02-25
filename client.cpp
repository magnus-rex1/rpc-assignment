#include "socket.h"
#include <iostream>
#include <memory>

int main(int argc, char** argv)
{
    Socket socket;

    if (argc <= 1) {
        std::cout << "Usage: <port>\n";
        exit(1);
    }

    int port;
    sscanf(argv[1], "%d", &port);

    SocketAddress dest;
    makeDestSA(&dest, "127.0.0.1", port);

    // SocketAddress* dest = new SocketAddress();
    // makeDestSA(dest, "127.0.0.1", port);

    socket.UDPsend(nullptr, &dest);

    return 0;
}
