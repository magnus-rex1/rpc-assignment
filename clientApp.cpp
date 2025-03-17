#include "socket.h"
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    if (argc <= 1) {
        std::cout << "Usage: <machine> <port>\n";
        exit(1);
    }

    int port;
    sscanf(argv[1], "%d", &port);

    try {
        SocketAddress* dest = new SocketAddress;
        const char* localhost = "127.0.0.1";
        makeDestSA(dest, localhost, port);

        Client* client = new Client();

        while (1) {
            std::string input;
            std::cout << "Enter a message: ";
            std::getline(std::cin, input);

            UDPMessage* callMessage = new UDPMessage(SIZE);
            UDPMessage* replyMessage = new UDPMessage(SIZE);

            std::stringstream ss(input);
            int left, right, op = -1;
            char c_op;
            if (ss >> left >> c_op >> right && (c_op == '+' || c_op == '-' || c_op == '*' || c_op == '/')) {
                switch (c_op) {
                case '+':
                    op = 1;
                    break;
                case '-':
                    op = 2;
                    break;
                case '*':
                    op = 3;
                    break;
                case '/':
                    op = 4;
                    break;
                default:
                    break;
                }

                RPCMessage rpc(MessageType::Request, op, left, right);
                rpc.marshall(&callMessage);
            } else {
                callMessage = new UDPMessage((unsigned char*)input.c_str(), input.length());
            }

            client->DoOperation(callMessage, replyMessage, dest);

            delete callMessage;
            delete replyMessage;
        }

        delete client;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
