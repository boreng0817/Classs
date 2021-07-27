#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------STRUCTURE----------
typedef struct TreeNode *treeptr;
typedef treeptr Tree;
typedef struct TreeNode{
	int value;
	treeptr left, right;
}TreeNode;
//=============================

//----------FUNCTION-----------
Tree init ( Tree T );
Tree insert ( Tree T , int X );
Tree delete ( Tree T , int X );
Tree find ( Tree T , int X );
Tree findMax ( Tree T );
void inorder ( Tree T );
void preorder ( Tree T );
void postorder ( Tree T );
//=============================


FILE* in;
FILE* out;
int main () {
	Tree T;
	int num;
	char op[10];

	in = fopen ( "input.txt" , "r" );
	out = fopen ( "output.txt", "w" );
	T = init ( T );

	while ( fscanf ( in, "%s", op) == 1) {
	
		if( !strcmp( op, "pi" ) ) {
			fprintf ( out, "pi - " );
			inorder ( T -> left );
			fprintf ( out, "\n" );
		}
		
		else if( !strcmp ( op, "pr") ) {
			fprintf ( out, "pr - " );
			preorder ( T -> left );
			fprintf ( out, "\n" );
		}

		else if( !strcmp ( op, "po") ) {
			fprintf ( out, "po - " );
			postorder ( T -> left );
			fprintf ( out, "\n" );
		}

		else {
			
			fscanf ( in, "%d", &num );

			if ( !strcmp ( op, "i" ) )
				T = insert ( T , num );
			
			else if ( ! strcmp ( op, "d" ) )
				T = delete ( T , num );

			else if ( ! strcmp ( op, "f" ) ) {
				Tree temp = find ( T -> left , num );
				if(temp == NULL)
					fprintf ( out, "%d is not in the tree.\n", num );
				else
					fprintf ( out, "%d is in the tree.\n", num);
			}
		}
	}

	fclose ( in );
	fclose ( out );

	return 0;
}

//트리를 인자로 받아 트리를 정의 해주는 함수.
Tree init ( Tree T ) {
	T = malloc ( sizeof ( struct TreeNode ) );
	T -> value = 2109876543;
	T -> left = NULL;
	T -> right = T;

	return T;
}

//BST와 int 하나를 받아 적당한 자리에 insert하는 함수. 중복이 있을시 오류 메세지를 출력.
Tree insert ( Tree T, int X ) {
	if ( T == NULL ) {
		T = malloc ( sizeof ( TreeNode ) );
		T -> value = X;
		T -> left = T -> right = NULL;
	}

	else if ( X < T -> value )
		T -> left = insert ( T -> left, X );
	else if ( X > T -> value )
		T -> right = insert ( T -> right, X );
	else
		fprintf ( out, "%d already exists.\n", X );
	
	return T;
}

//BST와 int 하나를 받아 int가 있다면 삭제하고, 아니면 오류 메세지를 출력.
Tree delete ( Tree T, int X ) {
	Tree temp;

	if ( T == NULL )
		fprintf(out, "Deletion failed. %d does not exist.\n", X);
	
	else if ( X < T -> value )
		T -> left = delete ( T -> left, X );
	
	else if ( X > T -> value )
		T -> right = delete ( T -> right, X );
	
	else if ( T -> left && T -> right ) {
		temp = findMax( T -> left );
		T -> value = temp -> value;
		T -> left = delete ( T -> left, T -> value );
	}

	else {
		temp = T;
		if ( T -> left == NULL )
			T = T -> right;
		else if ( T -> right == NULL )
			T = T -> left;
		free ( temp );
	}
	return T;
}

//BST와 int 하나를 받아 트리 안에 int가 존재하면 노드를 리턴, 없다면 null을 리턴하는 함수
Tree find ( Tree T, int X ) {
	if ( T == NULL )
		return NULL;
	if ( X < T -> value )
		return find ( T -> left, X );
	else if ( X > T -> value )
		return find ( T -> right, X );
	else
		return T;
}

//BST의 max value를 찾아 리턴하는 함수. BST가 비어있다면 NULL을 리턴
Tree findMax ( Tree T ) {
	if ( T == NULL )
		return NULL; //헤더를 쓰면 사용 안해도 괜찮음
	while( T -> right != NULL )
		T = T -> right;
	return T;
}

//BST를 받아 중위순회를 하는 함수.
void inorder ( Tree T ) {
	if( T ) {
		inorder ( T -> left );
		fprintf ( out, "%d ", T -> value );
		inorder ( T -> right );
	}
}

//BST를 받아 전위순회를 하는 함수.
void preorder ( Tree T ) {
	if( T ) {
		fprintf ( out, "%d ", T -> value );
		preorder ( T -> left );
		preorder ( T -> right ); 
	}   
}   

//BST를 받아 후위순회를 하는 함수.
void postorder ( Tree T ) {
	if( T ) {
		postorder ( T -> left );
		postorder ( T -> right );
		fprintf ( out, "%d ", T -> value );
	}   
}   
