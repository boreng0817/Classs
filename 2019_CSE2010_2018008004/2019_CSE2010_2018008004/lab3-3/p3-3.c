#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node *PtrToNode;
typedef PtrToNode Stack;

struct Node {
	int element;
	int priority;
	PtrToNode next;
};

Stack CreateStack ();
int IsEmpty ( Stack S );
void MakeEmpty ( Stack S );
void Push ( int X, Stack S );
int Top ( Stack S );
void Pop ( Stack S );

int Priority ( char ch );
void reverse ( char str[] );

int main ( int argc, const char * argv[]) {
	FILE * in, * out;
	char ch, temp;
	char str[110];
	char* infix;
	Stack S = CreateStack ();
	Stack postfix = CreateStack ();
	int i, j, len, num1, num2;

	in = fopen( argv[1], "r" );
	out = fopen( "output.txt", "w" );

	infix = malloc(110);
	fscanf( in, "%s", infix );

	len = strlen( infix );

	for ( i = 0 ; i < len ; ++i ) {
		ch = infix[i];

		if ( ch >= 48 && ch <= 57 )  //ch is number
			Push ( ch, postfix );

		else if ( ch == '#' ) {
			while ( !IsEmpty ( S ) ) {
				temp = Top ( S );
				Pop ( S );
				Push ( temp, postfix );
			}
		}

		else if ( ch == '(' || IsEmpty( S ) || Priority( Top ( S ) ) < Priority( ch ) )
			Push ( ch, S );

		else if ( ch == ')' ) {
			while ( Top ( S ) != '(' ) {
				temp = Top ( S );
				Pop ( S );
				Push ( temp, postfix );
			}
			Pop ( S );
		}

		else if ( Priority ( Top ( S ) ) >= Priority ( ch ) ) {

			while ( 1 ) {
				if( IsEmpty ( S ) || Priority ( Top ( S ) ) < Priority ( ch ) )
					break;

				temp = Top ( S );
				Pop ( S );
				Push ( temp, postfix );
			}
			Push ( ch, S );
		}
	}

	infix[len - 1] = '\0';

	for ( i = 0 ; !( IsEmpty ( postfix ) ) ; ++i ) {
		temp = Top ( postfix );
		Pop ( postfix );
		str[i] = temp;
	}

	reverse ( str ); // prefix to postfix
	
	len = strlen( str );
	i = 0;
	while ( i < len ) {
		ch = str[i++];

		if ( ch >= 48 && ch <= 57 ) 
			Push ( ch - '0', S );

		else {
			num2 = Top ( S );
			Pop ( S );
			num1 = Top ( S );
			Pop ( S );

			if ( ch == '+' ) 
				Push ( num1 + num2 , S );
			else if ( ch == '-' )
				Push ( num1 - num2 , S );
			else if ( ch == '/' )
				Push ( num1 / num2 , S );
			else if ( ch == '%' )
				Push ( num1 % num2 , S );
			else if ( ch == '*' )
				Push ( num1 * num2 , S );
		}
	}

	fprintf ( out, "Infix Form : %s\n"
				   "Postfix Form : %s\n"
				   "Evaluation Result : %d\n", infix, str, Top ( S )  );

	fclose ( in );
	fclose ( out );
	MakeEmpty ( S );
	MakeEmpty ( postfix );
	free(infix);

	return 0;
}

//스택을 만들어 리턴해주는 함수
Stack CreateStack () {
	Stack S;
	S = malloc( sizeof(struct Node) );

	if( S == NULL)
		return NULL;

	S -> element = 0; //HEADER
	S -> next = NULL;
	S -> priority = -1;

	return S;
}

//스택을 인자로 받아 스택이 비었으면 true를, 아니면 false를 리턴하는 함수
int IsEmpty ( Stack S ) {
	return S -> next == NULL;
}

//스택을 인자로 받아 받은 스택을 비우는 함수
void MakeEmpty ( Stack S ) {
	while( !IsEmpty( S ) )
		Pop( S );
}

//스택과 Element를 인자로 받아 스택의 TOP에 인자를 Insert하는 함수
void Push ( int X, Stack S ) {
	PtrToNode temp;
	temp = malloc( sizeof( struct Node ) );
	temp -> priority = Priority ( X );
	temp -> element = X;
	temp -> next = S -> next;
	S -> next = temp;
}

//스택을 인자로 받아 스택의 TOP을 리턴하는 함수
int Top ( Stack S ) {
	if( IsEmpty ( S ) )
		return '\0';
	else
		return S -> next -> element;
}

//Stack을 인자로 받아 스택의 Top에 있는 원소를 스택에서 삭제하는 함수
void Pop ( Stack S ) {
	if( IsEmpty ( S ) )
		return;
	else {
		PtrToNode temp = S -> next;
		S -> next = temp -> next;
		free(temp);
	}
}

//character를 받아 우선 순위를 배정해주는 함수
int Priority ( char ch ) {
	if( ch == '%' || ch == '/' || ch == '*' )
		return 1;
	else if( ch == '+' || ch == '-' )
		return 0;
	else if( ch == '(' )
		return -1;
	else if( ch == ')' )
		return -2;
	else
		return 7;
}

//char*를 인자로 받아 char *를 좌우 반전시키는 함수
void reverse(char s[])
{
	int c, i, j;

	for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
