#include <stdio.h>
#include <string.h>
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>

#define SERVER "server"
#define CLIENT "client"

#define SOCKET_ADDR "ipc:///tmp/pubsub.ipc"

int iRet;

int server(const char *url)
{
    int sock = nn_socket(AF_SP, NN_PUB);
    iRet = nn_bind(sock, url);
    char *str = " *********** ";
    printf("SERVER: PUBLISHING DATE %s\r\n", str);
    int bytes = nn_send(sock, str, strlen(str) + 1, 0);
    return nn_shutdown(sock, 0);
}

int client(const char *url, char *name)
{
    char *buf = NULL;
    int sock = nn_socket(AF_SP, NN_SUB);
    iRet = nn_setsockopt(sock, NN_SUB, NN_SUB_SUBSCRIBE, "", 0);
    iRet = nn_connect(sock, url);
    int bytes = nn_recv(sock, &buf, NN_MSG, 0);
    printf("CLIENT (%s): RECEIVED %s\n", name, buf);
    nn_freemsg(buf);

    return nn_shutdown(sock, 0);
}

int main(int argc, char *argv[])
{
     if (argc == 2 && strncmp(SERVER, argv[1], strlen(SERVER)) == 0)
     {
            return server(SOCKET_ADDR);
     }
     else if (argc == 3 && strncmp (CLIENT, argv[1], strlen (CLIENT)) == 0)
     {
            return client(SOCKET_ADDR, argv[2]);
     }
     else
     {
            fprintf(stderr, "Usage: pubsub %s|%s <ARG> ...\n", SERVER, CLIENT);
            return 1;
     }
    return 0;
}
