#include <stdio.h>

void quickSort(int a[], int left, int right)
{
    int baseNum = a[left];
    int cLeft = left;
    int cRight = right;

    if (left > right)return;

    while (cLeft != cRight)
    {
        while(a[cRight] >= baseNum && cLeft < cRight)cRight--;
        while(a[cLeft] <= baseNum && cLeft < cRight)cLeft++;
        if (cLeft < cRight)
        {
            int tempNum = a[cRight];
            a[cRight] = a[cLeft];
            a[cLeft] = tempNum;
        }
    }


    a[left] = a[cLeft];
    a[cLeft] = baseNum;

    quickSort(a, left, cLeft - 1);
    quickSort(a, cLeft + 1, right);

    return;
}

void printArray(int a[], int max)
{
    for(int i = 0; i < max; i++)
    {
        printf("%d ", a[i]);
    }
    printf("\r\n");
}

int main()
{
    int a[10] = {2 ,4, 0, 5, 9, 6, 8, 7, 5, 1};
    quickSort(a, 0, 9);
    printArray(a, 10);
}
