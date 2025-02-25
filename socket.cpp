#include "socket.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define SIZE 1000

void printSA(struct sockaddr_in sa)
{
    printf("sa = %d, %s, %d\n",
        sa.sin_family, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
}

void makeDestSA(struct sockaddr_in* sa, char* hostname, int port)
{
    struct hostent* host;

    sa->sin_family = AF_INET;
    if ((host = gethostbyname(hostname)) == NULL) {
        printf("Unknown host name\n");
        exit(-1);
    }
    sa->sin_addr = *(struct in_addr*)(host->h_addr);
    sa->sin_port = htons(port);
}

void makeReceiverSA(struct sockaddr_in* sa, int port)
{
    sa->sin_family = AF_INET;
    sa->sin_port = htons(port);
    sa->sin_addr.s_addr = htonl(INADDR_ANY);
}

Message::Message(unsigned char* message, unsigned int len)
{
    data = message;
    length = len;
}

Message::Message(unsigned int len)
{
    data = nullptr;
    length = len;
}

// create the socket and bind it to any local port
Socket::Socket()
{
    /*
     * create a socket
     * AF_INET - IPv4 Internet protocol
     * SOCK_STREAM - two way communication
     */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Problem creating socket\n";
        exit(1);
    }

    socketAddress->sin_family = AF_INET;
    socketAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddress->sin_port = htons(0);

    if (bind(s, (struct sockaddr*)socketAddress, sizeof(struct sockaddr_in)) < 0) {
        std::cerr << "Problem binding\n";
        std::cerr << std::strerror(errno) << '\n';
        exit(0);
    }

    // Print port we a assigned to
    socklen_t length = sizeof(socketAddress);
    if (getsockname(s, (struct sockaddr*)socketAddress, &length) < 0) {
        std::cerr << "Error getsockname\n";
        exit(1);
    }
    std::cout << "The server passive port number is " << ntohs(socketAddress->sin_port) << std::endl;
    printSA(*socketAddress);
}

// opens a socket and binds it to the given port
Socket::Socket(int port)
{
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Problem creating socket\n";
        exit(1);
    }

    socketAddress->sin_family = AF_INET;
    socketAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddress->sin_port = htons(port);

    if (bind(s, (struct sockaddr*)socketAddress, sizeof(struct sockaddr_in)) < 0) {
        std::cerr << "Problem binding\n";
        std::cerr << std::strerror(errno) << '\n';
        exit(0);
    }

    // Print port we a assigned to
    socklen_t length = sizeof(socketAddress);
    if (getsockname(s, (struct sockaddr*)socketAddress, &length) < 0) {
        std::cerr << "Error getsockname\n";
        exit(1);
    }
    std::cout << "The server passive port number is " << ntohs(socketAddress->sin_port) << std::endl;
}

Status Socket::UDPsend(UDPMessage* m, SocketAddress* destination)
{
    Status status;
    status = Status::Ok;

    connect(s, (struct sockaddr*)destination, sizeof(struct sockaddr));

    const char* mess = "This is a test message";
    std::cout << "Sending message: \"" << mess << "\"" << '\n';
    std::cout << "To the address: " << inet_ntoa(destination->sin_addr)
              << ":" << ntohs(destination->sin_port) << '\n';
    send(s, mess, strlen(mess), 0);

    return status;
}

Status Socket::UDPreceive(UDPMessage** m, SocketAddress* origin)
{
    Status status;
    status = Status::Ok;

    if (listen(s, 5) < 0) {
        perror("Error calling listen\n");
        status = Status::Bad;
    }

    int clientSocket = accept(s, nullptr, nullptr);

    char buffer[SIZE];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Message received: " << buffer << std::endl;

    return status;
}
