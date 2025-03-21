#include "socket.h"

int main(void)
{
    try {
        Server* server = new Server(SERVER_PORT);

        SocketAddress* client = new SocketAddress;
        UDPMessage* callMessage = new UDPMessage(1024);

        std::stringstream ss;

        while (1) {
            server->GetRequest(callMessage, client);

            if (strcmp((const char*)callMessage->GetMessage(), "Stop") == 0) {
                std::cout << "Closing server, received \'Stop\'.\n";
                break;
            }

            if (strcmp((const char*)callMessage->GetMessage(), "Ping") == 0) {
                const char* ok = "Ok";
                UDPMessage reply((unsigned char*)ok, strlen(ok));
                server->SendReply(&reply, client);
                continue;
            }

            RPCMessage rpc(MessageType::Reply);
            rpc.unmarshall(callMessage);
            if (!rpc.isInvalid()) {
                if (rpc.eval() == Status::Bad) {
                    const char* str = "Bad operation";
                    Message bad_op((unsigned char*)str, strlen(str));
                    server->SendReply(&bad_op, client);
                    continue; // go back to the start of the loop
                }

                Message* reply = new Message(1); // UDPMessage
                rpc.marshall(&reply);
                server->SendReply(reply, client);
                delete reply;
            } else {
                const char* non_arithmetic = "Not an arithmetic expression";
                UDPMessage reply((unsigned char*)non_arithmetic, strlen(non_arithmetic));
                server->SendReply(&reply, client);
            }
        }

        delete client;
        delete server;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
