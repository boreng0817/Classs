#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Union ( int*, int, int );
int Find ( int*, int );
int numSet( int*, int );

FILE* in;
FILE* out;
int main() {
	int* edge;
	int* Set;
	int* maze;
	int i, j, n;
	int r1, r2;

	in = fopen( "input.txt", "r" );
	out = fopen( "output.txt", "w" );

	fscanf( in, "%d", &n );

	edge = (int*)malloc( sizeof(int) * n*(2*n-1) );
    maze = (int*)malloc( sizeof(int) * n*(2*n-1) );	
	Set = (int*)malloc( sizeof(int) * (n*n + 1) );

	for(i = 1 ; i <= n*n ; ++i)
		Set[i] = 0;
	for(i = 0 ; i < n*(2*n-1) ; ++i) {
		edge[i] = 1;
		maze[i] = 0;
	}

	srand( (unsigned int)time(NULL) );

	while(numSet(Set, n) > 1) {

		r1 = rand()%(2*n-1);
		if(r1%2==0)
			r2 = rand()%(n-1);
		else
			r2 = rand()%n;
		if(	edge[ n*r1 + r2 ] == 0 ) 
			continue;
		else
			edge[ n*r1 + r2 ] = 0;

		i = (r1/2)*n + r2 + 1;

		if(r1%2==0) 
			j = i + 1;

		else 
			j = i + n;
		

		i = Find(Set,i);
		j = Find(Set,j);

		if(i!=j)
			Union(Set, i, j);
		else 
			maze[ n*r1 + r2 ] = 1;
	}

	for( i = 0 ; i < n ; ++i )
		fprintf(out,"+-");
	fprintf(out,"+\n");

	for( i = 0 ; i < 2*n - 1 ; ++i) {
		if(i%2 == 0) {
			if( i == 0 ) 
				fprintf(out,"  " );
			else
				fprintf(out,"| ");

			for(j = 0; j < n - 1; ++j ) {
				if(maze[i*n + j] || edge[i*n + j])
					fprintf(out,"| ");
				else
					fprintf(out,"  ");
			}
			if(i == 2*n-2)
				fprintf(out,"\n");
			else
				fprintf(out,"|\n");
		}
		else {
			for( j = 0 ; j < n ; ++j ) {
				if( maze[i*n + j] || edge[i*n + j])
					fprintf(out,"+-");
				else
					fprintf(out,"+ ");
			}
			fprintf(out,"+\n");
		}
	}

	for( i = 0 ; i < n ; ++i ) 
		fprintf(out,"+-");
	fprintf(out,"+\n");

	fclose(in);
	fclose(out);

	return 0;
}

//disjoint set을 union하는 함수
void Union (int* S, int a, int b) {
	S[b] = a;
}

//int를 하나 받아 그 int가 포함되어있는 Set의 Root를 리턴하는 함수
int Find (int* S, int num) {
	for(;S[num] > 0; num = S[num])
		;
	return num;
}

//n^2의 element를 포함하는 전체집합에서의 subset의 숫자를 리턴하는 함수
int numSet( int* S, int n ) {
	int i, count = 0;
	for(i = 1 ; i <= n*n ; ++i)
		if(!S[i])
			count++;
	return count;
}
