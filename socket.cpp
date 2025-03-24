#include "socket.h"

/*
 * Socket implementation
 */

void Socket::init(int port)
{
    /*
     * create a socket
     * AF_INET - IPv4 Internet protocol
     * SOCK_STREAM - two way communication
     */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Problem creating socket\n";
        throw std::runtime_error(std::strerror(errno));
    }

    socketAddress = new SocketAddress;

    socketAddress->sin_family = AF_INET;
    socketAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddress->sin_port = htons(port);

    if (bind(s, (struct sockaddr*)socketAddress, sizeof(struct sockaddr_in)) < 0) {
        std::cerr << "Problem binding\n";
        throw std::runtime_error(std::strerror(errno));
    }

    // Print port we a assigned to
    socklen_t length = sizeof(socketAddress);
    if (getsockname(s, (struct sockaddr*)socketAddress, &length) < 0) {
        std::cerr << "Error getsockname\n";
        throw std::runtime_error(std::strerror(errno));
    }
    std::cout << "The assigned port number is " << ntohs(socketAddress->sin_port) << std::endl;
}

// create the socket and bind it to any local port
Socket::Socket()
{
    init();
}

// opens a socket and binds it to the given port
Socket::Socket(int port)
{
    init(port);
}

Socket::~Socket()
{
    close(s);
    delete socketAddress;
}

Status Socket::UDPsend(UDPMessage* m, SocketAddress* destination)
{
    Status status;
    status = Status::Ok;

    unsigned char* mess = m->GetMessage();

    sendto(s, mess, strlen((const char*)mess), 0, (struct sockaddr*)destination, sizeof(struct sockaddr));

    return status;
}

Status Socket::UDPreceive(UDPMessage** m, SocketAddress* origin)
{
    Status status;
    status = Status::Ok;

    char buffer[SIZE];
    memset(buffer, 0, sizeof buffer);

    struct sockaddr_storage clientAddr;
    socklen_t clientLen = sizeof clientAddr;
    bool failed = false;

    // timeout after five tries
    for (int i = 0; i < 5; i++) {
        int n = server_timeout(s, 5);
        if (n == -2) {
            std::cout << "Failed to connect to server...\n";
            failed = true;
        }
    }
    if (failed == true) {
        std::cout << "Tried 5 times to connect to server, closing client now.\n";
        throw std::runtime_error("Failed to find open server");
    }

    int len = recvfrom(s, buffer, sizeof buffer, 0, (struct sockaddr*)&clientAddr, &clientLen);

    if (len == 0) {
        std::cout << "Server closed connection.\n";
    }

    // Get the message origin
    *origin = *(SocketAddress*)&clientAddr;

    UDPMessage mess((unsigned char*)buffer, len); // create a new UDPMessage
    **m = mess; // transfer ownership of mess (deep copy)

    return status;
}

int Socket::anything()
{
    int n = recvfromtimeout(s, nullptr, 0, nullptr, 0, 2);
    if (n == -2) {
        std::cout << "took too long to respond\n";
    }

    return 0; // anyThingThere(this->s);
}
