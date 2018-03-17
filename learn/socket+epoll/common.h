#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <sys/epoll.h>

#define PORT 1987
#define MAXLINE 4096
#define BUFLENGTH 4096
#define BUFSIZE  1
#define BUFBASE  4096
#define MAXEVENT 1


