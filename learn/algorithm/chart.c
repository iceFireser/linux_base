#include <stdio.h>

#define W -1

int e[5][5] = {{0, 1, 1, W, 1},
               {1, 0, W, 1, W},
               {1, W, 0, W, 1},
               {W, 1, W ,0, W},
               {1, W, 1, W, 0}};

int book[5] = {0};

int sum = 0;
int n = 5;

void dfs(int cur)
{
    printf("%d ", cur);
    sum++;
    if (sum == n) return;

    for (int i = 0; i < n; i++)
    {
        if ((e[cur][i] == 1) && (book[i] == 0))
        {
            book[i] = 1;
            dfs(i);
        }
    }

    return;
}

int main()
{
    book[0] = 1;
    dfs(0);
    return 0;
}
