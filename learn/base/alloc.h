#ifndef _ALLOC_H_
#define _ALLOC_H_


/* 原子加 第一个参数为变量指针，第二个参数是要加的值 AO表示ATOMIC                           */
#define ATOMIC_ADD(ptr, value)         ((__typeof__(*(ptr)))__sync_add_and_fetch((ptr), (value)))
#define ATOMIC_ADD_EX(sum, value)      ((__typeof__(sum))__sync_add_and_fetch((&sum), (value)))

/* 原子减 */
#define ATOMIC_SUB(ptr, value)         ((__typeof__(*(ptr)))__sync_sub_and_fetch((ptr), (value)))
#define ATOMIC_SUB_EX(sum, value)      ((__typeof__(sum))__sync_sub_and_fetch((&sum), (value)))

VOID *H_Malloc(size_t ulSize);
VOID *H_Calloc(size_t ulNum, size_t ulSize);

VOID H_Free(VOID *p);

#define H_Free_struct(p) \
    free((ULONG *)p - 1); \
    ATOMIC_SUB(&g_stCount.ulAllocSumByte, sizeof(*p)); \
    ATOMIC_SUB(&g_stCount.ulHasUsedByte, sizeof(*p));



#endif
