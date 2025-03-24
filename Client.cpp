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

    // anything();

    status = UDPreceive(&replyMessage, server);
    return status;
}
