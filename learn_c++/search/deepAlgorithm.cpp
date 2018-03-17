
#include "head.h"
#include "deepAlgorithm.h"

deepAlgorithm :: deepAlgorithm()
{

}

deepAlgorithm :: ~deepAlgorithm()
{

}

int n, m, p, q, min = 99999999;
int a[50][50] = { {0, 0, 1, 0},
                  {0, 0, 0, 0},
				  {0, 0, 1, 0},
				  {0, 1, 0, 0},
				  {0, 0, 0, 1} };

int book[50][50] = {0};

void dfs(int x, int y, int step)
{
    int next[4][2] = { {0, 1},
                       {1, 0},
                       {0, -1},
                       {-1, 0}};
    int tx, ty, k;

    if (x == p && y == q)
    {
        if (step < min)
        {
            min = step;
        }
        return;
    }

    for (k = 0; k < 4; k++)
    {
        tx = x + next[k][0];
        ty = y + next[k][1];

        if (tx < 0 || tx >= n || ty < 0 || ty >= m)
        {
            continue;
        }

        if (a[tx][ty] == 0 && book[tx][ty] == 0)
        {
            book[tx][ty] = 1;
            dfs(tx, ty, step + 1);
            book[tx][ty] = 0;
        }
    }

    return;

}

void deepAlgorithm::search()
{
    int startx, starty;

    n = 4;
	m = 5;

	startx = 0;
	starty = 0;

	p = 3;
	q = 2;

	book[startx][starty] = 1;
	dfs(startx, starty, 0);

	std::cout << min << std::endl;
}



