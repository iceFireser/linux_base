#include "head.h"

#define SHOWBUF_MAXLEN 256

STATIC VOID ERR_Show(IN INT iErrnoFlag, IN INT iError, IN const CHAR *pcFmt, IN va_list pAp)
{
    CHAR szBuf[SHOWBUF_MAXLEN];
    memset(szBuf, 0, sizeof(szBuf));

    size_t ulLen = 0;

    ulLen += vscnprintf(szBuf, SHOWBUF_MAXLEN, pcFmt, pAp);

    if (0 != iErrnoFlag)
    {
        ulLen += scnprintf(szBuf + ulLen, SHOWBUF_MAXLEN - ulLen, ": %s", strerror(errno));
    }

    ulLen += scnprintf(szBuf + ulLen, SHOWBUF_MAXLEN - ulLen, "\n");

    fflush(stdout);
    fputs(szBuf, stderr);
    fflush(NULL);

    return;
}

VOID ERR_Dump(IN const CHAR *pcFmt, ...)
{
    va_list pAp;

    va_start(pAp, pcFmt);
    ERR_Show(1, errno, pcFmt, pAp);
    va_end(pAp);

    abort();
    exit(1);

    return;
}

VOID ERR_Sys(IN const CHAR *pcFmt, ...)
{
    va_list pAp;

    va_start(pAp, pcFmt);
    ERR_Show(1, errno, pcFmt, pAp);
    va_end(pAp);

    return;
}
