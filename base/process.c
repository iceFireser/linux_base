#include "head.h"

/************************************************************************/

/* 消息队列 */
/* 原理：内核中申请结点挂到队列中直到进程读走它 */

#define PROCESS_MAX_SEND 64

typedef struct MSG_BUF
{
    LONG ulType;
    CHAR szText[PROCESS_MAX_SEND];
}MSG_BUF_S;

STATIC VOID Process_MsgSend(IN INT iQueueId, CHAR *pcText)
{
    MSG_BUF_S stBuf = {0};

    stBuf.ulType = 1;
    strlcpy(stBuf.szText, pcText, PROCESS_MAX_SEND);

    if (-1 == (msgsnd(iQueueId, (struct msgbuf*)&stBuf, strlen(stBuf.szText) + 1, 0)))
    {
        ERR_Sys("msgsend");
        return;
    }

    printf("msgsnd type:0x%lx %s\n", stBuf.ulType, stBuf.szText);

}
STATIC VOID Process_MsgRecv(IN INT iQueueId)
{
    MSG_BUF_S stBuf = {0};

    stBuf.ulType = 1;
    if (-1 == (msgrcv(iQueueId, (struct msgbuf*)&stBuf, PROCESS_MAX_SEND, 1, 0)))
    {
        ERR_Sys("msgrcv");
    }

    printf("msgrcv type:0x%lx %s\n", stBuf.ulType, stBuf.szText);

}

INT PROCESS_MsgMain(INT iArgc, CHAR *szArgv[])
{
    if (iArgc < 2)
    {
        ERR_Hint("parameter less");
        return -1;
    }

    key_t key;
    INT iMsgQueueId = -1;
    key = ftok(".", 'm');

    if (-1 == (iMsgQueueId = msgget(key, IPC_CREAT | 0660)))
    {
        ERR_Hint("msgget");
        return -1;
    }

    if (0 == strcmp(szArgv[1], "-s"))
    {
        Process_MsgSend(iMsgQueueId,  szArgv[2]);
    }
    else if (0 == strcmp(szArgv[1], "-c"))
    {
        Process_MsgRecv(iMsgQueueId);
    }
    else
    {
        ERR_Hint("parameter error");
    }
    return 1;
}

/************************************************************************/
/* 信号量 */

union semun
{
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};


#define PROCESS_SEM_MAX 1

STATIC INT Process_SemCreate(IN key_t key)
{
    const INT iMembers = 1;
    INT iSemId;
    if (-1 == (iSemId = semget(key, iMembers, IPC_CREAT|IPC_EXCL|0666)))
    {
        ERR_Sys("semget");
        return -1;
    }

    union semun unSemOpts;
    unSemOpts.val = PROCESS_SEM_MAX;
    for (UINT i = 0; i < iMembers; i++)
    {
        semctl(iSemId, i, SETVAL, unSemOpts);
    }
    printf("sem create\n");

    return iSemId;
}

STATIC VOID Process_SemDelete(IN INT iSemId)
{
    semctl(iSemId, 0, IPC_RMID);
    printf("sem delete\n");

    return;
}

STATIC VOID Process_SemWait(IN INT iSemId)
{
    const INT iMember = 0;
    struct sembuf stSemOpt = {iMember, 0, 0};
    INT iSemVal;
    iSemVal = semctl(iSemId, iMember, GETVAL);
    printf("iSemval = %d wait begin\n", iSemVal);
    if (-1 == (semop(iSemId, &stSemOpt, 1))) /* 第三个参数表示stSemOpt数组个数 */
    {
        ERR_Sys("semop");
        return;
    }

    iSemVal = semctl(iSemId, iMember, GETVAL);
    printf("iSemval = %d wait finish\n", iSemVal);

    return;
}

STATIC INT Process_SemOpen(IN key_t key)
{
    const INT iMembers = 1;
    INT iSemId;
    if (-1 == (iSemId = semget(key, 0, 0666)))
    {
        ERR_Sys("semget");
        return -1;
    }

    return iSemId;
}


STATIC VOID Process_SemContinue(IN INT iSemId)
{
    const INT iMember = 0;
    struct sembuf stSemOpt = {iMember, -1, 0};

    if (-1 == (semop(iSemId, &stSemOpt, 1))) /* 第三个参数表示stSemOpt数组个数 */
    {
        ERR_Sys("semop");
        return;
    }

    printf("continue has exec");

    return;
}


INT PROCESS_SemMain(INT iArgc, CHAR *szArgv[])
{
    if (iArgc < 2)
    {
        ERR_Hint("parameter less");
        return -1;
    }

    key_t key;
    key = ftok(".", 's');


    if (0 == strcmp(szArgv[1], "-w")) /* wait */
    {
        INT iSemId = Process_SemCreate(key);
        Process_SemWait(iSemId);
        Process_SemDelete(iSemId);
    }
    else if (0 == strcmp(szArgv[1], "-c"))/* continue */
    {
        INT iSemId = Process_SemOpen(key);
        Process_SemContinue(iSemId);
    }
    else
    {
        ERR_Hint("parameter error");
        return -1;
    }

    return 1;
}

/************************************************************************/
/* 共享内存 */

#define PROCESS_SHM_SIZE 1024 /* 共享内存大小 */

INT Process_ShmOpen(IN key_t key)
{
    INT iShmId;
    if (-1 == (iShmId = shmget(key, PROCESS_SHM_SIZE, IPC_CREAT|IPC_EXCL|0666)))
    {
        if (-1 == (iShmId = shmget(key, PROCESS_SHM_SIZE, 0666)))
        {
            ERR_Sys("shmget");
            return -1;
        }
    }


    return iShmId;
}

VOID *Process_ShmMap(IN INT iShmId) /* 将共享内存映射到本进程 */
{

    VOID *ptr = NULL;
    if (NULL == (ptr = shmat(iShmId, 0, 0)))
    {
        ERR_Sys("shmat");
    }

    return ptr;
}

VOID Process_ShmWrite(VOID *ptr, CHAR *pcText)
{
    strcpy(ptr, pcText);
    printf("Process_ShmWrite %s\n", (CHAR *)ptr);

    return;
}

VOID Process_ShmRead(VOID *ptr)
{

    printf("Process_ShmRead %s\n", (CHAR *)ptr);

    return;
}


INT PROCESS_ShmMain(INT iArgc, CHAR *szArgv[])
{
    if (iArgc < 2)
    {
        ERR_Sys("parameter less");
    }

    key_t key;
    key = ftok(".", 'S');

    INT iShmId = Process_ShmOpen(key);
    VOID *ptr = Process_ShmMap(iShmId);

    if (0 == strcmp(szArgv[1], "-w")) /* write */
    {
        Process_ShmWrite(ptr, szArgv[2]);
    }
    else if (0 == strcmp(szArgv[1], "-r")) /* read */
    {
        Process_ShmRead(ptr);
    }
    else
    {
        ERR_Sys("parameter error");
    }

    return 0;

}



