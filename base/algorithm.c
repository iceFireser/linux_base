#include "head.h"

/* 最短路径 */
/* shorttest path 缩写为 SP */

VOID ALGORITHM_SP_MapScanf(INOUT ALGORITHM_SP_MAP_S *pstMap)
{
    return;
}

VOID ALGORITHM_SP_MapPrint(IN ALGORITHM_SP_MAP_S *pstMap)
{
    UINT uiPointCount = pstMap->uiPointCount;
    UINT i = 0;
    UINT j = 0;

    printf("---------------MAP-TABLE-START-------------------\n");
    for (i = 0; i < uiPointCount; i++)
    {

        for (j = 0; j < uiPointCount; j++)
        {
            if (pstMap->aui[i][j] != ALGORITHM_SP_INVAILD)
            {
                printf("%8u", pstMap->aui[i][j]);
            }
            else
            {
                printf("     inf");
            }

        }
        printf("\n");
    }
    printf("----------------MAP-TABLE-END--------------------\n");
    return;
}

/* 主要思想：动态规划，为第条路径尝试       通过第一个点，第一个加第二个点，
   第一个加第二个加第三个点，等等*/
VOID  ALGORITHM_SP_FloydWarshall(INOUT ALGORITHM_SP_MAP_S *pstMap)
{

    UINT k = 0;
    UINT i = 0;
    UINT j = 0;

    UINT uiPointCount = pstMap->uiPointCount;


    /* 核心算法 */
    for (k = 0; k < uiPointCount; k++)
    {
        for (i = 0; i < uiPointCount; i++)
        {
            for (j = 0; j < uiPointCount; j++)
            {
                if (pstMap->aui[i][j] > pstMap->aui[i][k] + pstMap->aui[k][j])
                {
                    pstMap->aui[i][j] = pstMap->aui[i][k] + pstMap->aui[k][j];
                }
            }
        }
    }


    return;

}

/* 主要思想：从距离最短的点开始松驰，并继续从此点去寻找最短的点松驰 */
VOID ALGORITHM_SP_Dijkstra(IN ALGORITHM_SP_MAP_S *pstMap, OUT UINT auiDis[ALGORITHM_MAPSIDE_SIZE])
{
    UINT uiPointCount = pstMap->uiPointCount;
    UINT auiBook[ALGORITHM_MAPSIDE_SIZE] = {0};
    UINT i = 0;
    UINT j = 0;
    UINT u = 0;
    UINT v = 0;
    UINT uiInf = ALGORITHM_SP_INVAILD;
    UINT uiMin = 0;

    for (i = 0; i < uiPointCount; i++)
    {
        auiDis[i] = pstMap->aui[0][i];
    }

    for (i = 0; i < uiPointCount; i++)
    {
        auiBook[i] = 0;
    }
    auiBook[0] = 1;

    /* 核心算法 */
    for (i = 0; i < uiPointCount - 1; i++)
    {
        /* 找到离起始点剩余的最近的顶点 */
        uiMin = uiInf;
        for (j = 0; j < uiPointCount; j++)
        {
            if ((0 == auiBook[j]) && (auiDis[j] < uiMin))
            {
                uiMin = auiDis[j];
                u = j;
            }
        }

        auiBook[u] = 1;

        for (v = 0; v < uiPointCount; v++)
        {
            if (pstMap->aui[u][v] < uiInf)
            {
                if (auiDis[v] > auiDis[u] + pstMap->aui[u][v])
                {
                    auiDis[v] = auiDis[u] + pstMap->aui[u][v];
                }
            }
        }

    }


}

/* 主要思想：一轮一轮的进尝试进行所有边的松驰 */
VOID ALGORITHM_SP_BellmanFord(IN ALGORITHM_SP_SIDE_S *pstSide, OUT INT aiDis[ALGORITHM_MAPSIDE_SIZE])
{
    UINT k = 0;
    UINT i = 0;
    UINT uiPointCount = pstSide->uiPointCount;
    UINT uiSideCount = pstSide->uiSideCount;

    INT *u = pstSide->aiU;
    INT *v = pstSide->aiV;
    INT *w = pstSide->aiW;

    UINT uiCheck = 0;

    INT aiBakDis[ALGORITHM_MAPSIDE_SIZE];

    for (i = 0; i < uiPointCount; i++)
    {
        aiDis[i] = ALGORITHM_SP_INVAILD;
    }

    aiDis[0] = 0;

    for (k = 0; k < uiPointCount-1; k++)
    {
        /* 备份auiDis数组 */
        for (i = 0; i < uiPointCount; i++)
        {
            aiBakDis[i] = aiDis[i];
        }

        /* 进行一轮松驰 */
        for (i = 0; i < uiSideCount; i++)
        {
            if (aiDis[v[i]] > aiDis[u[i]] + w[i])
            {
                aiDis[v[i]] = aiDis[u[i]] + w[i];
            }
        }

        /* 松驰完后检测auiDis数组是否有更新 */
        uiCheck = 0;

        for (i = 0; i < uiPointCount; i++)
        {
            if (aiBakDis[i] != aiDis[i])
            {
                uiCheck = 1;
                break;
            }
        }

        /* 如果数组没有更新     ，提前退出循环           */
        if (0 == uiCheck)
        {
            break;
        }
    }

    /* 检测负权回路 */
    UINT uiFlag = 0;
    for (i = 0; i < uiSideCount; i++)
    {
        if (aiDis[v[i]] > aiDis[u[i]] + w[i])
        {
            uiFlag = 1;
        }
    }

    if (1 == uiFlag)
    {
        printf("此图含有负权回路\n");
    }

    return;
}

/* 主要思想：利用队列的特性对BellmanFord算法进行的优化 */
VOID ALGORITHM_SP_BellmanFord_QueueEx(IN ALGORITHM_SP_SIDE_S *pstSide, OUT INT aiDis[ALGORITHM_MAPSIDE_SIZE])
{
    UINT k = 0;
    UINT i = 0;
    UINT uiPointCount = pstSide->uiPointCount;
    UINT uiSideCount = pstSide->uiSideCount;

    INT *u = pstSide->aiU;
    INT *v = pstSide->aiV;
    INT *w = pstSide->aiW;

    INT aiFirst[ALGORITHM_POINT_COUNT];
    INT aiNext[ALGORITHM_SIDE_COUNT];

    /* 初始化aiDis数组 */
    for (i = 0; i < uiPointCount; i++)
    {
        aiDis[i] = ALGORITHM_SP_INVAILD;
    }
    aiDis[0] = 0;

    /* 用来标记是否已经入队列 */
    UINT auiBook[ALGORITHM_POINT_COUNT] = {0};

    for (i = 0; i < uiPointCount; i++)
    {
        aiFirst[i] = -1;
    }

    /* 构建数组式邻接表 */
    for (i = 0; i < uiSideCount; i++)
    {
        aiNext[i] = aiFirst[u[i]];
        aiFirst[u[i]] = i;
    }

    /* 初始化队列 */
    UINT auiQueue[ALGORITHM_POINT_COUNT * ALGORITHM_POINT_COUNT];

    UINT uiHead = 0;
    UINT uiTail = 0;

    auiQueue[uiTail] = 0;
    uiTail++;

    auiBook[0] = 1;

    /* 核心算法 */
    while(uiHead < uiTail)
    {
        /* 松驰一个点相连的所有边 */
        k = aiFirst[auiQueue[uiHead]];
        while(-1 != k)
        {
            if (aiDis[v[k]] > aiDis[u[k]] + w[k])
            {
                aiDis[v[k]] = aiDis[u[k]] + w[k];

                /* 入队列 */
                if (0 == auiBook[v[k]])
                {
                    auiQueue[uiTail] = v[k];
                    uiTail++;
                    auiBook[v[k]] = 1;
                }
            }



            k = aiNext[k];
        }

        /* 出队列 */
        auiBook[auiQueue[uiHead]] = 0;
        uiHead++;
    }

    return;
}

/* 二叉树binary tree 缩写为 BT */

VOID Algorithm_bt_Swap(INOUT UINT auiNum[ALGORITHM_BT_POINTCOUNT], UINT uiX, UINT uiY)
{
    UINT uiT = auiNum[uiX];
    auiNum[uiX] = auiNum[uiY];
    auiNum[uiY] = uiT;

    return;
}

VOID Algorithm_bt_SiftDown(INOUT UINT auiNum[ALGORITHM_BT_POINTCOUNT], IN UINT uiCount, IN UINT uiIndex)
{
    UINT uiTempIndex = 0;
    UINT uiFlag = 0; /* 标记是否需要继续循环 */

    /* 本结点有子结点并且不比子结点数值小 则循环 */
    while((uiIndex * 2 <= uiCount) && (0 == uiFlag))
    {
        /* 与左子结点比较 记录较小值的位置 */
        if (auiNum[uiIndex] > auiNum[uiIndex * 2])
        {
            uiTempIndex = uiIndex * 2;
        }
        else
        {
            uiTempIndex = uiIndex;
        }

        /* 再继续与右结点比较 记录更小值的位置 */
        if ((uiIndex * 2 + 1) <= uiCount)
        {
            if (auiNum[uiTempIndex] > auiNum[uiIndex * 2 + 1])
            {
                uiTempIndex = uiIndex * 2 + 1;
            }
        }

        /* 如果较小值位置与现在位置不同则交换，否则设置标志位为1表示不再循环 */
        if (uiTempIndex != uiIndex)
        {
            Algorithm_bt_Swap(auiNum, uiTempIndex, uiIndex);
            uiIndex = uiTempIndex;
        }
        else
        {
            uiFlag = 1;
        }
    }

    return;
}

VOID Algorithm_bt_HeapCreate(INOUT UINT auiNum[ALGORITHM_BT_POINTCOUNT], IN UINT uiCount)
{
    /* 完全二叉树特性 uiCount / 2必为最后一个非叶子结点 */
    for (UINT i = uiCount / 2; i >= 1; i--)
    {
        Algorithm_bt_SiftDown(auiNum, uiCount, i);
    }

    return;
}

UINT Algorithm_bt_DeleteMax(INOUT UINT auiNum[ALGORITHM_BT_POINTCOUNT], IN UINT *puiCount)
{
    /* 弹出顶值 并把最大值移到此处进行下滑处理 */
    UINT uiTempIndex = auiNum[1];
    auiNum[1] = auiNum[*puiCount];
    (*puiCount)--;
    Algorithm_bt_SiftDown(auiNum, *puiCount, 1);

    return uiTempIndex;
}

/* 主要思想：利用二叉树的最小堆特性，进行排序 */
VOID ALGORITHM_BT_HeapSort(INOUT UINT auiNum[ALGORITHM_BT_POINTCOUNT], IN UINT uiCount)
{
    UINT auiNumDeal[ALGORITHM_BT_POINTCOUNT + 1];
    memcpy(&auiNumDeal[1], auiNum, uiCount * sizeof(UINT));

    /* 创建堆 */
    Algorithm_bt_HeapCreate(auiNumDeal, uiCount);

    /* 从顶部弹出结点 */
    UINT uiTempCount = uiCount;
    for (UINT i = 1; i <= uiCount; i++)
    {
        auiNum[i - 1] = Algorithm_bt_DeleteMax(auiNumDeal, &uiTempCount);
    }

    return;
}

/* 并查集部分 */

UINT algorithm_bt_UnionFindSet_Getf(IN UINT auiNum[ALGORITHM_BT_POINTCOUNT], IN UINT uiIndex)
{
    /* 如果是最终根，直接返回 */
    if (auiNum[uiIndex] == uiIndex)
    {
        return uiIndex;
    }
    else
    { /* 递归更新路过的点的根为最终根 */
        auiNum[uiIndex] = algorithm_bt_UnionFindSet_Getf(auiNum, auiNum[uiIndex]);
    }

    return auiNum[uiIndex];
}

VOID algorithm_bt_UnionFindSet_Merge(IN UINT auiNum[ALGORITHM_BT_POINTCOUNT],
                                                    IN UINT uiIndex1, IN UINT uiIndex2)
{
    UINT uiTemp1;
    UINT uiTemp2;

    uiTemp1 = algorithm_bt_UnionFindSet_Getf(auiNum, uiIndex1);
    uiTemp2 = algorithm_bt_UnionFindSet_Getf(auiNum, uiIndex2);

    /* 有关系的两点最终根不同 则合并 */
    if (uiTemp1 != uiTemp2)
    {
        auiNum[uiTemp2] = uiTemp1;
    }

    return;
}

/* 并查集 union-find set */
/* 主要思想：根据线索关系，把各个点逐渐合并到各自的根上(生成森林) */
UINT ALGORITHM_BT_UnionFindSet(IN ALGORITHM_BT_RELATION_S *pstRelation,  OUT UINT auiNum[ALGORITHM_BT_POINTCOUNT])
{
    UINT uiSum = 0;

    /* 初始化数组 */
    for (UINT i = 0; i < pstRelation->uiPointCount; i++)
    {
        auiNum[i] = i;
    }

    /* 处理线索关系 */
    for (UINT i = 0; i < pstRelation->uiRelationCount; i++)
    {
        algorithm_bt_UnionFindSet_Merge(auiNum, pstRelation->auiRelation[i][0], pstRelation->auiRelation[i][1]);
    }

    /* 计算最终根数量 */
    for (UINT i = 0; i < pstRelation->uiPointCount; i++)
    {
        if (auiNum[i] == i)
        {
            uiSum++;
        }

    }

    return uiSum;
}