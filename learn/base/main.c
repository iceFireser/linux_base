#include "head.h"
 
INT g_iMainFd = -1;

INT epoll_add(INT iMainfd, INT iFd, EPOLL_PROC_PF pfCallBack)
{
    INT iRet = -1;
    EPOLL_EX_NODE_S *pstNode = (EPOLL_EX_NODE_S *)H_Malloc(sizeof(EPOLL_EX_NODE_S));
    if (NULL != pstNode)
    {
        memset(pstNode, 0, sizeof(EPOLL_EX_NODE_S));
        pstNode->iFd = iFd;
        pstNode->pfCallBack = pfCallBack;

        struct epoll_event event;
        memset(&event, 0, sizeof(event));
        event.events = EPOLLIN;
        event.data.ptr = pstNode;
        iRet = epoll_ctl(iMainfd, EPOLL_CTL_ADD, iFd, &event);
        if (0 == iRet)
        {
            return ERROR_SUCCESS;
        }

        free(pstNode);
    }

    return ERROR_FAILED;

}



INT main(INT iArgc, CHAR *szArgv[])
{
    g_iMainFd = epoll_create(1);

    if (g_iMainFd >= 0)
    {
        INT iRet = main_(iArgc, szArgv);
        if (0 == iRet)
        {
            struct epoll_event events[1];
            memset(events, 0, sizeof(events));
            for (;;)
            {
                int num = 0;
                num = epoll_wait(g_iMainFd, events, 1, 0);
                if (num > 0)
                {
                    EPOLL_EX_NODE_S *pstNode = (EPOLL_EX_NODE_S *)events[0].data.ptr;
                    UINT uiEvents = events[0].events;
                    EPOLL_EX_NODE_S *pstFreeNode = NULL;

                    if (((uiEvents & EPOLLERR) != 0) || ((uiEvents & EPOLLHUP) != 0))
                    {
                        pstFreeNode = pstNode;
                    }

                    pstNode->pfCallBack(events[0].events, pstNode->iFd);
                    if (pstFreeNode != NULL)
                    {
                        free(pstFreeNode);
                    }

                }
            }

        }
    }

    return 0;
}

