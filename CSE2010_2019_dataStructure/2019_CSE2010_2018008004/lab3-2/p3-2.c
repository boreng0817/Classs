#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE_SIZE 100

struct QueueRecord;
typedef struct QueueRecord *Queue;

struct QueueRecord {
	int front;
	int rear;
	int size;
	int *array;
};

void MakeEmpty ( Queue Q );
void Enqueue ( int item, Queue Q );
int Dequeue ( Queue Q );

int main ( int argc, const char * argv[] ) {
	int N, num, i;
	char *oper;
	FILE *in, *out;
	Queue Q;
	Q = malloc( sizeof(struct QueueRecord) );

	MakeEmpty ( Q );
	Q->array = (int*)malloc( sizeof(int) * MAX_QUEUE_SIZE );

	in = fopen(argv[1], "r");
	out = fopen("output.txt", "w");

	fscanf( in, "%d", &N );

	for( i = 0 ; i < N ; ++i ) {
		fscanf( in, "%s", oper );

		if( !strcmp( oper, "enQ" ) ) {
			
			fscanf( in, "%d", &num );
			
			if( Q -> size == MAX_QUEUE_SIZE )
				fprintf( out, "Full\n" );
			
			else
				Enqueue( num, Q );
		}

		else {
			if( Q -> size == 0)
				fprintf( out, "Empty\n" );
			
			else 
				fprintf( out, "%d\n", Dequeue( Q ) );
		}
	}

	free( Q -> array );
	fclose(in);
	fclose(out);

	return 0;
}

//인자로 큐를 받아 빈 상태로 초기화 시킵니다
void MakeEmpty ( Queue Q ) {
	Q -> size = 0;
	Q -> front = 1;
	Q -> rear = 0;
}

//인자로 int 하나와 큐를 받아 rear + 1에 넣습니다
void Enqueue ( int item, Queue Q ) {
	Q -> rear = (Q -> rear + 1) % MAX_QUEUE_SIZE;
	Q -> array[ Q -> rear ] = item;
	Q -> size++;
}

//인자로 큐를 받아 front에 있는 원소를 return합니다
int Dequeue ( Queue Q ) {
	int temp = Q -> array[ Q -> front ];
	Q -> front = (Q -> front + 1) % MAX_QUEUE_SIZE;
	Q -> size--;
	return temp;
}
   	

