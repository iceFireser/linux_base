#include "head.h"
#include "breadthAlgorithm.h"

breadthAlgorithm::breadthAlgorithm()
{

}

breadthAlgorithm::~breadthAlgorithm()
{

}


struct node{
	int x;
	int y; /* 纵坐标 */
	int f; /*  */
	int s; /* 步数 */
};

void breadthAlgorithm::search()
{
	struct node que[2500];
	int next[4][2] = {  {0, 1},
				    	{1, 0},
						{0, -1},
						{-1, 0} };
	int head, tail;
	int i, j, k, n, m, startx, starty, p, q, tx, ty, flag;

    int a[50][50] = { {0, 0, 1, 0},
	                  {0, 0, 0, 0},
					  {0, 0, 1, 0},
					  {0, 1, 0, 0},
					  {0, 0, 0, 1} };

    int book[50][50] = {0};

	startx = 0;
	starty = 0;

	p = 3;
	q = 2;

	n = 4;
	m = 5;

	head = 0;
	tail = 0;
	que[tail].x = startx;
	que[tail].y = starty;
	que[tail].f = 0;
	que[tail].s = 0;
	tail++;
	book[startx][starty] = 1;

	flag = 0;

	while (head < tail)
	{
		for (k = 0; k < 4; k++)
		{
			tx = que[head].x + next[k][0];
			ty = que[head].y + next[k][1];

			if (tx < 0 || tx >= n || tx < 0 || ty >= m)
			{
				continue;
			}

			if (a[tx][ty] == 0 && book[tx][ty] == 0)
			{
				book[tx][ty] = 0;
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
	std::cout << que[tail - 1].s << std::endl;

	return;
}


