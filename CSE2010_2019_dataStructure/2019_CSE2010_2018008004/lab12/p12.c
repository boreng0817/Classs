#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEL 1234567890

typedef struct HashTbl *hash;
typedef struct HashTbl {
	int size;
	int* arr;
} HashTbl;

void insert(hash H, int X, int i, int sol);
void delete(hash H, int X, int sol);
int find(hash H, int X, int sol);
void print(hash H);
int Hash(int val, int size, int i, int sol);
hash make_hash(int size);
void free_hash(hash H);

FILE* in;
FILE* out;
int main() {
	int test_case, table_size, j, val, solution, idx;
	char sol[20], op[10];
	hash H;

	in = fopen( "input.txt", "r" );
	out = fopen( "output.txt","w" );

	fscanf( in, "%d", &test_case );

	for( j = 0 ; j < test_case ; ++j ) {

		fscanf( in, "%s", sol );
		if( !strcmp(sol, "Linear") ) {
			fprintf(out, "Linear\n");
			solution = 1;
		}
		else if( !strcmp(sol, "Quadratic") ) {
			fprintf(out, "Quadratic\n");
			solution = 2;
		}
		else if( !strcmp(sol, "Double") ) {
			fprintf(out, "Double\n");
			solution = 3;
		}
		else
			continue;

		fscanf( in, "%d", &table_size );

		H = make_hash( table_size );

		
		while( strcmp(op,"q") ) {
			fscanf( in, "%s", op );

			if( !strcmp(op,"p") ) {
				print( H );
				continue;
			}

			fscanf( in, "%d", &val );

			if( !strcmp(op, "i") )
				insert( H, val, 0, solution );

			else if( !strcmp(op, "f") ) {
				idx = find( H, val, solution );
				if(!idx) 
					fprintf(out, "not found\n");
				else
					fprintf(out, "%d is at %d\n", val, idx);
			}
			else if( !strcmp(op, "d" ) )
				delete( H, val, solution );
		}
		free_hash( H );
		op[0] = '1';
	}

	fclose( in );
	fclose( out );

	return 0;
}

//해시에 알맞는 probing으로 인서트하는 함수
//있다면 에러 메세지를, 성공하면 성공 메세지를 출력
void insert ( hash H, int X, int i, int sol ) {
	int exist = find( H, X, sol);
	int hash_val = Hash(X, H -> size, i, sol);

	if( exist ) {
		fprintf(out, "Already exists\n");
	}
	else {
		if( H -> arr[hash_val] != 0 && H -> arr[hash_val] != DEL ) 
			insert( H, X, i + 1, sol );
		else {
			H -> arr[hash_val] = X;
			fprintf(out, "Inserted %d\n", X); 
		}
	}
}

//val을 해싱하여 찾아 있다면 지워 DEL로 바꾸고 성공 메세지를 출력
//아니면 에러 메세지만 출력한다
void delete ( hash H, int X, int sol ) {
	int idx = find( H, X, sol );
	if( !idx ) 
		fprintf(out, "%d not exists\n", X); 
	else {
		fprintf(out, "Deleted %d\n", X);
		H -> arr[idx] = DEL;
	}
}

//val를 해싱하여 찾고, 자리가 채워져 있는데 값이 다르다면
//probing을 하여 찾고, 없다면 0을 리턴
int find( hash H, int X, int sol ) {
	int i = 0, idx = Hash( X, H -> size, i, sol );

	while( H -> arr[idx] && H -> arr[idx] != DEL ) {
		if( H -> arr[idx] == X )
			return idx;
		else
			idx = Hash( X, H -> size, ++i, sol );
	}

	return 0;
}

//해싱테이블을 출력하는 함수
void print( hash H ) {
	int i;
	for( i = 0 ; i < H -> size ; ++i ) {
		if( H -> arr[i] == DEL )
			H -> arr[i] = 0;
		fprintf( out, "%d ", H -> arr[i] );
	}
	fprintf( out, "\n\n" );
}

//알맞는 solution에 해당하는 해싱을 하는 함수
int Hash( int val, int size, int i, int sol ){
	if( sol == 1 ) 
		return (val + i) % size;
	else if( sol == 2 )
		return (val + i*i) % size;
	else if( sol == 3 ) 
		return (val	+ i*(7 - val%7)) % size;
}

//빈 해시테이블을 만들어 리턴하는 함수
hash make_hash( int size ) {
	int i;
	hash H = malloc( sizeof ( HashTbl ) );
	H -> arr = malloc( sizeof(int) * size );
	H -> size = size;

	for( i = 0 ; i < size ; ++i )
		H -> arr[i] = 0;

	return H;
}

//다 사용한 해시테이블을 지우는 함수
void free_hash ( hash H ) {
	free( H -> arr );
	free( H );
}
