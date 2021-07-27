#include <stdio.h>

typedef struct{
char name[10];
int age;
}Person;


int main()
{
Person a[3];
for( int i = 0; i<3 ; i++)
	scanf("%s %d", a[i].name, &a[i].age);
for( int i = 0; i<3 ; i++)
	printf("Name:%s, Age:%d\n", (a+i)->name, (a+i)->age);

return 0;
}

