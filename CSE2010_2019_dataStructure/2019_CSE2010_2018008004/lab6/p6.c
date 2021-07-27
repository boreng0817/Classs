#include <stdio.h>
#include <stdlib.h>


struct AVLNode;
typedef struct AVLNode *Pos;
typedef struct AVLNode *Tree;

struct AVLNode
{
	int value;
	Tree L;
	Tree R;
	int height;
}AVLNode;

int height ( Pos P );
int MAX( int a, int b );
Tree insert ( int X, Tree T );
Pos LL( Pos K );
Pos RR( Pos K );
Pos LR( Pos K );
Pos RL( Pos K );
void inorder ( Tree T );

FILE* in;
FILE* out;
int fail;
int main() {
	int num;
	Tree T = NULL;
	in = fopen ( "input.txt", "r" );
	out = fopen ( "output.txt", "w" );

	while ( fscanf( in, "%d", &num ) == 1 ) {
		T = insert ( num, T );

		if( !fail ) 
			inorder ( T );
		else
			fprintf ( out, "%d already in the tree!", num );		

		fprintf ( out, "\n" );
	}

	fclose ( out );
	fclose ( in );

	return 0;
}

//노드를 받아 노드의 높이를 리턴하는 함수
int height ( Pos P ) {
	if ( P == NULL )
		return -1;
	else
	   return P -> height;
}

//int 2개를 받아 더 큰 숫자를 리턴하는 함수
int MAX( int a, int b ) {
	return a > b ? a : b;
}

//노드를 트리에 인서트하는 함수.
Tree insert ( int X, Tree T ) {
	if ( T == NULL ) {
		T = malloc ( sizeof( AVLNode ) );
		T->L = T->R = NULL;
		T -> value = X;
		T -> height = 0;
		fail = 0;
	}

	else if( X > T -> value ) {
		T -> R = insert ( X, T -> R );
		if( height ( T -> R ) - height ( T -> L ) == 2 ) {
			if( X > T -> R -> value )
				T = RR(T);
			else
				T = RL(T);
		}
	}
	else if ( X < T -> value ) {
		T -> L = insert ( X, T -> L );
		if ( height ( T -> L ) - height ( T -> R ) == 2 ) {
			if ( X < T -> L -> value )
				T = LL(T);
			else
				T = LR(T);
		}
	}

	else
		fail = 1;
	
	T -> height = MAX(height(T->L), height(T->R)) + 1;	
	return T;	
}

//single rotation when node inserted to left child of left child
Pos LL ( Pos K ) {
	Pos temp = K -> L;
	K -> L = temp -> R;
	temp -> R = K;

	K -> height = MAX(height(K->L),height(K->R)) + 1;
	temp -> height = MAX(height(temp->L), K->height) + 1;
	return temp;
}

//single rotation when node inserted to right child of right child
Pos RR ( Pos K ) {
	Pos temp = K -> R;
	K -> R = temp -> L;
	temp -> L = K;

	K -> height = MAX ( height(K->L), height(K->R) ) + 1;
	temp -> height = MAX ( height(temp->R), K->height ) + 1;
	return temp;
}

//double rotation when node inserted to right child of left child
Pos LR ( Pos K ){
	K -> L = RR ( K -> L );
	return LL ( K );
}

//double rotation when node inserted to left child of right child
Pos RL ( Pos K ) {
	K -> R = LL ( K -> R );
	return RR ( K );
}

//print into file while doing inorder traversal.
void inorder ( Tree T ) {
	if( T ) {
		inorder ( T -> L );
		fprintf ( out, "%d(%d) ", T -> value, height ( T ) );
		inorder ( T -> R );
	}
}

