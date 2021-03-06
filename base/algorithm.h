#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

/**********************************************************************************/
/*                             最短路径相关算法                                           */
/**********************************************************************************/


#define ALGORITHM_SP_INVAILD 0x0fffffff /* 表示不可达 */

#define ALGORITHM_MAPSIDE_SIZE 100

typedef struct ALGORITHM_SP_MAP
{
    UINT aui[ALGORITHM_MAPSIDE_SIZE][ALGORITHM_MAPSIDE_SIZE];
    UINT uiPointCount;
    UINT uiSideCount;
}ALGORITHM_SP_MAP_S;

VOID ALGORITHM_SP_MapScanf(INOUT ALGORITHM_SP_MAP_S *pstMap);

VOID ALGORITHM_SP_MapPrint(IN ALGORITHM_SP_MAP_S *pstMap);

VOID  ALGORITHM_SP_FloydWarshall(INOUT ALGORITHM_SP_MAP_S *pstMap);

/* 示例用法 */
/*
    ALGORITHM_SP_MAP_S stMap = {{{0, 2 ,6, 4},
                                 {ALGORITHM_SP_INVAILD, 0, 3, ALGORITHM_SP_INVAILD} ,
                                 {7, ALGORITHM_SP_INVAILD, 0, 1},
                                 {5, ALGORITHM_SP_INVAILD, 12, 0}},
                                4,
                                8
                               };

    ALGORITHM_SP_MapPrint(&stMap);

    ALGORITHM_SP_FloydWarshall(&stMap);

    ALGORITHM_SP_MapPrint(&stMap);

*/

VOID ALGORITHM_SP_Dijkstra(IN ALGORITHM_SP_MAP_S *pstMap, OUT UINT auiDis[ALGORITHM_MAPSIDE_SIZE]);

/* 示例用法 */
/*
    ALGORITHM_SP_MAP_S stMap = {{{0, 1, 12, ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD},
                                 {ALGORITHM_SP_INVAILD, 0, 9, 3, ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD},
                                 {ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, 0, ALGORITHM_SP_INVAILD, 5, ALGORITHM_SP_INVAILD},
                                 {ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, 4, 0, 13, 15},
                                 {ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, 0, 4},
                                 {ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, ALGORITHM_SP_INVAILD, 0}},
                                6,
                                9};

    UINT auiDis[ALGORITHM_MAPSIDE_SIZE];
    ALGORITHM_SP_Dijkstra(&stMap, auiDis);

    for (UINT i = 0; i < 6; i++)
    {
        printf("auiDis[%d] = %d\n", i, auiDis[i]);
    }

*/

#define ALGORITHM_SIDE_COUNT 100
#define ALGORITHM_POINT_COUNT 100

typedef struct ALGORITHM_SP_SIDE
{
    INT aiU[ALGORITHM_SIDE_COUNT];
    INT aiV[ALGORITHM_SIDE_COUNT];
    INT aiW[ALGORITHM_SIDE_COUNT];
    UINT uiPointCount;
    UINT uiSideCount;
}ALGORITHM_SP_SIDE_S;

VOID ALGORITHM_SP_BellmanFord(IN ALGORITHM_SP_SIDE_S *pstSide, OUT INT aiDis[ALGORITHM_MAPSIDE_SIZE]);

/* 示例用法 */

/*
    ALGORITHM_SP_SIDE_S stSide = {{1, 0, 0, 3, 2},
                                  {2, 1, 4, 4, 3},
                                  {2, -3, 5, 2, 3},
                                  5,
                                  5};
    UINT aiDis[ALGORITHM_MAPSIDE_SIZE];

    ALGORITHM_SP_BellmanFord(&stSide, aiDis);

    for (UINT i = 0; i < 5; i++)
    {
        printf("aiDis[%d] = %d\n", i, aiDis[i]);
    }
*/

VOID ALGORITHM_SP_BellmanFord_QueueEx(IN ALGORITHM_SP_SIDE_S *pstSide, OUT INT aiDis[ALGORITHM_MAPSIDE_SIZE]);

/* 示例用法 */
/*
    ALGORITHM_SP_SIDE_S stSide = {{0, 0, 1, 1, 2, 3, 4},
                                  {1, 4, 2, 4, 3, 4, 2},
                                  {2, 10, 3, 7, 4, 5, 6},
                                  5,
                                  7};
    UINT aiDis[ALGORITHM_MAPSIDE_SIZE];

    ALGORITHM_SP_BellmanFord_QueueEx(&stSide, aiDis);

    for (UINT i = 0; i < 5; i++)
    {
        printf("aiDis[%d] = %d\n", i, aiDis[i]);
    }
*/

/**********************************************************************************/
/*                             二叉树相关算法                                            */
/**********************************************************************************/
#define ALGORITHM_BT_POINTCOUNT 100

VOID ALGORITHM_BT_HeapSort(INOUT UINT auiNum[ALGORITHM_BT_POINTCOUNT], IN UINT uiCount);

/* 示例用法 */

/*

    UINT auiNum[ALGORITHM_BT_POINTCOUNT] = {99, 5, 36, 7, 22, 17, 46, 12, 2, 19, 25, 28, 1, 92};
    UINT uiCount = 14;
    ALGORITHM_BT_HeapSort(auiNum, uiCount);
    for (UINT i = 0; i < uiCount; i++)
    {
        printf("%d ", auiNum[i]);
    }
    printf("\n");

*/
#define ALGORITHM_BT_RELATIONCOUNT 100

typedef struct ALGORITHM_BT_RELATION
{
    UINT auiRelation[ALGORITHM_BT_RELATIONCOUNT][2];
    UINT uiRelationCount;
    UINT uiPointCount;
}ALGORITHM_BT_RELATION_S;

UINT ALGORITHM_BT_UnionFindSet(IN ALGORITHM_BT_RELATION_S *pstRelation, OUT UINT auiNum[ALGORITHM_BT_POINTCOUNT]);

/* 示例用法 */
/*
    ALGORITHM_BT_RELATION_S stRelation = {{{0, 1},
                                           {2, 3},
                                           {4, 1},
                                           {3, 5},
                                           {1, 5},
                                           {7, 6},
                                           {8, 6},
                                           {0, 5},
                                           {1, 3}},
                                          9,
                                          10};
    UINT auiNum[ALGORITHM_BT_POINTCOUNT] = {0};
    UINT uiSum = ALGORITHM_BT_UnionFindSet(&stRelation, auiNum);
    for (UINT i = 0; i < 10; i++)
    {
        printf("%u ", auiNum[i]);
    }
    printf("\nsum = %d\n", uiSum);
*/

#endif
