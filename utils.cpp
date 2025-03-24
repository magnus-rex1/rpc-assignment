#include "socket.h"
#include <sys/select.h>

// SA - Socket Address
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

int recvfromtimeout(int s, char* buf, int len, struct sockaddr* to, socklen_t* to_len, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or datareceived
    n = select(s + 1, &fds, NULL, NULL, &tv);
    if (n == 0)
        return -2;
    if (n == -1)
        return -1;

    // data must be here, so do a normal recvfrom()
    return recvfrom(s, buf, len, 0, to, to_len);
}

int server_timeout(int s, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or datareceived
    n = select(s + 1, &fds, NULL, NULL, &tv);
    if (n == 0)
        return -2; // timeout
    if (n == -1)
        return -1; // error

    return 0;
}

int anyThingThere(int s)
{
    unsigned long read_mask;
    struct timeval timeout;
    int n;

    timeout.tv_sec = 2; /*seconds wait*/
    timeout.tv_usec = 0; /* micro seconds*/
    read_mask = (1 << s);
    if ((n = select(32, (fd_set*)&read_mask, 0, 0, &timeout)) < 0)
        perror("Select fail:\n");
    else
        printf("n = %d\n", n);
    return n;
}
