#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ORDER 3
#define max(A,B) (A) > (B) ? (A) : (B)
#define min(A,B) (A) < (B) ? (A) : (B)
#define MAX(A,B,C) max(max(A,B), max(B,C))
#define MIN(A,B,C) min(min(A,B), min(B,C))
#define MID(A,B,C) (min(A,B))^(min(B,C))^(min(C,A))

typedef struct B_node* btree;
typedef struct B_node {
	int num;
	btree child[ORDER + 1];
	int key[ORDER];
} B_node;

void insert(int key, btree T);
void ins(int key, btree T);
btree find_prev(int key, btree T);
void inorder(btree T);
btree init(btree T);
void split(btree T, btree C);

FILE* in;
FILE* out;
int main() {
	int n;
	char op[5];
	btree B = init(B);

	in = fopen("input.txt", "r");
	out = fopen("output.txt", "w");

	while( fscanf(in,"%s", op) == 1 ) {
		if( !strcmp(op,"i") ) {
			fscanf(in, "%d", &n);
			insert(n,B);
		}
		else if( !strcmp(op,"p") ) 
			inorder(B);
	}
	fclose(in);
	fclose(out);
	free(B);

	return 0;
}

//빈 node를 생성하는 함수
btree init(btree T) {
	int i;
	T = malloc(sizeof(struct B_node));
	T -> num = 0;
	for( i = 0 ; i < 4 ; ++i) {
		T -> child[i] = malloc(sizeof(struct B_node));
		T -> child[i] = NULL;
	}
	return T;
}

//재귀적으로 b-tree를 inorder traversal하는 함수
void inorder(btree T) {
	if(T) {
		inorder(T -> child[0]);
		if(T -> num > 0)
			fprintf(out, "%d ", T -> key[0]);
		inorder(T -> child[1]);
		if( T -> num > 1)
			fprintf(out, "%d ", T -> key[1]);
		inorder(T -> child[2]);
	}
}

//node의 parent node를 찾아 return 해주는 함수
btree find_prev(int key, btree T) {
	btree temp = T, parent = T;

	while(temp) {
		if( temp -> key[0] == key || (temp -> num == 2 && temp -> key[1] == key ) )
			break;
		parent = temp;
		if( key < temp -> key[0]  )
			temp = temp -> child[0];
		else if( temp -> num == 1 && temp -> key[0] < key )
			temp = temp -> child[1];
		else if( temp -> num == 2 && temp -> key[1] < key )
			temp = temp -> child[2];
		else if( temp -> num == 2 && temp -> key[1] > key )
			temp = temp -> child[1];
		else 
			break;
	}
	return parent;
}

//node에 key를 insert하는 함수. leaf에 도달한 뒤 overflow를 처리해줌
void insert(int key, btree T) {
	btree search = T, temp = T;

	while(temp && temp -> num != 0) {
		search = temp;
		if( key < temp -> key[0] )
			temp = temp -> child[0];
		else if( temp -> num == 1 && temp -> key[0] < key )
			temp = temp -> child[1];
		else if( temp -> num == 2 && temp -> key[1] < key )
			temp = temp -> child[2]; 
		else if( temp -> num == 2 && temp -> key[1] > key )
			temp = temp -> child[1];
	}

	ins(key,search);
	split(search,T);
}

//node에 key를 일단 대입하는 함수
void ins(int key, btree search) {
	if( search -> num == 0 )
		search -> key[0] = key;

	else if( search -> num == 1 ) {
		if( search -> key[0] < key )
			search -> key[1] = key;
		else {
			search -> key[1] = search -> key[0];
			search -> key[0] = key;
		}
	}

	else {
		int a = key, b = search -> key[0], c = search -> key[1];
		search -> key[0] = MIN(a,b,c);
		search -> key[1] = MID(a,b,c);
		search -> key[2] = MAX(a,b,c);
	}
	++(search -> num);
}

//node와 btree를 받아 overflow된 node를 split하는 함수
void split(btree node, btree T) {
	int i;
	if ( node -> num == 3 ) {
		if ( node == T ) {
			btree right = init(right), left = init(left);
			left -> key[0] = node -> key[0];
			right -> key[0] = node -> key[2];
			node -> key[0] = node -> key[1];
			node -> num = right -> num = left -> num = 1;
			for ( i = 0 ; i < 2 ; ++i ) {
				left -> child[i] = node -> child[i];
				right -> child[i] = node -> child[i+2];
			}
			node -> child[0] = left;
			node -> child[1] = right;
			node -> child[2] = node -> child[3] = NULL;
		}
		else {
			btree parent = find_prev(node -> key[0], T);
			for(i = 2 ; parent -> child[i] != node && i >= 0 ; --i) 
				parent -> child[i+1] = parent -> child[i];
			btree temp = init(temp);
			temp -> num = node -> num = 1;
			temp -> key[0] = node -> key[2];
			ins(node -> key[1],parent);
			parent -> child[i+1] = temp;
			for(i = 0 ; i < 2 ; ++i) { 
				temp -> child[i] = node -> child[i+2];
				node -> child[i+2] = NULL;
			}

			split(parent,T);
		}

	}
	else
		return;
}
