#ifndef BASETYPE_H_
#define BASETYPE_H_


typedef void VOID;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef unsigned short BOOL_T;
typedef int INT;
typedef unsigned int UINT;
typedef long long INT64;
typedef unsigned long long UINT64;
typedef long LONG;
typedef unsigned long ULONG;

#define BOOL_TURE 0x0001
#define BOOL_FALSE 0x0000

#define ERROR_SUCCESS 0x00000000
#define ERROR_FAILED  0x00000001

#define IN
#define OUT
#define INOUT

#define STATIC static

typedef VOID (*EPOLL_PROC_PF)(UINT uiEvent, int iFd);

#define SYMMETY_HEAD \
if (ERROR_SUCCESS == iRet) \
{

#define SYMMETY_TAIL \
}


#endif
