#include <stdio.h>

void getSumDiff (int a, int b, int* pSum, int* pDiff){
*pSum = a + b;
*pDiff = a - b;
}

int main(void){
int pSum;
int pDiff;
int a,b;

scanf("%d %d", &a, &b);
getSumDiff(a,b,&pSum,&pDiff);

printf("Sum:%d\ndiff:%d\n", pSum, pDiff);

return 0;
}
