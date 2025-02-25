#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void printSA(struct sockaddr_in sa);
void makeDestSA(struct sockaddr_in* sa, char* hostname, int port);
void makeLocalSA(struct sockaddr_in* sa);
void makeReceiverSA(struct sockaddr_in* sa, int port);

enum Status {
    Ok,
    Bad,
    WrongLength
};

// typedef struct sockaddr_in SocketAddress;
using SocketAddress = struct sockaddr_in;

class Message {
public:
    Message(unsigned char*, unsigned int); // message and length supplied
    explicit Message(unsigned int); // only the length

private:
    unsigned char* data;
    unsigned int length;
};

using UDPMessage = Message;

class Socket {
public:
    Socket();
    Socket(int);
    Status UDPsend(UDPMessage* m, SocketAddress* destination);
    Status UDPreceive(UDPMessage** m, SocketAddress* origin);

private:
    int s; // socket descriptor
    SocketAddress* socketAddress;
};

class Client : public Socket {
public:
    Client(); // calls Socket()
    Status DoOperation(UDPMessage* callMesssage, UDPMessage* replyMessage, SocketAddress* server);
};

class Server : public Socket {
public:
    Server(int);
    Status GetRequest(UDPMessage* callMessage, SocketAddress* client);
    Status SendReply(UDPMessage* replyMessage, SocketAddress* client);
};
