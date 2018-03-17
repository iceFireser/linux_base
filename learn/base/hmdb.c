#include "head.h"

#define INDEXLEN_SIZE 4
#define SEP ':'
#define SPACE ' '
#define ENDLINE '\n'

#define PTR_SIZE 7
#define PTR_MAX 9999999 /* max file offset = 10**PTR_SZ - 1 */
#define HASH_SIZE 137
#define FREE_OFF 0
#define HASH_OFF PTR_SIZE

typedef struct hmdb_node
{
    INT iIndexFd;
    INT iDataFd;

    CHAR *pcIndexBuf;
    CHAR *pcDataBuf;
    CHAR *pcName;

    ULONG ulDataBufLen;
    ULONG ulIndexBufLen;
    ULONG ulNameLen;

    off_t lIndexOff;
    size_t ulIndexLen;

    off_t lDataOff;
    size_t ulDataLen;

    off_t lNextNodePtr;
    off_t lNodePtr;
    off_t lHashLinkHeadOff; /* hash冲突链开始的位置 */
    off_t lHashBeginOff; /* hsh数组开始的位置 */
    ULONG ulHashSize;
    ULONG ulDelOkCount;
    ULONG ulDelErrCount;
    ULONG ulFetchOkCount;
    ULONG ulFetchErrCount;
    ULONG ulNextRecCount;
    ULONG ulStor1;
    ULONG ulStor2;
    ULONG ulStor3;
    ULONG ulStor4;
    ULONG ulStorErr;
}HMDB_NODE_S;

STATIC HMDB_NODE_S * Hmdb_Alloc(UINT uiNameLen);
STATIC VOID Hmdb_Free(HMDB_NODE_S *pstHmdb);
STATIC ULONG Hmdb_hash(IN HMDB_NODE_S *pstHmdb, IN const CHAR *pcKey);
STATIC off_t Hmdb_ReadLinkHead(HMDB_NODE_S *pstHmdb, off_t lOffset);
STATIC off_t Hmdb_ReadIndex(HMDB_NODE_S *pstHmdb, off_t lOffsef);
STATIC VOID Hmdb_WriteData(IN HMDB_NODE_S *pstHmdb, IN const CHAR *pcData, IN off_t lOffset, IN INT iWhence);
STATIC VOID Hmdb_WriteIndex(IN HMDB_NODE_S *pstHmdb, IN const CHAR *pcKey,
                            IN off_t lOffset, IN INT iWhence, IN off_t lNextNodePtr);
STATIC VOID Hmdb_WritePtr(IN HMDB_NODE_S *pstHmdb, IN off_t lOffset, IN off_t lNextNodePtr);
STATIC VOID Hmdb_Dodelete(IN HMDB_NODE_S *pstHmdb);
STATIC INT Hmdb_FindFree(IN HMDB_NODE_S *pstHmdb, IN ULONG ulKeyLen, IN LONG ulDataLen);

STATIC HMDB_NODE_S * Hmdb_Alloc(UINT uiNameLen)
{
    HMDB_NODE_S *pstDbNode;

    ULONG ulNameLen     = uiNameLen + 5;    /* + 5 是为了文件后缀，例如.idx .dat 再加'\0'   */
    ULONG ulIndexBufLen = INDEXLEN_MAX + 1; /* + 1 是为了 '\0' */
    ULONG ulDataBufLen  = DATALEN_MAX + 1;  /* + 1 是为了 '\0' */

    VOID *p1 = H_Calloc(1, sizeof(HMDB_NODE_S));
    VOID *p2 = H_Malloc(ulNameLen);
    VOID *p3 = H_Malloc(ulIndexBufLen);
    VOID *p4 = H_Malloc(ulDataBufLen);

    if ((NULL != p1)&&(NULL != p2)&&(NULL != p3)&&(NULL != p4))
    {
        pstDbNode = p1;
        pstDbNode->iIndexFd   = pstDbNode->iDataFd = -1;
        pstDbNode->pcName     = p2;
        pstDbNode->pcIndexBuf = p3;
        pstDbNode->pcDataBuf  = p4;
        pstDbNode->ulNameLen     = ulNameLen;
        pstDbNode->ulIndexBufLen = ulIndexBufLen;
        pstDbNode->ulDataBufLen  = ulDataBufLen;
    }
    else
    {
        H_Free(p1);
        H_Free(p2);
        H_Free(p3);
        H_Free(p4);
    }

    return pstDbNode;

}

STATIC VOID Hmdb_Free(HMDB_NODE_S *pstHmdb)
{
    if (NULL == pstHmdb)
    {
        return;
    }

    if (pstHmdb->iIndexFd >= 0)
    {
        close(pstHmdb->iIndexFd);
    }

    if (pstHmdb->iDataFd >= 0)
    {
        close(pstHmdb->iIndexFd);
    }

    if (NULL != pstHmdb->pcIndexBuf)
    {
        H_Free(pstHmdb->pcIndexBuf);
    }

    if (NULL != pstHmdb->pcDataBuf)
    {
        H_Free(pstHmdb->pcDataBuf);
    }

    if (NULL != pstHmdb->pcName)
    {
        H_Free(pstHmdb->pcName);
    }

    H_Free(pstHmdb);

    return;
}

STATIC INT Hmdb_FindAndLock(IN HMDB_NODE_S *pstHmdb, IN const CHAR *pcKey, IN INT iWriteLock)
{
    off_t lOffset     = 0;
    off_t lNextOffset = 0;

    pstHmdb->lHashLinkHeadOff = (Hmdb_hash(pstHmdb, pcKey) + pstHmdb->lHashBeginOff);
    pstHmdb->lNodePtr = pstHmdb->lHashLinkHeadOff;

    if (iWriteLock)
    {
        File_WriteWaitLock(pstHmdb->iIndexFd, pstHmdb->lHashLinkHeadOff, SEEK_SET, 1);
    }
    else
    {
        File_ReadWaitLock(pstHmdb->iIndexFd, pstHmdb->lHashLinkHeadOff, SEEK_SET, 1);
    }

    lOffset = Hmdb_ReadLinkHead(pstHmdb, pstHmdb->lNodePtr);
    while(lOffset != 0)
    {
        lNextOffset = Hmdb_ReadIndex(pstHmdb, lOffset);
        if (strcmp(pstHmdb->pcIndexBuf, pcKey) == 0)
        {
            break;
        }

        pstHmdb->lNodePtr = lOffset;
        lOffset = lNextOffset;
    }

    return (0 == lOffset ? -1 : 0);

}

STATIC ULONG Hmdb_hash(IN HMDB_NODE_S *pstHmdb, IN const CHAR *pcKey)
{
    ULONG ulHashValue = 0;
    CHAR c = 0;
    INT  i = 0;

    /* 充分均匀hash散列 */
    for (i = 1; (c = *pcKey++) != 0; i++)
    {
        ulHashValue += c * i;
    }

    ulHashValue = ulHashValue % pstHmdb->ulHashSize;

    return ulHashValue;
}

STATIC off_t Hmdb_ReadLinkHead(HMDB_NODE_S *pstHmdb, off_t lOffset)
{
    CHAR szAsciiPtr[PTR_SIZE + 1] = {0}; /* + 1 是为了'\0' */

    lseek(pstHmdb->iIndexFd, lOffset, SEEK_SET);

    LONG lLen = read(pstHmdb->iIndexFd, szAsciiPtr, PTR_SIZE);
    if (lLen != PTR_SIZE)
    {
        ERR_Sys("Hmdb_ReadLinkHead: read error of ptr field");
    }

    szAsciiPtr[PTR_SIZE] = '\0';

    return atol(szAsciiPtr);
}

STATIC off_t Hmdb_ReadIndex(HMDB_NODE_S *pstHmdb, off_t lOffsef)
{
    LONG lLen = 0;
    CHAR *pcPtr1 = NULL;
    CHAR *pcPtr2 = NULL;
    CHAR szAsciiPtr[PTR_SIZE + 1]      = {0};
    CHAR szAsciiLen[INDEXLEN_SIZE + 1] = {0};
    struct iovec astIov[2];

    pstHmdb->lIndexOff = lseek(pstHmdb->iIndexFd, lOffsef, 0 == lOffsef ? SEEK_CUR : SEEK_SET);

    astIov[0].iov_base = szAsciiPtr;
    astIov[0].iov_len = PTR_SIZE;
    astIov[0].iov_base = szAsciiLen;
    astIov[0].iov_len = INDEXLEN_SIZE;

    lLen = readv(pstHmdb->iIndexFd, &astIov[0], 2);
    if (lLen != (PTR_SIZE + INDEXLEN_SIZE))
    {
        if ((0 == lLen) && (0 == lOffsef))
        {
            return -1; /* 读到文件结束符 */
        }
        ERR_Sys("Hmdb_ReadIndex: readv error of index record");
    }

    szAsciiPtr[PTR_SIZE] = '\0';
    pstHmdb->lNextNodePtr = atol(szAsciiPtr);
    szAsciiLen[INDEXLEN_SIZE] = '\0';
    pstHmdb->ulIndexLen = (ULONG)atol(szAsciiLen);

    if ((pstHmdb->ulIndexLen < INDEXLEN_MIN) || (pstHmdb->ulIndexLen > INDEXLEN_MAX))
    {
        ERR_Sys("Hmdb_ReadIndex: invalid length");
    }

    lLen = read(pstHmdb->iIndexFd, pstHmdb->pcIndexBuf, pstHmdb->ulIndexLen);
    if (lLen != pstHmdb->ulIndexLen)
    {
        ERR_Sys("Hmdb_ReadIndex: read error of index record");
    }

    if (pstHmdb->pcIndexBuf[pstHmdb->ulIndexLen - 1] != ENDLINE)
    {
        ERR_Sys("Hmdb_ReadIndex: missing endline");
    }

    /* 用null替换endline */
    pstHmdb->pcIndexBuf[pstHmdb->ulIndexLen - 1] = '\0';


    pcPtr1 = strchr(pstHmdb->pcIndexBuf, SEP);
    if (NULL == pcPtr1)
    {
        ERR_Sys("Hmdb_ReadIndex: missing first separator");
    }

    /* 用null替换SEP */
    *pcPtr1++ = '\0';

    pcPtr2 = strchr(pcPtr1, SEP);
    if (NULL == pcPtr2)
    {
        ERR_Sys("Hmdb_ReadIndex: missing second separator");
    }

    /* 用null替换SEP */
    *pcPtr2++ = '\0';

    if (strchr(pcPtr2, SEP) != NULL)
    {
        ERR_Sys("Hmdb_ReadIndex: too many separators");
    }

    pstHmdb->lDataOff = atol(pcPtr1);
    if (pstHmdb->lDataOff < 0)
    {
        ERR_Sys("Hmdb_ReadIndex: starting offset < 0");
    }

    pstHmdb->ulDataLen = atol(pcPtr2);
    if ((pstHmdb->ulDataLen <= 0) || (pstHmdb->ulDataLen > DATALEN_MAX))
    {
        ERR_Sys("Hmdb_ReadIndex: invalid length");
    }

    return pstHmdb->lNextNodePtr;
}

STATIC CHAR *Hmdb_ReadData(HMDB_NODE_S *pstHmdb)
{
    lseek(pstHmdb->iDataFd, pstHmdb->lDataOff, SEEK_SET);

    LONG lLen = read(pstHmdb->iDataFd, pstHmdb->pcDataBuf, pstHmdb->ulDataLen);
    if (lLen != pstHmdb->ulDataLen)
    {
        ERR_Sys("Hmdb_ReadData: read error");
    }

    if (pstHmdb->pcDataBuf[pstHmdb->ulDataLen - 1] != ENDLINE)
    {
        ERR_Sys("Hmdb_ReadData: missing ENDLINE");
    }

    /* 用null替换endline */
    pstHmdb->pcDataBuf[pstHmdb->ulDataLen - 1] = '\0';

    return pstHmdb->pcDataBuf;
}

STATIC VOID Hmdb_Dodelete(HMDB_NODE_S *pstHmdb)
{
    INT i = 0;
    CHAR *pcPtr = NULL;
    off_t lFreePtr = 0;
    off_t lSavePtr = 0;

    for (pcPtr = pstHmdb->pcDataBuf, i = 0; i < (pstHmdb->ulDataLen - 1); i++)
    {
        *pcPtr++ = SPACE;
    }

    *pcPtr = '\0';
    pcPtr = pstHmdb->pcIndexBuf;

    while(*pcPtr)
    {
        *pcPtr++ = SPACE;
    }

    File_WriteWaitLock(pstHmdb->iDataFd, pstHmdb->lDataOff, SEEK_SET, 1);

    Hmdb_WriteData(pstHmdb, pstHmdb->pcDataBuf, pstHmdb->lDataOff, SEEK_SET);

    lFreePtr = Hmdb_ReadLinkHead(pstHmdb, FREE_OFF);

    lSavePtr = pstHmdb->lNextNodePtr;

    Hmdb_WriteIndex(pstHmdb, pstHmdb->pcIndexBuf, pstHmdb->lIndexOff, SEEK_SET, lFreePtr);

    Hmdb_WritePtr(pstHmdb, FREE_OFF, pstHmdb->lIndexOff);

    Hmdb_WritePtr(pstHmdb, pstHmdb->lHashLinkHeadOff, lSavePtr);

    File_UnLock(pstHmdb->iIndexFd, FREE_OFF, SEEK_SET, 1);

    return;
}

STATIC VOID Hmdb_WriteData(IN HMDB_NODE_S *pstHmdb, IN const CHAR *pcData, IN off_t lOffset, IN INT iWhence)
{
    struct iovec astIov[2] = {0};
    static CHAR cEndline = ENDLINE;

    if (SEEK_END == iWhence)
    {
        File_WriteWaitLock(pstHmdb->iDataFd, 0, SEEK_SET, 0);
    }

    pstHmdb->lDataOff = lseek(pstHmdb->iDataFd, lOffset, iWhence);
    if (-1 == pstHmdb->lDataOff)
    {
        ERR_Sys("Hmdb_WriteData: lseek error");
    }


    pstHmdb->ulDataLen = strlen(pcData) + 1;

    astIov[0].iov_base = (CHAR *)pcData;
    astIov[0].iov_len = pstHmdb->ulDataLen - 1;
    astIov[1].iov_base = &cEndline;
    astIov[1].iov_len = 1;

    LONG lLen = writev(pstHmdb->iDataFd, &astIov[0], 2);
    if (lLen != pstHmdb->ulDataLen)
    {
        ERR_Sys("Hmdb_WriteData: writev error of data record");
    }

    if (SEEK_END == iWhence)
    {
        File_UnLock(pstHmdb->iDataFd, 0, SEEK_SET, 0);
    }

    return;

}

STATIC VOID Hmdb_WriteIndex(IN HMDB_NODE_S *pstHmdb, IN const CHAR *pcKey,
                            IN off_t lOffset, IN INT iWhence, IN off_t lNextNodePtr)
{
    struct iovec astIov[2] = {0};
    CHAR szAsciiPtrLen[PTR_SIZE + INDEXLEN_SIZE + 1] = {0};
    ULONG ulLen = 0;
    LONG lLen = 0;

    if ((lNextNodePtr < 0) || (lNextNodePtr > PTR_MAX))
    {
        ERR_Sys("Hmdb_WriteIndex: invalid ptr: %d", lNextNodePtr);
    }

    pstHmdb->lNextNodePtr = lNextNodePtr;
    scnprintf(pstHmdb->pcIndexBuf, pstHmdb->ulIndexBufLen,
              "%s%c%ld%c%ld\n", pcKey, SEP, pstHmdb->lDataOff, SEP, (LONG)pstHmdb->ulDataLen);
    ulLen = strlen(pstHmdb->pcIndexBuf);

    if ((ulLen < INDEXLEN_MIN) || (ulLen > INDEXLEN_MAX))
    {
        ERR_Sys("Hmdb_WriteIndex: invalid length");
    }

    scnprintf(szAsciiPtrLen, sizeof(szAsciiPtrLen), "%*ld%*d", PTR_SIZE, lNextNodePtr, INDEXLEN_SIZE, ulLen);

    if (SEEK_END == iWhence)
    {
        File_WriteWaitLock(pstHmdb->iIndexFd, ((pstHmdb->ulHashSize + 1) * PTR_SIZE) + 1, SEEK_SET, 0);
    }

    pstHmdb->lIndexOff = lseek(pstHmdb->iIndexFd, lOffset, iWhence);
    if (-1 == pstHmdb->lIndexOff)
    {
        ERR_Sys("Hmdb_WriteIndex: lseek error");
    }

    astIov[0].iov_base = szAsciiPtrLen;
    astIov[0].iov_len = PTR_SIZE + INDEXLEN_SIZE;
    astIov[1].iov_base = pstHmdb->pcIndexBuf;
    astIov[1].iov_len = ulLen;

    lLen = writev(pstHmdb->iIndexFd, &astIov[0], 2);
    if (lLen != (PTR_SIZE + INDEXLEN_SIZE + ulLen))
    {
        ERR_Sys("Hmdb_WriteIndex: writev error of index record");
    }

    if (SEEK_END == iWhence)
    {
        File_UnLock(pstHmdb->iIndexFd, ((pstHmdb->ulHashSize + 1) * PTR_SIZE) + 1, SEEK_SET, 0);
    }

    return;
}

STATIC VOID Hmdb_WritePtr(IN HMDB_NODE_S *pstHmdb, IN off_t lOffset, IN off_t lNextNodePtr)
{
    CHAR szAsciiPtr[PTR_SIZE + 1] = {0};

    if ((lNextNodePtr < 0) || (lNextNodePtr > PTR_MAX))
    {
        ERR_Sys("Hmdb_WritePtr: invalid ptr: %d", lNextNodePtr);
    }

    scnprintf(szAsciiPtr, sizeof(szAsciiPtr), "%*ld", PTR_SIZE, lNextNodePtr);

    lseek(pstHmdb->iIndexFd, lOffset, SEEK_SET);

    LONG lLen = write(pstHmdb->iIndexFd, szAsciiPtr, PTR_SIZE);
    if (lLen != PTR_SIZE)
    {
        ERR_Sys("Hmdb_WritePtr: write error of ptr field");
    }

    return;
}

STATIC INT Hmdb_FindFree(IN HMDB_NODE_S *pstHmdb, IN ULONG ulKeyLen, IN LONG ulDataLen)
{
    INT iRec = 0;
    off_t lOffset = 0;
    off_t lNextOffset = 0;
    off_t lSaveOffset = 0;

    File_WriteWaitLock(pstHmdb->iIndexFd, FREE_OFF, SEEK_SET, 1);

    lSaveOffset = FREE_OFF;
    lOffset = Hmdb_ReadLinkHead(pstHmdb, lSaveOffset);

    while(lOffset != 0)
    {
        lNextOffset = Hmdb_ReadIndex(pstHmdb, lOffset);
        if ((strlen(pstHmdb->pcIndexBuf) == ulKeyLen) && (pstHmdb->ulDataLen == ulDataLen))
        {
            break;
        }
        lSaveOffset = lOffset;
        lOffset = lNextOffset;
    }

    if (0 == lOffset)
    {
        iRec = -1;
    }
    else
    {
        Hmdb_WritePtr(pstHmdb, lSaveOffset, pstHmdb->lNextNodePtr);
        iRec = 0;
    }

    File_UnLock(pstHmdb->iIndexFd, FREE_OFF, SEEK_SET, 1);

    return iRec;
}

HMDB_HANDLE HMDB_Open(IN const CHAR *pcPathName, IN INT iFlag, ...)
{
    HMDB_NODE_S *pstHmdb;
    INT iPathNameLen = 0;
    INT iMode = 0;
    INT iRet = ERROR_SUCCESS;
    INT i = 0;
    LONG lLen = 0;
    ULONG ulLen = 0;

    CHAR szAscii[PTR_SIZE + 1];
    memset(szAscii, 0, sizeof(szAscii));
    CHAR szHash[(HASH_SIZE + 1) * PTR_SIZE + 2]; /* 空闲链 + 哈希格数组 + '\n' + '\0' */
    memset(szHash, 0, sizeof(szHash));

    struct stat stFileStat;
    memset(&stFileStat, 0, sizeof(stFileStat));

    iPathNameLen = strlen(pcPathName);

    pstHmdb = Hmdb_Alloc(iPathNameLen);
    if (NULL == pstHmdb)
    {
        return NULL;
    }

    pstHmdb->ulHashSize = HASH_SIZE;
    pstHmdb->lHashBeginOff = HASH_OFF;

    strlcpy(pstHmdb->pcName, pcPathName, pstHmdb->ulNameLen);
    strlcat(pstHmdb->pcName, ".idx", pstHmdb->ulNameLen);

    if (iFlag & O_CREAT)
    {
        va_list pAp;

        va_start(pAp, iFlag);
        iMode = va_arg(pAp, INT);
        va_end(pAp);

        pstHmdb->iIndexFd = open(pstHmdb->pcName, iFlag, iMode);
        strlcpy(pstHmdb->pcName, pcPathName, pstHmdb->ulNameLen);
        strlcat(pstHmdb->pcName, ".dat", pstHmdb->ulNameLen);
        pstHmdb->iDataFd = open(pstHmdb->pcName, iFlag, iMode);

    }
    else
    {
        pstHmdb->iIndexFd = open(pstHmdb->pcName, iFlag);
        strlcpy(pstHmdb->pcName, pcPathName, pstHmdb->ulNameLen);
        strlcat(pstHmdb->pcName, ".dat", pstHmdb->ulNameLen);
        pstHmdb->iDataFd = open(pstHmdb->pcName, iFlag);
    }

    if ((pstHmdb->iIndexFd < 0) || (pstHmdb->iDataFd < 0))
    {

        Hmdb_Free(pstHmdb);
        return NULL;
    }

    if ((iFlag & (O_CREAT | O_TRUNC)) == (O_CREAT | O_TRUNC))
    {
        File_WriteWaitLock(pstHmdb->iIndexFd, 0, SEEK_SET, 0);

        iRet = fstat(pstHmdb->iIndexFd, &stFileStat);
        if (iRet < 0)
        {
            ERR_Sys("HMDB_Open fstat faild");
        }

        if (0 == stFileStat.st_size)
        {
            /* 建立索引文件 */
            scnprintf(szAscii, sizeof(szAscii), "%*d", PTR_SIZE, 0);
            szHash[0] = 0;
            for (i = 0; i < (HASH_SIZE + 1); i++)
            {
                strlcat(szHash, szAscii, sizeof(szHash));
            }

            strlcat(szHash, "\n", sizeof(szHash));

            ulLen = strlen(szHash);
            lLen = write(pstHmdb->iIndexFd, szHash, ulLen);
            if ((ULONG)lLen != ulLen)
            {
                ERR_Sys("HMDB_Open write error");
            }

        }

        File_UnLock(pstHmdb->iIndexFd, 0, SEEK_SET, 0);

        HMDB_Rewind(pstHmdb);
        return pstHmdb;

    }



}

VOID HMDB_Close(IN HMDB_HANDLE pHandle)
{
    HMDB_NODE_S *pstHmdb = pHandle;
    Hmdb_Free(pstHmdb);

    return;
}

CHAR *HMDB_Fetch(IN HMDB_HANDLE pHandle, IN const CHAR *pcKey)
{
    HMDB_NODE_S *pstHmdb = pHandle;
    CHAR *pcPtr;

    INT iRet = 0;
    iRet = Hmdb_FindAndLock(pstHmdb, pcKey, 0);
    if (iRet < 0)
    {
        pcPtr = NULL;
        pstHmdb->ulFetchErrCount++;
    }
    else
    {
        pcPtr = Hmdb_ReadData(pstHmdb);
        pstHmdb->ulFetchOkCount++;
    }

    File_UnLock(pstHmdb->iIndexFd, pstHmdb->lHashLinkHeadOff, SEEK_SET, 1);

    return pcPtr;

}

INT HMDB_Delete(IN HMDB_HANDLE handle, IN const CHAR *pcKey)
{
    HMDB_NODE_S *pstHmdb = handle;
    INT iRc = 0;

    INT iRet = Hmdb_FindAndLock(pstHmdb, pcKey, 1);
    if (0 == iRet)
    {
        Hmdb_Dodelete(pstHmdb);
        pstHmdb->ulDelOkCount++;
    }
    else
    {
        iRc = -1; /* 没有找到 */
        pstHmdb->ulDelErrCount++;
    }

    File_UnLock(pstHmdb->iIndexFd, pstHmdb->lHashLinkHeadOff, SEEK_SET, 1);

    return iRc;
}

INT HMDB_Store(IN HMDB_HANDLE handle, IN const CHAR *pcKey, IN const CHAR *pcData, IN INT iFlag)
{
    HMDB_NODE_S *pstHmdb = handle;
    INT iRc = 0;
    ULONG ulKeyLen = 0;
    ULONG ulDataLen = 0;
    off_t lNextNodePtr = 0;

    if ((iFlag != HMDB_INSERT) && (iFlag != HMDB_REPLACE) && (iFlag != HMDB_STORE))
    {
        errno = EINVAL;
        return -1;
    }

    ulKeyLen = strlen(pcKey);
    ulDataLen = strlen(pcData) + 1; /* + 1是为了'\n' */
    if ((ulDataLen < DATALEN_MIN) || (ulDataLen > DATALEN_MAX))
    {
        ERR_Sys("HMDB_Store: invalid data length");
    }

    INT iRet = Hmdb_FindAndLock(pstHmdb, pcKey, 1);
    /* 没有找到 */
    if (iRet < 0)
    {
        if (HMDB_REPLACE == iFlag)
        {
            iRc = -1;
            pstHmdb->ulStorErr++;
            errno = ENOENT;
            goto DO_RETURN;
        }

        lNextNodePtr = Hmdb_ReadLinkHead(pstHmdb, pstHmdb->lHashLinkHeadOff);

        iRet = Hmdb_FindFree(pstHmdb, ulKeyLen, ulDataLen);

        /* 无法找到空闲空间 */
        if (iRet < 0)
        {
            Hmdb_WriteData(pstHmdb, pcData, 0, SEEK_END);
            Hmdb_WriteIndex(pstHmdb, pcKey, 0, SEEK_END, lNextNodePtr);

            Hmdb_WritePtr(pstHmdb, pstHmdb->lHashLinkHeadOff, pstHmdb->lIndexOff);
            pstHmdb->ulStor1++;
        }
        else
        {
            Hmdb_WriteData(pstHmdb, pcData, pstHmdb->lDataOff, SEEK_SET);
            Hmdb_WriteIndex(pstHmdb, pcKey, pstHmdb->lIndexOff, SEEK_SET, lNextNodePtr);

            Hmdb_WritePtr(pstHmdb, pstHmdb->lHashLinkHeadOff, pstHmdb->lIndexOff);
            pstHmdb->ulStor2++;
        }
    }
    else
    {
        if (HMDB_INSERT == iFlag)
        {
            iRc = 1; /* 已经存在 */
            pstHmdb->ulStorErr++;
            goto DO_RETURN;
        }

        if (ulDataLen != pstHmdb->ulDataLen)
        {
            Hmdb_Dodelete(pstHmdb);

            lNextNodePtr = Hmdb_ReadLinkHead(pstHmdb, pstHmdb->lHashLinkHeadOff);

            Hmdb_WriteData(pstHmdb, pcData, 0, SEEK_END);
            Hmdb_WriteIndex(pstHmdb, pcKey, 0, SEEK_END, lNextNodePtr);

            Hmdb_WritePtr(pstHmdb, pstHmdb->lHashLinkHeadOff, pstHmdb->lIndexOff);
            pstHmdb->ulStor3++;
        }
        else
        {
            Hmdb_WriteData(pstHmdb, pcData, pstHmdb->lDataOff, SEEK_SET);
            pstHmdb->ulStor4++;
        }

    }

    iRc = 0; /* 成功 */

DO_RETURN:
    File_UnLock(pstHmdb->iIndexFd, pstHmdb->lHashLinkHeadOff, SEEK_SET, 1);

    return iRc;

}

VOID HMDB_Rewind(IN HMDB_HANDLE handle)
{
    HMDB_NODE_S *pstHmdb = handle;
    off_t lOffset;

    lOffset = (pstHmdb->ulHashSize + 1) * PTR_SIZE; /* +1 是为了空闲链 */
    pstHmdb->lIndexOff = lseek(pstHmdb->iIndexFd, lOffset + 1, SEEK_SET); /* + 1是因为hash数组末尾有'/n' */

    return;

}


CHAR *HMDB_NextRec(IN HMDB_HANDLE handle, IN CHAR *pcKey, IN ULONG ulLen)
{
    HMDB_NODE_S *pstHmdb = NULL;
    CHAR c = 0;
    CHAR *pcPtr = NULL;

    File_ReadWaitLock(pstHmdb->iIndexFd, FREE_OFF, SEEK_SET, 1);

    do
    {
        off_t lOffset = Hmdb_ReadIndex(pstHmdb, 0);
        if (lOffset < 0)
        {
            pcPtr = NULL;
            goto DO_RETURN;
        }

        pcPtr = pstHmdb->pcIndexBuf;

        while(((c = *pcPtr++) != 0) && (SPACE == c));

    }while(0 == c);

    if (pcKey != NULL)
    {
        strlcpy( pcKey, pstHmdb->pcIndexBuf, ulLen);
    }

    pcPtr = Hmdb_ReadData(pstHmdb);
    pstHmdb->ulNextRecCount++;

DO_RETURN:

    File_UnLock(pstHmdb->iIndexFd, FREE_OFF, SEEK_SET, 1);

    return pcPtr;
}
