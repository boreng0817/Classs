#include <stdio.h>

typedef struct{
int xpos;
int ypos;
}Point;

void scale2x(Point* pp)
{
pp->xpos *= 2;
pp->ypos *= 2;
}

int main()
{
Point a[3];
int i = 0;
for(i = 0; i < 3; i++)
	scanf("%d %d", &(a+i)->xpos,&(a+i)->ypos);
for(i = 0; i < 3; i++)
	scale2x(a+i);
for(i = 0; i < 3; i++)
	printf("[%d] %d %d\n", i , (a+i)->xpos,(a+i)->ypos);

return 0;
}
