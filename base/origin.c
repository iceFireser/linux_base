#include "head.h"

#define GIT_URL https://github.com/iceFireser/linux_base

#define FILE_PATH "/home/h/demo/temp/open.txt"

INT main_(INT iArgc, CHAR *szArgv[])
{
    struct addrinfo stHint = {
                                .ai_flags = AI_PASSIVE,
                                .ai_family = AF_UNSPEC,
                                .ai_socktype = SOCK_DGRAM,
                                .ai_protocol = 0,
                                .ai_addrlen = 0,
                                .ai_addr = NULL,
                                .ai_canonname = NULL,
                                .ai_next = NULL};
    struct addrinfo *pstResult;
    INT iRet = getaddrinfo(NULL, szArgv[1], &stHint, &pstResult);



    return -1;
}


