#include <stdio.h>
#include <stdlib.h>
#define INF 201234123
#define SWAP(A,B) (A)^=(B)^=(A)^=(B)

typedef struct Node* node;
typedef struct Node {
	int key;
	int weight;
} Node;

typedef struct Heap* heap;
typedef struct Heap {
	int cap;
	int size;
	node arr;
} Heap;

typedef struct Graph* graph;
typedef struct Graph {
	int size;
	int* node;
	int** matrix;
} Graph;

graph make_graph(graph G);
void insert_node(graph G, int sta, int des, int wei);
void fill_graph(graph G);
void dijkstra(graph G, int* pred, int s);
heap make_heap(int size);
void fill_heap(heap H, graph G, int s);
void decre_val(heap H, int nodei, int val);
int del_min(heap H);
int find_node(graph G, int node);
void print_path(graph G, int* pred, int s, int d);
void free_graph(graph G);
void free_heap(heap H);


FILE* in;
FILE* out;
int main() {
	int *pred, s, d;
	graph G;

	in = fopen( "input.txt", "r" );
	out = fopen( "output.txt", "w" );

	G = make_graph( G );
	pred = malloc( sizeof(int) * ( G -> size + 1 ) );
	fill_graph( G );

	fscanf( in, "%d%d", &s, &d );

	dijkstra( G, pred, s );
	print_path(G, pred, find_node(G, s), find_node(G, d));

	free_graph( G );
	free( pred );
	fclose( in );
	fclose( out );

	return 0;
}

//그래프를 만들어 노드배열을 채우는 함수.
graph make_graph( graph G ) {
	int size = 1, i, j, n;
	int queue[105] = {0,};
	char enter;
	G = malloc( sizeof( Graph ) );

	for(; enter != '\n' && enter != '\r' ;) {
		fscanf( in, "%d%c", &n, &enter );
		queue[size++] = n;
	}

	G -> size = size - 1;
	G -> node = malloc(sizeof(int) * (size + 1) );
	G -> matrix = malloc( sizeof(int*) * (size + 1) );
	for( i = 1 ; i <= size ; ++i ) {
		G -> node[i] = queue[i];
		G -> matrix[i] = malloc( sizeof(int) * (size + 1) );
	}

	for( i = 1 ; i <= size ; ++i )
		for( j = 1 ; j <= size ; ++j )
			G -> matrix[i][j] = INF;

	return G;
}

//엣지에 해당하는 가중치를 입력해주는 함수
void insert_node( graph G, int sta, int des, int wei ) {
	int i = find_node( G, sta ), j = find_node( G, des );
	G -> matrix[i][j] = wei;
}

//그래프의 가중치를 채우는 함수
void fill_graph( graph G ) {
	int s, d, w;
	char a, b, enter = ' ';

	while( enter != '\r' && enter != '\n' ) {
		fscanf(in, "%d%c%d%c%d%c", &s,&a,&d,&b,&w,&enter);
		insert_node( G, s, d, w );
	}
}

//최단경로를 찾는 함수. 다익스트라 알고리즘
void dijkstra( graph G, int* pred, int s ) {
	int node, idx = find_node( G, s ), i;
	int* d = malloc( sizeof(int) * (G -> size + 1) );
	
	for( i = 1 ; i <= G -> size ; ++i ) {
		if( i == idx ) {
			d[i] = 0;
			pred[i] = idx;
		}
		else {
			d[i] = INF;
			pred[i] = INF;
		}
	}
	

	heap H = make_heap( G -> size );
	fill_heap(H,G,idx);

	while( H -> size != 0 ) { // H is not empty
		node = del_min( H );
		for( i = 1 ; i <= G -> size ; ++i ) 
			if ( d[node] + G -> matrix[node][i] < d[i] ) {
				d[i] = d[node] + G -> matrix[node][i];
				pred[i] = node;
				decre_val( H, i, d[i] );
			}
	}
	free_heap( H );
}

//힙을 만드는 함수
heap make_heap( int size ) {
	heap H = malloc( sizeof( Heap ) );
	H -> cap = size;
	H -> arr = malloc( sizeof( Node ) * (size + 1) );
	H -> size = 0;

	return H;
}

//힙을 채우는 함수 O(n)
void fill_heap ( heap H, graph G, int s ) {
	int i, cnt;
	H -> arr[1].key = s;
	H -> arr[1].weight = 0;
	for( i = 2, cnt = 1 ; i <= G -> size ; ++i ) {
		if( cnt == s ) 
			cnt++;
		H -> arr[i].key = cnt++;
		H -> arr[i].weight = INF;
	}
	H -> size = G -> size;
}

//힙의 최소를 빼고, 힙을 알맞는 상태로 만든 뒤
//빼둔 최소를 리턴하는 함수
int del_min( heap H ) {
	int min_idx = H -> arr[1].key, i, c;


	SWAP(H -> arr[1].key, H -> arr[H -> size].key);
	SWAP(H -> arr[1].weight, H -> arr[H -> size].weight);

	H -> size--;

	for( i = 1 ; i * 2 <= H -> size ; i = c ) {
		c = i * 2;
		if( c != H -> size && H -> arr[c + 1].weight < H -> arr[c].weight )
			c++;
		if( H -> arr[i].weight > H -> arr[c].weight ) {
			SWAP(H -> arr[i].key, H -> arr[c].key);
			SWAP(H -> arr[i].weight, H -> arr[c].weight);
		}
		else
			break;
	}

	return min_idx;
}

//힙 배열에서 노드를 찾아 그 노드의 값을 줄이고
//알맞는 위치에 넣는 함수
void decre_val( heap H, int node, int val) {
	int i;
	for( i = 1 ; H -> arr[i].key != node ; ++i )
	   ;
	H -> arr[i].weight = val;
	for( ; i > 1 ; i /= 2 ) { 
		if( H -> arr[i].weight < H -> arr[i/2].weight ) {
			SWAP(H -> arr[i].weight,H -> arr[i/2].weight);
			SWAP(H -> arr[i].key, H -> arr[i/2].key);
		}
		else
			break;
	}
}

//해당 노드의 index를 찾는 함수
int find_node(graph G, int node) {
	int i;
	for( i = 1 ; node != G -> node[i] ; ++i )
		;
	return i;
}

//경로를 따라가 존재하면 출력하고, 존재하지 않으면 경로가 없다는
//메세지를 출력해주는 함수
void print_path(graph G, int* pred, int s, int d) {
	int size = 0, i;
	int arr[105] = {0,};

	if(pred[d] == INF) {
		fprintf(out, "no path\n");
		return;
	}

	for(;;++size) {
		arr[size] = d;
		d = pred[d];
		if( d == s ) {
			arr[++size] = d;
			break;
		}
	}
	for(i = size ; i >= 0 ; --i) 
		fprintf(out, "%d ", G -> node[arr[i]]);
	fprintf(out, "\n");
}

//다 사용한 그래프를 삭제하는 함수
void free_graph( graph G ) {
	int i;

	for( i = 1 ; i <= G -> size ; ++i )
		free( G -> matrix[i] );
	free( G -> matrix );
	free( G -> node );
	free( G );
}

//다 사용한 힙을 삭제하는 함수
void free_heap( heap H ) {
	free( H -> arr );
	free( H );
}
