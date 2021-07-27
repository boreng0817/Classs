#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define is_empty(A) (A)->size==0

typedef struct Graph* graph;
typedef struct Graph {
	int size;
	int* node;
	int** matrix;
} Graph;

typedef struct Queue* queue;
typedef struct Queue {
	int* key;
	int front;
	int rear;
	int size;
	int max_queue_size;
} Queue;

graph init_graph( graph G );
void insert_edge( graph G, int a, int b );
void topsort( graph G );
queue init_queue( int size );
void del_queue( queue Q );
void enq( queue Q, int X );
int deq( queue Q );
int front( char* str );
int back( char* str );
void check_indeg( graph G, int* indeg);
void fill_graph( graph G );
void print( graph G );
void del_graph( graph G );

FILE* in;
FILE* out;
int main() {
	graph G;
	in = fopen( "input.txt", "r" );
	out = fopen( "output.txt", "w" );

	G = init_graph( G );
	fill_graph( G );
	print( G );
	topsort( G );
	del_graph( G );

	fclose( in );
	fclose( out );
	return 0;
}

//알맞는 노드를 배정해 node의 개수가 size인 
//graph를 만들어 리턴하는 함수
graph init_graph( graph G ) {
	G = malloc( sizeof ( Graph ) );
	queue Q = init_queue( 600 );
	int count = 0, n, i;
	char enter;

	for(; enter != '\n' && enter != '\r' ; count++ ) {
		fscanf( in, "%d%c", &n, &enter );
		enq( Q, n );
	}
	G -> size = count;
	G -> node = malloc( sizeof(int) * count );
	G -> matrix = malloc( sizeof(int*) * count );
	for ( i = 0 ; i < count ; ++i ) {
		G -> matrix[i] = malloc( sizeof(int) * count );
		G -> node[i] = deq(Q);
	}

	del_queue( Q );
	return G;
}

//graph에 edgd = (a,b)를 넣는 함수
void insert_edge( graph G, int a, int b ) {
	int i = 0, j = 0;

	for(; i < G -> size && G -> node[i] != a ; ++i )
		;
	for(; j < G -> size && G -> node[j] != b ; ++j )
		;
	G -> matrix[i][j] = 1;
}

//graph에 대한 topological 소트를 하는 함수
void topsort( graph G ) {
	queue Q = init_queue( 2 * (G -> size) );
	queue topo = init_queue( 2 * (G -> size) );
	int* indeg = malloc( sizeof(int) * G -> size );
	int i, node;

	for( i = 0 ; i < G -> size ; ++i)
		indeg[i] = 0;

	check_indeg(G, indeg);

	for(i = 0 ; i < G -> size ; ++i) 
		if(!indeg[i])
			enq( Q, i );
	
	while(!is_empty(Q)) {
		node = deq(Q);
		enq(topo, node);
		for( i = 0 ; i < G -> size ; ++i )
			if(G->matrix[node][i])
				if(!(--indeg[i]))
					enq( Q, i );
	}

	fprintf(out,"TopSort Result : ");
	for(i = 0 ; i < G -> size ; ++i)
		fprintf(out, "%d ", G -> node[deq(topo)]);
	fprintf(out, "\n");

	free( indeg );
	del_queue( Q );
	del_queue( topo );
}

//각 노드의 indegree를 계산하는 함수
void check_indeg ( graph G, int* indeg ) {
	int i, j; 
	for( i = 0 ; i < G -> size ; ++i ) 
		for( j = 0 ; j < G -> size ; ++j ) 
			indeg[i] += (G -> matrix)[j][i];
}

//max_size가 size인 큐를 만들어 리턴하는 함수
queue init_queue ( int size ) {
	queue Q = malloc( sizeof ( Queue ) );
	Q -> key = malloc( sizeof( int )*size );
	Q -> front = 1;
	Q -> rear = 0;
	Q -> size = 0;
	Q -> max_queue_size = size;
	return Q;
}

//다 사용한 큐를 삭제하는 함수
void del_queue ( queue Q ) {
	free( Q -> key );
	free( Q );
}

//큐에 int를 넣는 함수
void enq ( queue Q, int X ) {
	(Q -> size)++;
	Q -> rear = (Q -> rear + 1) % (Q -> max_queue_size);
	Q -> key[Q -> rear] = X;
}

//큐에서 int를 빼는 함수
int deq ( queue Q ) {
	if(is_empty(Q))
		return -1;
	int _return;
	(Q -> size)--;
	_return = Q -> key[ ((Q -> front)++)];
	Q -> front = (Q -> front) % (Q -> max_queue_size);

	return _return;
}

//'-'로 나눠진 string의 앞부분을 숫자로 만들어 리턴하는 함수
int front( char* str ) {
	int count = 0, i, size = strlen(str), _return = 0, ten = 1;
	queue Q = init_queue( size ); 

	for( i = 0 ; str[i] != '-' ; ++i,++count,ten *= 10 ) 
		enq( Q, str[i] );
	for( i = 0 ; i < count ; ++i, ten /= 10 ) 
		_return += (deq(Q) - '0')*ten;
	del_queue( Q );
	
	return _return/10;
}

//앞 함수와 같이, 뒷부분을 숫자로 리턴하는 함수
int back( char* str ) {
	int count = 0, i, size = strlen(str), _return = 0, ten = 1;
	queue Q = init_queue( size ); 
	for( i = 0 ; str[i] != '-' ; ++i)
		;
	for( ++i ; i < size ; ++i,++count,ten *= 10 )
		enq( Q, str[i] );
	for( i = 0 ; i < count ; ++i, ten /= 10 )
		_return += (deq(Q) - '0')*ten;
	del_queue( Q );

	return _return/10;
}

//그래프의 edge를 채우는 함수
void fill_graph( graph G ) {
	char str[100];

	while( fscanf(in, "%s", str) == 1 ) 
		insert_edge( G, front(str), back(str) );
}

//그래프를 출력하는 함수
void print( graph G ) {
	int i, j;

	fprintf(out, "  ");
	for( i = 0 ; i < G -> size ; ++i ) 
		fprintf(out, "%d ", G -> node[i]);
	fprintf(out, "\n");

	for( i = 0 ; i < G -> size ; ++i ) {
		fprintf(out, "%d ", G -> node[i]);
		for( j = 0 ; j < G -> size ; ++j ) 
			fprintf(out, "%d ", G -> matrix[i][j]);
		fprintf(out, "\n");
	}

	fprintf(out, "\n\n");
}

//다 사용한 그래프를 삭제하는 함수
void del_graph( graph G ) {
	int i;
	free( G -> node );
	for(i = 0 ; i < G -> size ; ++i)
		free( G -> matrix[i] );
	free( G -> matrix );
	free( G );
}

