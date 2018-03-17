#include <stdio.h>

int a[10], book[10], n;

void dfs(int step)
{
    /* 判断边界 */
    if (step == (n + 1))
    {
        for (int i = 1; i <= n; i++)
        printf("%d  ", a[i]);

        printf("\r\n");
        return;
    }
    /* 尝试第一种可能 */
    for (int i = 1; i <= n; i++)
    {
        if (book[i] == 0)
        {
            a[step] = i;
            book[i] = 1;
            dfs(step + 1);/* 继续下一步 */
            book[i] = 0;
        }
    }
    /* 返回 */
    return;
}

int main()
{
    /* 1~9个盒子，n是盒子最后一个 */
    n = 9;
    /* 从第一个盒子开始尝试 */
    dfs(1);


    return 0;
}
