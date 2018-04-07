#include "head.h"

size_t vscnprintf(IN CHAR *pcBuf, IN size_t ulLen, IN const CHAR *pcFmt, IN va_list pAp)
{
    size_t ulNum = vsnprintf(pcBuf, ulLen, pcFmt, pAp);
    size_t ulFinal = (ulNum> ulLen) ? ulLen : ulNum;

    return ulFinal;
}

size_t scnprintf(IN CHAR *pcBuf, IN size_t ulLen, IN const CHAR *pcFmt, ...)
{
    va_list pAp;
    va_start(pAp ,pcFmt);
    size_t ulFinal = vscnprintf(pcBuf, ulLen, pcFmt, pAp);
    va_end(pAp);

    return ulFinal;
}

CHAR *strlcpy(IN CHAR *pcDest, IN const CHAR *pcSrc, IN size_t ulLen)
{
    if ((NULL == pcDest) || (NULL == pcSrc))
    {
        return pcDest;
    }

    ULONG ulSrcLen = strlen(pcSrc) ;
    ULONG ulCopyLen = ulSrcLen > (ulLen - 1) ? (ulLen - 1) :  ulSrcLen;
    memcpy(pcDest, pcSrc, ulCopyLen);

    pcDest[ulCopyLen] = '\0';

    return pcDest;
}

CHAR *strlcat(IN CHAR *pcDest, IN const CHAR *pcSrc, IN size_t ulLen)
{
    if ((NULL == pcDest) || (NULL == pcSrc))
    {
        return pcDest;
    }

    pcDest[ulLen - 1] = '\0';

    ULONG ulStartOff = strlen(pcDest);
    ULONG ulReserveLen = ulLen - ulStartOff;
    ULONG ulSrcLen = strlen(pcSrc) ;
    ULONG ulCopyLen = ulSrcLen > (ulReserveLen - 1) ? (ulReserveLen - 1) :  ulSrcLen;
    memcpy(pcDest + ulStartOff, pcSrc, ulCopyLen);

    pcDest[ulStartOff + ulCopyLen] = '\0';

    return pcDest;
}

