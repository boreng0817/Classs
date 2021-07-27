#include <stdio.h>

typedef struct
{
 double x;
 double y;
}Point;

typedef struct
{
 char name[7];
 Point points[3];
}Person;

Point max(Person per){
double max = 0;
Point new_p;
double a[3];
for( int i = 0 ; i < 3 ; i++ )
	a[i] = (per.points[i].x)*(per.points[i].x) + (per.points[i].y)*(per.points[i].y);
max = (a[0]>a[1]?a[0]:a[1]) > a[2] ? (a[0]>a[1]?a[0]:a[1]) : a[2];
for( int i = 0 ; i < 3 ; i++ ) {
	if(max == a[i])	     {
		new_p = per.points[i];
		return new_p; }
} }

int main(){
Person per[3];
for( int i = 0 ; i < 3 ; i++){
	scanf("%s", per[i].name);
	for( int j = 0 ; j < 3 ; j++)
		scanf("%lf %lf", &per[i].points[j].x, &per[i].points[j].y); }
for( int i = 0 ; i < 3 ; i++)
	printf("%s (%f, %f)\n", per[i].name , max(per[i]).x , max(per[i]).y);
return 0;
}
