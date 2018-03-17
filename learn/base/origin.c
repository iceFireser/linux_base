#include "head.h"


#define FILE_PATH "/home/h/demo/temp/open.txt"

INT main_(INT iArgc, CHAR *szArgv[])
{
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

    return -1;
}


