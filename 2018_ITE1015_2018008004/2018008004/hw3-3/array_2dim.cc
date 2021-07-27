#include <stdio.h>

int main(){

int a[3][2] = {0,};
int i;
for(i = 0 ; i<6 ; i++)
	scanf("%d", a[0]+i);
for(i = 0 ; i<3 ; i++)
	printf("%d %d\n", a[i][0] , a[i][1]);
for(i = 0 ; i<2 ; i++)
	printf("%d 번째 열의 숫자의 합: %d\n",i+1, a[0][i] + a[1][i] + a[2][i]);

return 0;
}
	
