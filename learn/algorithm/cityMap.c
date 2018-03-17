#include <stdio.h>

#define W -1

int e[5][5] = {{0, 2, W, W, 10},
               {W, 0, 3, W, 7},
               {4, W, 0, 4, W},
               {W, W, W, 0, 5},
               {W, W, 3, W, 0}};

int book[5] = {0};

int min = 0x7fffffff;

void dfs(int cur, int dis)
{
    if (dis > min)return;
    if (cur == n)
    {
        if (dis < min) min = dis;
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        if ((book[i] == 0) && (e[cur][i] != W))
        {
            book[i] = 1;
            dfs(i, dis + e[cur][i]);
            book[i] = 0;
        }
    }

    return;
}

int main()
{
    book[0] = 1;
    dfs(0, 0);

    return 0;
}
