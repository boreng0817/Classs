#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int capa;
	int size;
	int* arr;
} Heap;

typedef Heap* heap;

void insert( int element, heap h );
void find( int element, heap h );
void print( heap h );
int is_in( int X, heap h );
heap init( heap h, int _size );

FILE* in;
FILE* out;

int main() {
	int n, i;
	char op[10];
	heap h;

	in = fopen( "input.txt", "r" );
	out = fopen( "output.txt", "w" );

	fscanf( in, "%d", &n );

	h = init( h, n );

	while( fscanf( in, "%s", op ) == 1 ) {
		if( !strcmp(op, "i") ) {
			fscanf( in, "%d", &n );
			insert( n, h );
		}
		else if( !strcmp(op, "f") ) {
			fscanf( in, "%d", &n );
			find( n, h );
		}
		else if( !strcmp(op, "p") )
			print( h );
	}

	fclose( in );
	fclose( out );
	free( h->arr );
	free( h );

	return 0;
}
//heap과 capa를 받아 초기화 해주는 함수
heap init( heap h , int _size ) {
	h = (heap)malloc( sizeof(Heap) );
	h -> size = 0;
	h -> capa = _size;
	h -> arr = (int*)malloc( sizeof(int) * (h -> capa + 1) );

	return h;
}
//heap을 하나 받아 heap에 있는 원소를들 출력하는 함수
void print ( heap h ) {
	int i;
	for( i = 1 ; i <= h -> size ; ++i ) 
		fprintf( out, "%d ", h -> arr[i] );
	fprintf( out, "\n" );
}
//int를 하나 받아 heap에 존재하는 상황과 존재하지 않는 상황에
//적절하게 메세지를 output.txt에 출력하는 함수
void find ( int element, heap h ) {
	int i;

	for( i = 1 ; i <= h -> size && h -> arr[i] != element; ++i )
		;

	if( i == h -> size + 1 )
		fprintf( out, "%d is not in the heap.\n", element );
	else
		fprintf( out, "%d is in the heap.\n", element );
}
//int가 heap에 있는지 확인하는 함수. 있다면 0을, 없으면 1을 리턴
int is_in ( int X, heap h ) {
	int i;
	for( i = 1 ; i <= h -> size && h -> arr[i] != X ; ++i)
		;
	return (h -> size == 0) || i == (h -> size + 1) ;
}
//heap과 insert 될 int 하나를 받아 heap에 넣는 함수. 이미 존재하거나, heap이 꽉 차면
//에러 메세지를 출력함.
void insert ( int element, heap h ) {
	int i;
	if( !is_in( element, h ) ) {
		fprintf( out, "%d is already in the heap.\n", element );
		return;
	}

	else if( h -> size == h -> capa ) {
		fprintf( out, "heap is full\n" );
		return;
	} 
	fprintf( out, "insert %d\n", element );

	h -> arr[++(h -> size)] = element;

	for( i = h -> size ; i > 1 && h -> arr[i/2] < h -> arr[i] ; i /= 2 ) {
		h -> arr[i] = h -> arr[i/2];
		h -> arr[i/2] = element;
	}
}
