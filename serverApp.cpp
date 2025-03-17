#include "socket.h"
#include <string>

int main(void)
{
    try {
        Server* server = new Server(SERVER_PORT);

        SocketAddress* client = new SocketAddress;
        UDPMessage* callMessage = new UDPMessage(1024);

        RPCMessage rpc(MessageType::Request);

        const char* ok = "Ok";

        auto send_ok = [&]() {
            UDPMessage reply((unsigned char*)ok, strlen(ok));
            server->SendReply(&reply, client);
        };

        int left, right;
        unsigned int op;
        std::stringstream ss;
        int res;

        while (1) {
            server->GetRequest(callMessage, client);

            // ss << *callMessage;
            // std::cout << "call message:" << *callMessage << '\n';
            int mt;
            int y;
            ss >> mt >> y;
            if (ss >> op >> left >> right) {
                rpc.unmarshall(callMessage);
                std::cout << "RPC: " << rpc << std::endl;
                rpc.op(left, right, &res);
            }

            RPCMessage rpc_reply(MessageType::Reply, 0, 0, res);

            if (strcmp((const char*)callMessage->GetMessage(), "Stop") == 0) {
                std::cout << "Closing server, received \'Stop\'.\n";
                send_ok();
                break;
            }

            if (strcmp((const char*)callMessage->GetMessage(), "Ping") == 0) {
                send_ok();
                continue;
            }
            send_ok();

            if (mt == MessageType::Request) {
                UDPMessage* reply = new UDPMessage(SIZE);
                rpc_reply.marshall(&reply);
                server->SendReply(reply, client);
                // delete reply;

                UDPMessage reply0((unsigned char*)std::to_string(res).c_str(), strlen(ok));
                server->SendReply(&reply0, client);
            }
        }

        delete client;
        delete server;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
