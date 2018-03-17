#include "common.h"

static int listen_fd, connect_fd, epoll_fd;
static struct sockaddr_in     servaddr;
static char buf[BUFLENGTH];
static int iRet, recv_len, send_len;


void printf_x(void *pData_, int len)
{
    unsigned char *pData = (unsigned char *)pData_;
    int a = len / 8;
    int b = len % 8;
    if (b > 0)
        a++;
    int c = 0;

    time_t tm;
    time(&tm);

    while (a-- > 0)
    {
        printf("\r\n%#8llx : ", (unsigned long long )tm);

        if (0 == a)
        {
            for (int i = 0; i < b; i++)
            {
                printf("%#2x ", pData[c * 8 + i]);
            }
            goto sss;
        }

        for (int i = 0; i < 8; i++)
        {
            printf("%#2x ", pData[c * 8 + i]);
        }
sss:
        c++;
    }
}

void listenCallBack(int iFd, uint32_t events)
{
    if (0 != events & (EPOLLHUP | EPOLLERR))
    {
        close(listen_fd);
        return;
    }

    connect_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
    if (-1 == connect_fd)
    {
        perror("accept");
        return;
    }

    int recvBuf = BUFSIZE * BUFBASE;
    iRet = setsockopt(connect_fd, SOL_SOCKET, SO_RCVBUF, (const char *)&recvBuf, sizeof(int));
    if (-1 == iRet)
    {
        perror("setsockopt");
        return;
    }

    struct epoll_event epollEvent = {
        .events = EPOLLIN | EPOLLHUP | EPOLLERR,
        .data.fd = connect_fd,
    };

    iRet = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &epollEvent);
    if (-1 == iRet)
    {
        perror("epoll_ctl");
        return;
    }

    return;

}
void connectCallBack(int iFd, uint32_t events)
{
    if (0 != events & (EPOLLHUP | EPOLLERR))
    {
        (void)epoll_ctl(epoll_fd, EPOLL_CTL_DEL, iFd, NULL);
        close(iFd);
    }
    else
    {
        recv_len = recv(connect_fd, buf, BUFLENGTH, MSG_WAITALL);
        printf_x((void *)buf, recv_len);
        sleep(1);
        printf("sleep 1s\r\n");
    }

    return;
}

void(*getCallBack(int iFd, uint32_t events))(int, uint32_t)
{
    if (iFd == listen_fd)
    {
        return listenCallBack;
    }
    else
    {
        return connectCallBack;
    }
}


int main(int argc, char *argv[])
{
    epoll_fd = epoll_create(1);
    if (-1 == epoll_fd)
    {
        perror("epoll_create");
        return 0;
    }

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd)
    {
        perror("socket");
        return 0;
    }

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(PORT);//设置的端口为DEFAULT_PORT

    iRet =  bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr));
    if (-1 == iRet)
    {
        perror("bind");
        return 0;
    }

    iRet = listen(listen_fd, 10);
    if (-1 == iRet)
    {
        perror("listen");
        return 0;
    }

    struct epoll_event epollEvent = {
        .events = EPOLLIN | EPOLLHUP | EPOLLERR,
        .data.fd = listen_fd,
    };

    iRet = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &epollEvent);
    if (-1 == iRet)
    {
        perror("epoll_ctl");
        return 0;
    }

    struct epoll_event ets[MAXEVENT];

    while (1)
    {
        iRet = epoll_wait(epoll_fd, ets, 1, -1);
        if (-1 == iRet)
        {
            perror("epoll_wait");
            return 0;
        }
        else if (iRet > 0)
        {
            void (*pf)(int, uint32_t) = getCallBack(ets[0].data.fd, ets[0].events);
            pf(ets[0].data.fd, ets[0].events);
        }
    }

    return 0;
}
