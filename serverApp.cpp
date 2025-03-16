#include "socket.h"

int main(void)
{
    try {
        Server server(SERVER_PORT);

        SocketAddress* client = new SocketAddress;
        UDPMessage callMessage(1024);

        const char* ok = "Ok";

        while (1) {
            server.GetRequest(&callMessage, client);

            if (strcmp((const char*)callMessage.GetMessage(), "Stop") == 0) {
                std::cout << "Closing server, received \'Stop\'.\n";

                UDPMessage reply((unsigned char*)ok, strlen(ok));
                server.SendReply(&reply, client);
                break;
            }

            if (strcmp((const char*)callMessage.GetMessage(), "Ping") == 0) {
                UDPMessage reply((unsigned char*)ok, strlen(ok));
                server.SendReply(&reply, client);
                continue;
            }

            UDPMessage reply((unsigned char*)ok, strlen(ok));
            server.SendReply(&reply, client);
        }

        delete client;
        // server is destroyed by the OS when main returns

    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
