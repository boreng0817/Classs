#include <stdio.h>
#include <stdlib.h>

void merge_iter(int size);
void merge_recur(int l, int r, int re);
void MR(int l, int r);
void init(int n);
void _free();

FILE* in;
FILE* out;
int* arr, *temp;
int main() {
	int n;

	in = fopen ( "input.txt", "r" );
	out = fopen ( "output.txt", "w" );

	fscanf(in, "%d", &n);

	init(n);
	merge_iter(n - 1);

	fprintf(out, "\nrecursive :\n");
	MR(0, n - 1);
	_free();

	fclose(in);
	fclose(out);

	return 0;
}

//recursive merge sort를 하는 함수 divide
void MR(int l, int r) {
	int m, i;
	if( l < r ) {
		m = (l+r)/2;
		MR(l,m);
		MR(m+1,r);
		merge_recur(l,m+1,r);

		for( i = l ; i <= r ; ++i )
			fprintf(out,  "%d ", arr[i]);
		fprintf(out,"\n");
	}
}

//merge하는 함수 (recursive)
void merge_recur(int l, int r, int re) {
	int i, le, size, idx;

	le = r - 1;
	idx = l;
	size = re - l + 1;

	for(;l <= le && r <= re;)
		if(arr[l] <= arr[r])
			temp[idx++] = arr[l++];
		else
			temp[idx++] = arr[r++];
	for(;l <= le;)
		temp[idx++] = arr[l++];
	for(;r <= re;)
		temp[idx++] = arr[r++];

	for( i = 0 ; i < size ; ++i, re-- )
		arr[re] = temp[re];
}

//iterative하게 merge하는 함수
void merge_iter(int size) {
	int block = 1, i, l, r, le, re, idx, ele;

	fprintf(out, "iterative : \n");

	while( block <=  size ) {
		l = 0;
		r = l + block;
		le = r - 1;
		re = le + block;
		idx = l;
		while( re <= size ) {
			while( l <= le && r <= re )
				if(arr[l] <= arr[r])
					temp[idx++] = arr[l++];
				else
					temp[idx++] = arr[r++];
			while(l <= le)
				temp[idx++] = arr[l++];
			while(r <= re)
				temp[idx++] = arr[r++];

			for(i = 0 ; i < block * 2 ; ++i, re--) 
				arr[re] = temp[re];
			for(i = 0 ; i < block * 2 ; ++i)
				fprintf(out, "%d ", temp[i + re + 1]);
			fprintf(out, "\n");

			l = re + 1 + 2 * block;
			r = l + block;
			le = r - 1;
			re = le + block;
			idx = l;
		}
		if( l + block <= size ) {
			re = size;
			ele = re - l + 1;
			while(l <= le && r <= re)
			if(arr[l] <= arr[r])  
				temp[idx++] = arr[l++];
			else
				temp[idx++] = arr[r++];
			while(l <= le)
				temp[idx++] = arr[l++];
			while(r <= re)
				temp[idx++] = arr[r++];		

			for(i = 0 ; i < ele ; ++i, re--) {
				arr[re] = temp[re];
			} 
			for(i = 0 ; i < ele ; ++i)
				fprintf(out, "%d ", arr[i + re + 1]);
			fprintf(out, "\n");
		}
		else if( l <= size ) { 
			for(i = 0 ; i < size - l + 1 ; ++i) 
				fprintf(out, "%d ", arr[l + i]);
			fprintf(out, "\n");
		}
		block *= 2;
	}

}

//사용할 arr과 temp를 할당하는 함수
void init(int n) {
	int i, num;
	arr = (int*)malloc( sizeof(int) * n );
	temp = (int*)malloc( sizeof(int) * n );

	for(i = 0 ; i < n ; ++i) {
		fscanf(in,"%d",&num);
		arr[i] = num;
	}

	fprintf(out, "input :\n");
	for(i = 0 ; i < n ; ++i)
		fprintf(out, "%d ", arr[i]);
	fprintf(out, "\n\n");
}

//다쓴 배열 2개를 free하는 함수
void _free() {
	free(arr);
	free(temp);
}
