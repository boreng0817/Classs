#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node *PtrToNode;
typedef PtrToNode Stack;

struct Node {
	int element;
	PtrToNode next;
};

Stack CreateStack ();
int IsEmpty ( Stack S );
void MakeEmpty ( Stack S );
void Push ( int X, Stack S );
void Top ( Stack S );
void Pop ( Stack S );


FILE* out;
int main ( int args, const char * argv[] ) {
	int num, N, i;
	FILE* in;
	char * oper;

	Stack S = CreateStack();

	in = fopen( argv[1], "r");
	out = fopen( "output.txt", "w");

	fscanf(in, "%d", &N);

	for( i = 0 ; i < N ; ++i) {
		fscanf(in, "%s", oper);

		if( !strcmp(oper, "push") ) {
			fscanf( in, "%d", &num );
			Push( num, S );
		}
		else {
			Top( S );
			Pop( S );
		}
	}

	MakeEmpty( S );
	fclose( out );
	fclose( in );

	return 0;
}

//빈 스택을 생성합니다
Stack CreateStack () {
	Stack S;
	S = malloc( sizeof(struct Node) );

	if( S == NULL)
		return NULL;

	S -> element = 0; //HEADER
	S -> next = NULL;
	return S;
}

//스택을 인자로 받습니다. 스택이 비었는지 확인합니다
int IsEmpty ( Stack S ) {
	return S -> next == NULL;
}

//스택을 인자로 받습니다. 스택을 비웁니다.
void MakeEmpty ( Stack S ) {
	while( !IsEmpty( S ) ) 
		Pop( S );
}

//int형 Element와 Stack을 인자로 받습니다. Top위에 Element를 push 합니다,
void Push ( int X, Stack S ) {
	PtrToNode temp;
	temp = malloc( sizeof( struct Node ) );
	temp -> element = X;
	temp -> next = S -> next;
	S -> next = temp;
}

//Stack을 인자로 받아 Stack의 맨 위 원소를 리턴합니다. 비어있다면 Empty를 출력합니다.
void Top ( Stack S ) {
	if( IsEmpty ( S ) )
		fprintf( out, "Empty\n" );
	else
		fprintf( out, "%d\n", S -> next -> element);
}

//Stack을 인자로 받아 맨위 원소를 지웁니다. 비어있다면 아무것도 하지 않습니다.
void Pop ( Stack S ) { 
	if( IsEmpty ( S ) )
		return;
	else {
		PtrToNode temp = S -> next;
		S -> next = temp -> next;
		free(temp);
	}
}
