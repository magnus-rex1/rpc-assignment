#include "socket.h"

void printSA(struct sockaddr_in sa)
{
    printf("sa = %d, %s, %d\n",
        sa.sin_family, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
}

void makeDestSA(struct sockaddr_in* sa, const char* hostname, int port)
{
    struct hostent* host;

    sa->sin_family = AF_INET;
    if ((host = gethostbyname(hostname)) == NULL) {
        printf("Unknown host name\n");
        exit(-1);
    }
    sa->sin_addr = *(struct in_addr*)(host->h_addr);
    sa->sin_port = htons(port);
}

void makeReceiverSA(struct sockaddr_in* sa, int port)
{
    sa->sin_family = AF_INET;
    sa->sin_port = htons(port);
    sa->sin_addr.s_addr = htonl(INADDR_ANY);
}

// get sockaddr, IPv4 or IPv6:
void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
