#include "socket.h"

/*
 * Client implementation
 */

Client::Client()
{
}

Status Client::DoOperation(UDPMessage* callMessage, UDPMessage* replyMessage, SocketAddress* server)
{
    Status status = UDPsend(callMessage, server);
    // std::cout << "Waiting for reply message\n";
    status = UDPreceive(&replyMessage, server);
    // std::cout << "DoOperation: " << *replyMessage << "\n";
    return status;
}
