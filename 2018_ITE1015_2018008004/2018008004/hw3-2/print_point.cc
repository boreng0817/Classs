#include <stdio.h>

typedef struct
{
	int xpos;
	int ypos;
}Point;

Point getScale2xPoint(const Point* p){
Point new_p;
new_p.xpos = 2*p->xpos;
new_p.ypos = 2*p->ypos;

return new_p;
}

void swapPoint(Point& p1, Point& p2){
Point temp;
temp = p1;
p1 = p2;
p2 = temp;
}

int main(){
Point p1;
scanf("%d %d", &p1.xpos, &p1.ypos);
Point p2 = getScale2xPoint(&p1);
printf( "Calling getScale2xPoint()\n"
	"P1 : %d %d\n"
	"P2 : %d %d\n", p1.xpos , p1.ypos , p2.xpos , p2.ypos );
swapPoint(p1,p2);
printf( "Calling swapPoint()\n"
        "P1 : %d %d\n"
        "P2 : %d %d\n", p1.xpos , p1.ypos , p2.xpos , p2.ypos );
return 0;

}

