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

    status = UDPreceive(&replyMessage, server);
    return status;
}
