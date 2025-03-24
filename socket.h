#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <ostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 1024
#define SERVER_PORT IPPORT_RESERVED + getuid()

// Implementations in utils.cpp
void printSA(struct sockaddr_in sa);
void makeDestSA(struct sockaddr_in* sa, const char* hostname, int port);
void makeLocalSA(struct sockaddr_in* sa);
void makeReceiverSA(struct sockaddr_in* sa, int port);
int recvfromtimeout(int s, char* buf, int len, struct sockaddr* to, socklen_t* to_len, int timeout);
int anyThingThere(int s);
int server_timeout(int s, int timeout);
// --------------------------------------------------------------------

enum Status {
    Ok,
    Bad,
    WrongLength,
    TimeOut
};

// typedef struct sockaddr_in SocketAddress;
using SocketAddress = struct sockaddr_in;

class Message {
public:
    Message(unsigned char*, unsigned int); // message and length supplied
    explicit Message(unsigned int); // only the length

    Message(const Message&);
    Message& operator=(const Message&);

    ~Message() { delete[] data; }

    unsigned char* GetMessage() const;
    unsigned int GetLength() const;

    friend std::ostream& operator<<(std::ostream& os, const Message& m)
    {
        return os << m.data;
    }

private:
    unsigned char* data;
    unsigned int length;
};

using UDPMessage = Message; // UDPMessage is now an alias of Message

class Socket {
public:
    Socket();
    Socket(int);
    ~Socket();

    void init(int port = 0);
    Status UDPsend(UDPMessage* m, SocketAddress* destination);
    Status UDPreceive(UDPMessage** m, SocketAddress* origin);
    int anything(); // calls anyThingThere()

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

enum MessageType {
    Request,
    Reply
};

class RPCMessage {
public:
    RPCMessage(MessageType);
    RPCMessage(MessageType, int, int, int);
    void marshall(Message**); // Marshalls self to Message argument
    void unmarshall(Message*); // Unmarshalls from given argument to self

    Status eval();
    Status add(int, int, int*);
    Status sub(int, int, int*);
    Status mul(int, int, int*);
    Status div(int, int, int*);

    int getResult(); // extract the result (arg1)
    bool isInvalid(); // failed to unmarshall

    friend std::ostream& operator<<(std::ostream& os, const RPCMessage& m)
    {
        return os << m.requestId << ' '
                  << m.procedureId << ' '
                  << m.arg1 << ' '
                  << m.arg2;
    }

private:
    MessageType type;
    unsigned int requestId; // unique id
    unsigned int procedureId; // e.g. (1,2,3,4) for (+,-,*,/)
    int arg1, arg2; // arguments/ return parameters
};
