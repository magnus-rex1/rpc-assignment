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

#define SIZE 1024

void printSA(struct sockaddr_in sa)
{
    printf("sa = %d, %s, %d\n",
        sa.sin_family, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
}

void makeDestSA(struct sockaddr_in* sa, const char* hostname, int port)
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

/*
 * Message implementation
 */

Message::Message(const char* message, unsigned int len)
{
    data = message;
    length = len;
}

Message::Message(unsigned int len)
{
    data = nullptr;
    length = len;
}

const char* Message::GetMessage()
{
    return data;
}

unsigned int Message::GetLength()
{
    return length;
}

// get sockaddr, IPv4 or IPv6:
void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * Socket implementation
 */

// create the socket and bind it to any local port
Socket::Socket()
{
    /*
     * create a socket
     * AF_INET - IPv4 Internet protocol
     * SOCK_STREAM - two way communication
     */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Problem creating socket\n";
        exit(1);
    }

    socketAddress = new SocketAddress();

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
    // printSA(*socketAddress);
}

// opens a socket and binds it to the given port
Socket::Socket(int port)
{
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Problem creating socket\n";
        exit(1);
    }

    socketAddress = new SocketAddress();

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

Socket::~Socket()
{
    delete socketAddress;
}

Status Socket::UDPsend(UDPMessage* m, SocketAddress* destination)
{
    Status status;
    status = Status::Ok;

    // connect(s, (struct sockaddr*)destination, sizeof(struct sockaddr));

    printSA(*socketAddress);
    const char* mess = m->GetMessage();
    std::cout << "Sending message: \"" << mess << "\"" << '\n';
    std::cout << "To the address: " << inet_ntoa(destination->sin_addr)
              << ":" << ntohs(destination->sin_port) << "\n\n";
    // send(s, mess, strlen(mess), 0); // This is used for stream sockets
    sendto(s, mess, strlen(mess), 0, (struct sockaddr*)destination, sizeof(struct sockaddr));

    return status;
}

Status Socket::UDPreceive(UDPMessage** m, SocketAddress* origin)
{
    Status status;
    status = Status::Ok;

    char buffer[SIZE];
    // recv(clientSocket, buffer, sizeof(buffer), 0);
    memset(buffer, 0, sizeof buffer);

    struct sockaddr_storage clientAddr;
    socklen_t clientLen = sizeof clientAddr;
    [[maybe_unused]] int len = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &clientLen);
    *origin = *(SocketAddress*)&clientAddr;

    std::cout << "Message received: " << buffer << '\n'
              << "From: ";

    char str[INET_ADDRSTRLEN];
    const char* packet = inet_ntop(clientAddr.ss_family, get_in_addr((struct sockaddr*)&clientAddr), str, sizeof str);
    std::cout << packet << "\n\n";

    *m = new UDPMessage(buffer, sizeof(buffer));

    return status;
}

/*
 * Client implementation
 */

Client::Client()
{
}

Status Client::DoOperation(UDPMessage* callMessage, UDPMessage* replyMessage, SocketAddress* server)
{
    Status status = UDPsend(callMessage, server);
    std::cout << "Waiting for reply message\n";
    status = UDPreceive(&replyMessage, server);
    return status;
}

/*
 * Server implementation
 */

Server::Server(int x)
    : Socket(x)
{
}

Status Server::GetRequest(UDPMessage* callMessage, SocketAddress* client)
{
    Status status = UDPreceive(&callMessage, client);

    std::string input;
    std::cout << "Enter a message: ";
    std::getline(std::cin, input);
    UDPMessage* mess = new UDPMessage(input.c_str(), input.length());
    status = UDPsend(mess, client);

    return status;
}

Status Server::SendReply(UDPMessage* replyMessage, SocketAddress* client)
{
    return UDPsend(replyMessage, client);
}
