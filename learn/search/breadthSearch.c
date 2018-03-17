#include <stdio.h>

struct node
{
    int x;
    int y;
    int f;
    int s; /* 步数 */
};



int a[5][4] = { {0,0,1,0},
                {0,0,0,0},
				{0,0,1,0},
				{0,1,0,0},
				{0,0,0,1}};


int main()
{
    struct node que[20];
    int book[5][4] = {0};
    int next[4][2] = {{0, 1},
                      {1, 0},
                      {0, -1},
                      {-1, 0}};

    int head, tail;
    int i, j, k, n, m, p, q, tx, ty, flag;

    n = 5;
    m = 4;

    p = 3;
    q = 2;

    head = 0;
    tail = 0;

    que[tail].x = 0;
    que[tail].y = 0;
    que[tail].f = 0;
    que[tail].s = 0;
    tail++;
    book[0][0] = 1; /* 标记已走过的点 */

    flag = 0; /* 用来标记是否到达标记点 */

    while (head < tail)
    {
        for (k = 0; k < 4; k++)
        {
            tx = que[head].x + next[k][0];
            ty = que[head].y + next[k][1];

            if (tx<0 || tx>=n || ty<0 || ty>=m)
            continue;

            if (a[tx][ty] == 0 && book[tx][ty] == 0)
            {
                book[tx][ty] = 1;
                que[tail].x = tx;
                que[tail].y = ty;
                que[tail].f = head;
                que[tail].s = que[head].s + 1;
                tail++;
            }

            if (tx == p && ty == q)
            {
                flag = 1;
                break;
            }

        }

        if (flag == 1)
        {
            break;
        }

        head++;
    }

    printf("%d\r\n", que[tail - 1].s);

    return 0;
}


