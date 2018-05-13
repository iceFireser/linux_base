#ifndef _PROCESS_H_
#define _PROCESS_H_

/* 消息队列试用函数 */
INT PROCESS_MsgMain(INT iArgc, CHAR *szArgv[]);
/* 信号量试用函数 */
INT PROCESS_SemMain(INT iArgc, CHAR *szArgv[]);
/* 共享内存试用函数 */
INT PROCESS_ShmMain(INT iArgc, CHAR *szArgv[]);

#endif
