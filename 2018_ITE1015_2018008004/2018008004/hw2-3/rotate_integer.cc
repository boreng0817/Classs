#include <stdio.h>

void rotateLeft(int* pa, int* pb, int* pc){
int temp;
temp = *pa;
*pa = *pb;
*pb = *pc;
*pc = temp;
}

void rotateRight(int* pa, int* pb, int* pc){
int temp;
temp = *pc;
*pc = *pb;
*pb = *pa;
*pa = temp;
}

int main(){
int a = 10, b = 20, c = 30 , choice;
printf("%d:%d:%d\n", a, b, c);
while(1){
	scanf("%d", &choice);
if(choice == 1){
	rotateLeft(&a,&b,&c);
	printf("%d:%d:%d\n", a, b, c);}
else if(choice == 2){
	rotateRight(&a,&b,&c);
	printf("%d:%d:%d\n", a, b, c);}
else
	break;}

return 0;
}
