#include "socket.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

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

Message::Message(unsigned char* message, unsigned int len)
{
    length = len;
    data = new unsigned char[len + 1];
    strcpy((char*)data, (char*)message);
}

Message::Message(unsigned int len)
{
    data = nullptr;
    length = len;
}

// Deep copy semantics
//
// Deep copy constructor
Message::Message(const Message& other)
    : data { nullptr }
{
    if (other.data) {
        data = new unsigned char[strlen((const char*)other.data) + 1];
        strcpy((char*)data, (const char*)other.data);
    }
}

// Deep copy assignment
Message& Message::operator=(const Message& other)
{
    if (this != &other) {
        delete[] data; // free old memory
        data = nullptr;
        if (other.data) {
            data = new unsigned char[strlen((char*)other.data) + 1];
            strcpy((char*)data, (char*)other.data);
        }
    }
    return *this;
}

unsigned char* Message::GetMessage() const
{
    return data;
}

unsigned int Message::GetLength() const
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
    socketAddress->sin_port = htons(port); // convert to network byte order

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
    std::cout << "The server port number is " << ntohs(socketAddress->sin_port) << std::endl;
    std::cout << "Start the client with this port number\n";
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
    std::cout << "\n----------------------------------------------------\n";
    std::cout << "Sending message: \"" << mess << "\"" << '\n';
    std::cout << "To the address: " << inet_ntoa(destination->sin_addr)
              << ":" << ntohs(destination->sin_port);
    std::cout << "\n----------------------------------------------------\n";

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

    [[maybe_unused]] int len = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &clientLen);

    *origin = *(SocketAddress*)&clientAddr;

    std::cout << "\n----------------------------------------------------\n";
    std::cout << "Message received: \"" << buffer << "\"\n"
              << "From: " << inet_ntoa(origin->sin_addr)
              << ':' << ntohs(origin->sin_port);
    std::cout << "\n----------------------------------------------------\n";

    // alternative way of getting the address of the sender(origin)
    // char s[INET_ADDRSTRLEN];
    // const char* addr = inet_ntop(AF_INET, &origin->sin_addr, s, sizeof s);
    // std::cout << addr << std::endl;

    UDPMessage mess((unsigned char*)buffer, len); // create a new UDPMessage
    **m = mess; // transfer ownership

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
    // std::cout << "DoOperation: " << *replyMessage << "\n";
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
    return UDPreceive(&callMessage, client);
}

Status Server::SendReply(UDPMessage* replyMessage, SocketAddress* client)
{
    return UDPsend(replyMessage, client);
}
