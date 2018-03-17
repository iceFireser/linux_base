#include "common.h"

typedef struct node{
    struct node *next;
}STQ_NODE;

typedef struct head{
    struct node *first;
    struct node *last;
}STQ_HEAD;

void STQ_Init(STQ_HEAD *head)
{
    head->last = NULL;
    head->first = NULL;
    return;
}

void STQ_AddTail(STQ_HEAD *head, STQ_NODE *node)
{
    if (head->first == NULL)
    {
        head->first = node;
    }
    else
    {
        head->last->next = node;
    }

    head->last = node;

    node->next = NULL;

    return;
}

STQ_NODE *STQ_GetFist(STQ_HEAD *head)
{
    return head->first;
}

void STQ_DelFirst(STQ_HEAD *head)
{
    struct node *first = head->first;
    if (head->first == head->last)
    {
        head->last = NULL;
        head->first = NULL;
    }

    head->first = first->next;
    free(first);

    return;
}

void STQ_FreeAll(STQ_HEAD *head)
{
    struct node *node = head->first;
    struct node *freeNode = node;
    while(node)
    {
        node = node->next;
        free(freeNode);
        freeNode = node;
    }

    head->last = NULL;
    head->first = NULL;

    return;
}

STQ_HEAD g_stListHead;

typedef struct dataNode
{
    STQ_NODE node;
    int dataCode;
}DATA_NODE_S;

static int epoll_fd;
static int iRet, recv_len, send_len;
static char buf[BUFLENGTH];
static int bIs = 0;


#define SEND_LEN (BUFSIZE * BUFBASE)

void callBack(int iFd, uint32_t events)
{
    printf("events %#x %#x \r\n", events, EPOLLOUT);

    if (0 != (events & (EPOLLHUP | EPOLLERR)) )
    {
        printf("epoll hup \r\n");
        (void)epoll_ctl(epoll_fd, EPOLL_CTL_DEL, iFd, NULL);
        close(iFd);
    }

    if (0 != (events & EPOLLOUT) )
    {
        printf("callback: epollout  iFd = %d\r\n", iFd);

        do {
            DATA_NODE_S *node = (DATA_NODE_S*)STQ_GetFist(&g_stListHead);
            if (NULL == node)
            {
                if (bIs == 1)
                {
                    bIs = 0;
                }
                struct epoll_event epollEvent = {
                    .events = EPOLLIN | EPOLLHUP | EPOLLERR,
                    .data.fd = iFd,
                };

                (void)epoll_ctl(epoll_fd, EPOLL_CTL_MOD, iFd, &epollEvent);

                printf("epollout callback list is all deal\r\n");
                break;

            }

            recv_len = send(iFd, buf, SEND_LEN, MSG_DONTWAIT);
            printf("send data %d Byte --epollout node id = %d \r\n", recv_len, node->dataCode);
            if (recv_len == SEND_LEN)
            {
                STQ_DelFirst(&g_stListHead);
            }
            else
            {
                break;
            }

        }while(1);

    }

    if (0 != (events & EPOLLIN))
    {
        printf("epollin \r\n");
    }

    return;
}


void dealAbnormal(int fd)
{
    printf("dealAbnormal\r\n");
    static int a = 0;

    DATA_NODE_S *dataNode = malloc(sizeof(DATA_NODE_S));
    if (NULL == dataNode)
    {
        printf("no memory\r\n");
        return;
    }

    dataNode->dataCode = a++;

    STQ_AddTail(&g_stListHead, &dataNode->node);

    struct epoll_event epollEvent = {
        .events = EPOLLOUT | EPOLLHUP | EPOLLERR,
        .data.fd = fd,
    };

    if (bIs == 0)
    {
        iRet = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &epollEvent);
        if (-1 == iRet)
        {
            perror("epoll_ctl");
            return;
        }
        bIs = 1;
    }


    return;
}

int main(int argc, char *argv[])
{
    int fd;
    struct sockaddr_in     addr;
    char buf[BUFLENGTH];

    STQ_Init(&g_stListHead);

    epoll_fd = epoll_create(1);
    if (-1 == epoll_fd)
    {
        perror("epoll_create");
        return 0;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
        perror("socket");
        return 0;
    }

    int sendBuf = BUFSIZE * BUFBASE;
    iRet = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char *)&sendBuf, sizeof(int));
    if (-1 == iRet)
    {
        perror("setsockopt");
        return 0;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    addr.sin_port = htons(PORT);//设置的端口为DEFAULT_PORT

    iRet = connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (-1 == fd)
    {
        perror("connect");
        return 0;
    }

    struct epoll_event epollEvent = {
        .events = EPOLLIN | EPOLLHUP | EPOLLERR,
        .data.fd = fd,
    };

    iRet = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &epollEvent);
    if (-1 == iRet)
    {
        perror("epoll_ctl");
        return 0;
    }

    int num = 10;

    //scanf("\r\n%d", &num);
    memset(buf, 0xFF, SEND_LEN);
    for (int i = 0; i < num; i++)
    {

       recv_len = send(fd, buf, SEND_LEN, MSG_DONTWAIT);
       printf("send data %d Byte index : ---- %d \r\n", recv_len, i);

       if (recv_len != SEND_LEN)
       {

            dealAbnormal(fd);
       }
    }

    struct epoll_event ets[MAXEVENT];
    do {


        iRet = epoll_wait(epoll_fd, ets, 1, -1);
        if (-1 == iRet)
        {
            perror("epoll_wait");
            return 0;
        }
        else if (iRet > 0)
        {
            void (*pf)(int, uint32_t) = callBack;
            pf(ets[0].data.fd, ets[0].events);
        }

    }while(num > 0);

    close(fd);
    STQ_FreeAll(&g_stListHead);
    close(epoll_fd);

    return 0;
}


