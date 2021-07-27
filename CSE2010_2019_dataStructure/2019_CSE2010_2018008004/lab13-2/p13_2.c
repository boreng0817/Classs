#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SWAP(A,B,T) (T)=(A), (A)=(B), (B)=(T) //간단한 int 2개 swap함수

void QS(int l, int r, int m);
int p(int l,int r,int m);
void print(int size);

int* arr;
FILE* in;
FILE* out;

int main() {
	int m, n, num, i;
	char op[50];

	in = fopen ( "input.txt", "r" );
	out = fopen ( "output.txt", "w" );

	while( fscanf(in, "%s", op ) == 1 ) { 

		fscanf(in, "%d", &n);

		arr = (int*)malloc(sizeof(int) * n);
		for ( i = 0 ; i < n ; ++i ) {
			fscanf(in, "%d", &num );
			arr[i] = num;
		}

		fprintf(out, "%s:\n", op);

		if ( !strcmp(op, "leftmost") ) 
			m = -1;
		else if ( !strcmp(op, "rightmost") ) 
			m = 1;
		else if( !strcmp(op, "middle") ) 
			m = 0;

		QS(0, n - 1, m);
		print(n);
		free(arr);
	}

	fclose(in);
	fclose(out);

	return 0;
}

//퀵 소트를 재귀적으로 구현한 함수.
//mode m 에 따라 left, right, middle로 Pivot을 정하여 소트함.
void QS(int l, int r, int m) {
	int idx, i;
	if( l >= r ) return;

	idx = p(l,r,m);

	fprintf(out, "<");
	for( i = l ; i <= r ; ++i) {
		if( idx == i )
			fprintf(out, "><%d ><", arr[i]);
		else
			fprintf(out, "%d ", arr[i]);
	}
	fprintf(out, ">\n");

	QS(l, idx - 1, m);
	QS(idx + 1, r, m);
}

//m = 0인 경우 middle
//m = -1인 경우 left
//m = 1인 경우 right로 Pivot을 정해 partition하는 함수
int p(int l,int r,int m) {
	int pivot, i, j, k, tmp, idx;

	if(m == 0) {
		i = l - 1;
		j = r + 1;
		idx = (l + r)/2;

		pivot = arr[idx];

		for(;;) {
			for(;arr[--j] > pivot;);
			for(;arr[++i] < pivot;);
			if( i < j )  {
				if(i == idx) idx = j;
				else if(j == idx) idx = i;
				SWAP(arr[i], arr[j], tmp);
			}
			else {
				return idx;
			}
		}

	}

	else if(m == -1) {
		i = l;
		j = r + 1;

		pivot = arr[l];

		for(;;) {
			for(;arr[--j] > pivot;);
			for(;arr[++i] < pivot;);
			if( i < j )  
				SWAP(arr[i],arr[j], tmp);
			else {
				SWAP(arr[j],arr[l], tmp);
				return j;
			}
		}

	}

	else if(m == 1) {
		i = l - 1;
		j = r;

		pivot = arr[r];

		for(;;) {
			for(;arr[--j] > pivot;);
			for(;arr[++i] < pivot;);
			if( i < j )  
				SWAP(arr[i],arr[j], tmp);
			else {
				SWAP(arr[i],arr[r], tmp);
				return i;
			}
		}
	}
}   

//길이가 size인 int 배열을 출력하는 함수
void print(int size) {
	int i;

	fprintf(out,"\nresult\n");
	for( i = 0 ; i < size ; ++i) 
		fprintf(out, "%d ", arr[i] );
	fprintf(out,"\n\n");
}


