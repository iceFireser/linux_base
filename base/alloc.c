#include "head.h"

typedef struct total_Count
{
    ULONG ulAllocSumByte;
    ULONG ulFreeSumByte;
    ULONG ulHasUsedByte;
}TOTAL_COUNT_NODE;

TOTAL_COUNT_NODE g_stCount = {0};

VOID *H_Malloc(size_t ulSize)
{
    VOID *p = malloc(sizeof(ULONG) + ulSize);
    if (NULL != p)
    {
        ULONG *pulPoint = (ULONG *)p;
        *pulPoint = ulSize;
        p = (VOID *)(pulPoint + 1);
        ATOMIC_ADD(&g_stCount.ulAllocSumByte, ulSize);
        ATOMIC_ADD(&g_stCount.ulHasUsedByte, ulSize);
    }

    return p;
}

VOID *H_Calloc(size_t ulNum, size_t ulSize)
{
    VOID *p = calloc(ulNum, sizeof(ULONG) + ulSize);
    if (NULL != p)
    {
        ULONG *pulPoint = (ULONG *)p;
        *pulPoint = ulSize;
        p = (VOID *)(pulPoint + 1);
        ATOMIC_ADD(&g_stCount.ulAllocSumByte, ulSize);
        ATOMIC_ADD(&g_stCount.ulHasUsedByte, ulSize);
    }

    return p;
}


VOID H_Free(VOID *p)
{
    if (NULL == p)
    {
        return;
    }
    ULONG *pulLen = ((ULONG *)p - 1);
    ATOMIC_SUB(&g_stCount.ulAllocSumByte, *pulLen);
    ATOMIC_SUB(&g_stCount.ulHasUsedByte, *pulLen);
    free(pulLen);

    return;
}

#define H_Free_struct(p) \
    free((ULONG *)p - 1); \
    ATOMIC_SUB(&g_stCount.ulAllocSumByte, sizeof(*p)); \
    ATOMIC_SUB(&g_stCount.ulHasUsedByte, sizeof(*p));

