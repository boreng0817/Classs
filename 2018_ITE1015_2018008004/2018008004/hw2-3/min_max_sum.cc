#include <stdio.h>

int main() {
int a[5] = {0,};
int max, min, sum = 0;
for( int i = 0 ; i < 5 ; i++)
	scanf("%d", &a[i]);
max = a[0];
min = a[0];

for( int i = 0 ; i < 5 ; i++){
sum += a[i];
if(a[i] > max)
	max = a[i];
if(a[i] < min)
	min = a[i];}

printf("min: %d\n"
	"max: %d\n"
	"sum: %d\n", min, max, sum);

return 0;
}
