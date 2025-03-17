#include "socket.h"

int main(void)
{
    try {
        Server* server = new Server(SERVER_PORT);

        SocketAddress* client = new SocketAddress;
        UDPMessage* callMessage = new UDPMessage(1024);

        RPCMessage rpc(MessageType::Request);

        const char* ok = "Ok";
        UDPMessage reply((unsigned char*)ok, strlen(ok));

        auto send_ok = [&]() {
            server->SendReply(&reply, client);
        };

        int left, right;
        char op;
        std::stringstream ss;

        while (1) {
            server->GetRequest(callMessage, client);

            ss << *callMessage;
            std::cout << "call message:" << *callMessage << '\n';
            if (ss >> left && ss >> op && ss >> right) {
                rpc.unmarshall(callMessage);
                std::cout << "RPC: " << rpc << std::endl;
            }

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
        }

        delete client;
        delete server;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
