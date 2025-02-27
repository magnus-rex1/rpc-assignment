#include <arpa/inet.h>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <ostream>
#include <sys/socket.h>
#include <unistd.h>

void printSA(struct sockaddr_in sa);
void makeDestSA(struct sockaddr_in* sa, const char* hostname, int port);
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
    Message(const char*, unsigned int); // message and length supplied
    explicit Message(unsigned int); // only the length

    // void SetMessage();
    const char* GetMessage();
    unsigned int GetLength();

    friend std::ostream& operator<<(std::ostream& os, Message* m)
    {
        return os << m->data << std::endl;
    }

private:
    const char* data;
    unsigned int length;
};

using UDPMessage = Message;

class Socket {
public:
    Socket();
    Socket(int);
    ~Socket();

    void init(int);
    Status UDPsend(UDPMessage* m, SocketAddress* destination);
    Status UDPreceive(UDPMessage** m, SocketAddress* origin);

private:
    int s; // socket descriptor
    SocketAddress* socketAddress;

    struct ClientSocket {
        int s;
        SocketAddress sa;
        socklen_t len;
    };
    std::array<ClientSocket, 5> clientSockets;
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
