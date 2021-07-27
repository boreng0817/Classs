#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

//----------STRUCTURES----------//
typedef struct threaded_tree *threaded_ptr;
typedef struct threaded_tree {
	short int left_thread;
	threaded_ptr left_child;
	char data;
	threaded_ptr right_child;
	short int right_thread;
}threaded_tree;

typedef threaded_ptr Tree;

typedef struct queue *Queue;
typedef struct queue {
	int rear;
	int front;
	int size;
	Tree* array;
}queue;
//----------STRUCTURES----------//

//----------FUNCTIONS-----------//
Tree init ( Tree T );
Tree insucc ( Tree T );
void insert ( char _data, Tree T );
void makeThread ( Tree T );
void fillQueue ( Queue Q, Tree T );
void tinorder ( Tree T );

void enq ( Queue Q, Tree node );
Tree deq ( Queue Q );
//----------FUNCTIONS-----------//


FILE* out;

int main ( int argc, const char * argv[] ) {
	FILE* in;
	int num, i;
	char ch;
	Tree head; 
	head = init ( head );
	in = fopen ( argv[1], "r" );
	out = fopen ( "output.txt", "w" );
	fscanf ( in, "%d", &num );

	for ( i = 0 ; i < num ; ++i ) {
		fscanf ( in, "%c", &ch );
		
		if( ch == '\n' || ch == '\t' || ch == ' ' ) {
			i--;
			continue;
		}

		insert ( ch, head );

	}

	makeThread ( head );
	tinorder ( head );

	fclose ( out );
	fclose ( in );
	return 0;

}

//Tree를 하나 받아 초기화 하고 리턴하는 함수
Tree init ( Tree T ) {
	T = malloc ( sizeof ( struct threaded_tree ) );
	T -> left_child = NULL;
	T -> right_child = T;
	T -> left_thread = FALSE;
	T -> right_thread = FALSE;
	T -> data = '-';
	return T;
}

//Threaded tree에 노드를 하나 받아 다음 inorder traversal 순서를 찾아 리턴하는 함수
Tree insucc ( Tree T ) {
	Tree temp;
	temp = T -> right_child;
	if ( !( T -> right_thread ) )
		while  ( !( temp -> left_thread ) )
			temp = temp -> left_child;
	return temp;
}

//char하나를 받아 노드를 만들어 넣어준 뒤에 트리에 complete한 순서대로 insert하는 함수
void insert ( char _data, Tree T ) {
	Queue Q = malloc ( sizeof ( struct queue ) );
	int i;
	Tree node, temp;

	Q -> size = 100;
	Q -> front = 1;
	Q -> rear = 0;
	Q -> array = malloc( sizeof ( struct threaded_tree ) * ( Q -> size ) );

	enq ( Q, T );
	
	node = (Tree) malloc ( sizeof ( struct threaded_tree ) );
	node -> data = _data;
	node -> left_child = NULL;
	node -> right_child = NULL;
	node -> left_thread = TRUE;
	node -> right_thread = TRUE;

	for (;;) {
		temp = deq( Q );

		if ( temp -> left_child == NULL ) {
			//printf( "%c -> %c\n", temp -> data, node -> data); // 연결 확인
			temp -> left_thread = FALSE;
			temp -> left_child = node;
			break;
		}

		else if ( temp -> right_child == NULL ) {
			//printf( "%c -> %c\n", temp -> data, node -> data); // 연결 확인
			temp -> right_thread = FALSE;
			temp -> right_child = node;
			break;
		}

		else {
			enq ( Q, temp -> left_child );
			enq ( Q, temp -> right_child );
		}
	}

	free ( Q -> array );
	free ( Q );
}  

//binary tree를 하나 받아 Threaded tree로 만들어주는 함수
void makeThread ( Tree T ) {
	Tree node1, node2, node3;
	Queue Q;
	Q = malloc ( sizeof ( struct queue ) );
	Q -> size = 100;
	Q -> rear - 0;
	Q -> front = 1;
	Q -> array = malloc ( sizeof ( struct threaded_tree ) * ( Q -> size ) );

	enq ( Q, T );
	fillQueue ( Q, T -> left_child );
	enq ( Q, T );

	node1 = deq ( Q );
	node2 = deq ( Q );
	node3 = deq ( Q );

	for ( ;; ) {
		if ( node2 -> left_thread )
			node2 -> left_child = node1;

		if ( node2 -> right_thread )
			node2 -> right_child = node3;
		
		if ( node3 -> right_child == T )
			break;
		node1 = node2;
		node2 = node3;
		node3 = deq ( Q );
	}
}

//Threaded가 안되어있는 binary tree를 하나 받아 inorder순서대로 큐에 집어 넣는 함수
void fillQueue ( Queue Q, Tree T ) {
	if( T ) {
		fillQueue ( Q, T -> left_child );
		enq ( Q, T );
		fillQueue ( Q, T -> right_child );
	}
}

//Threaded tree를 인자로 받아 Threaded tree를 inorder하는 함수
void tinorder ( Tree T ) {
	Tree temp = T;
	for(;;) {
		temp = insucc ( temp );
		if( temp == T ) break;
		fprintf ( out, "%c ", temp -> data );
	}
}

//큐와 노드 하나를 받아 큐 맨 뒤에 노드를 넣는 함수
void enq ( Queue Q, Tree node ) {
	Q -> rear += 1;
	Q -> array[ (Q -> rear) % ( Q -> size ) ] = node;
}

//큐를 인자로 받아 큐의 첫번째 원소를 지우며 리턴하는 함수
Tree deq ( Queue Q ) {
	Tree temp = Q -> array[ Q -> front ];
	Q -> front = ( Q -> front + 1 ) % ( Q -> size );
	return temp;
}

