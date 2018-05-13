
#ifndef _HEAD_H_
#define _HEAD_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>
#include <ctype.h>
#include <time.h>
#include <execinfo.h>

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/signalfd.h>
#include <sys/uio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>






#include "basetype.h"
#include "string_ex.h"
#include "lock.h"
#include "err.h"
#include "alloc.h"

#include "hmdb.h"
#include "algorithm.h"
#include "process.h"
#include "debug.h"


typedef struct EPOLL_EX_NODE
{
    INT iFd;
    EPOLL_PROC_PF pfCallBack;
}EPOLL_EX_NODE_S;

INT epoll_add(INT iMainfd, INT iFd, EPOLL_PROC_PF pfCallBack);

extern INT main_(INT iArgc, CHAR *szArgv[]);

extern INT g_iMainFd;

#define EPOLL_MAIN_ADD(iFd, pfCallBack) epoll_add(g_iMainFd, iFd, pfCallBack)

#define EPOLL_WAIT(g_iThread2Fd) \
if (g_iThread2Fd >= 0) \
{ \
    struct epoll_event _events[1]; \
    memset(_events, 0, sizeof(_events)); \
    for (;;) \
    { \
        int num = 0; \
        num = epoll_wait(g_iThread2Fd, _events, 1, 0); \
        if (num > 0) \
        { \
            EPOLL_EX_NODE_S *pstNode = (EPOLL_EX_NODE_S *)_events[0].data.ptr; \
            UINT uiEvents = _events[0].events; \
            EPOLL_EX_NODE_S *pstFreeNode = NULL; \
            if (((uiEvents & EPOLLERR) != 0) || ((uiEvents & EPOLLHUP) != 0)) \
            { \
                pstFreeNode = pstNode; \
            } \
            pstNode->pfCallBack(_events[0].events, pstNode->iFd); \
            if (pstFreeNode != NULL) \
            { \
                free(pstFreeNode); \
            }\
        }\
    }\
}

#endif

