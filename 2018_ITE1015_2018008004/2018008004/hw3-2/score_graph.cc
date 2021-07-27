#include <stdio.h>

typedef struct
{
char name[7];
int score;
}Person;

void printScoreStars(Person* persons, int len){
	//char a[]="******************************************";
	//for( int i = 0 ; i<3 ; i++){
	//a[(persons+i)->score/5] = '\0';
	//printf( "%s %s\n",(persons+i)->name, a);
	//a[(persons+i)->score/5] = '*'; } 이렇게 하면 점수 까이려나요?
int stars;
for(int i = 0 ; i<len ; i++){
stars = (persons+i)->score/5;

printf("%s ",(persons+i)->name);
for(int j = 0 ; j<stars; j++)
	printf("*");
printf("\n");
}}

int main(){
Person a[3];
int i = 0;

for( i = 0; i<3 ; i++)
	scanf("%s %d",(a+i)->name,&(a+i)->score);
printScoreStars(a,3);

return 0;

}
