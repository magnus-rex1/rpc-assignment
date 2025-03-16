#include "socket.h"

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
