#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;
typedef struct
{
	int studentID;
	char* studentName;
} ElementType;

struct Node
{
	ElementType element;
	PtrToNode next;
};

FILE* fptrw;

void Insert ( ElementType X, List L, Position P );
void Delete( int X, List L );
Position Find(int X, List L);
void PrintList ( List L );
List MakeEmpty( List L );
int IsEmpty( List L );
int IsLast( Position P, List L );
void DeleteList ( List L );

void Add ( ElementType X, List L );
void FindId ( int id, List L);
void Print ( List L );

int main (int argc, const char* argv[]) {
	List L;
	ElementType x;
	int num;
	char *name, *name1, *name2;
	char oper;
	FILE* fptr;
	fptr = fopen(argv[1], "r");
	fptrw = fopen("output.txt", "w");
	L = MakeEmpty ( L );

	while( 1 ) {
		fscanf( fptr, "%c", &oper );
		
		if ( oper == 'p' ) {
			Print( L );
			break;
		}

		else if ( oper == 'i' ) {

			name1 = malloc(40);
			name2 = malloc(40);
			name = malloc(40);

			fscanf( fptr, "%d %s %s", &num, name1, name2);

			strcpy(name, name1);
			strcat(name, " ");
			strcat(name, name2);
			
			x.studentID = num;
			x.studentName = malloc(40);
			x.studentName = name;
			Add(x, L);
			free(name);
			free(name1);
			free(name2);
		}

		else if ( oper == 'f') {
			fscanf( fptr, "%d", &num );
			FindId( num , L );
		}

		else if ( oper == 'd') {
			fscanf( fptr, "%d", &num );
			Delete( num, L );
		}

	}

	fclose(fptrw);
	fclose(fptr);
	DeleteList( L );

	return 0;
}

void Insert ( ElementType X, List L, Position P ) {
	Position cell;

	cell = malloc( sizeof( struct Node ) );
	if( cell == NULL )
		return;

	cell -> element.studentID = X.studentID;
	cell -> element.studentName = malloc(40);
    strcpy(cell -> element.studentName, X.studentName);
	cell -> next = P -> next;
	P -> next = cell;
}

void Delete (int id, List L ) {
	Position P = L, temp;
	
	while( P -> next != NULL && P -> next -> element.studentID != id ) 
		P = P -> next;
	
	if( !IsLast( P, L) ) {
	   temp = P -> next;
	   P -> next = temp -> next;
	   free(temp);
	   fprintf( fptrw, "Deletion Success : %d\n", id);
	   PrintList( L );
	}
	else
		fprintf( fptrw, "Deletion Failed : Student %d is not in the list.\n", id);
}

Position Find( int X, List L ) {
	Position P = L -> next;

	while( P != NULL && P->element.studentID != X )
		P = P -> next;

	return P;
}

void PrintList ( List L ) {
	Position P = L -> next;

	fprintf( fptrw, "Current List > ");

	while( P != NULL ) {
		fprintf( fptrw, "%d %s", P -> element.studentID , P -> element.studentName);
		if( P -> next != NULL )
			fprintf( fptrw, "-");
		P = P -> next;
	}

	fprintf( fptrw, "\n");

}

List MakeEmpty ( List L ) {
	L = (List)malloc( sizeof( struct Node ) );
	L -> element.studentID = -1;
	L -> element.studentName = "";
	return L;
}

int IsEmpty ( List L ) {
	return L -> next == NULL;
}

int IsLast( Position P, List L ) {
	return P -> next == NULL;
}
	
void DeleteList ( List L ) {
	Position temp, P;
	P = L -> next;

	while( P != NULL ) {
		temp = P -> next;
		free(P);
		P = temp;
	}
}

void Add ( ElementType X, List L ) {
	Position P = Find(X.studentID, L);
	int isDuplicate = ( P != NULL );

	if(isDuplicate) {
		fprintf( fptrw, "Insertion Failed. Id %d already exists.\n", X.studentID );
	}
	else {
		P = L;
		while( (P -> next != NULL) && (P -> next -> element.studentID < X.studentID) )
		   P = P -> next;
		Insert(X,L,P);
		fprintf( fptrw, "Insertion Success : %d\n", X.studentID);
		PrintList( L );
	}
}

void FindId ( int id, List L) {
	Position P = Find(id,L);
	if( P == NULL ) 
		fprintf( fptrw, "Find %d Failed. There is no student ID\n", id);
	else
		fprintf( fptrw, "Find Success : %d %s\n", id, P->element.studentName);
}

void Print (List L) {
	Position P = L -> next;
	fprintf( fptrw, "-----LIST-----\n");
	while( P != NULL ) {
		fprintf( fptrw, "%d %s\n", P -> element.studentID, P -> element.studentName );
		P = P -> next;
	}
	fprintf( fptrw, "--------------\n");
}
