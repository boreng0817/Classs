#include <iostream>
#include "magic_square.h"
#define MOD(X,Y) ((X%Y)+Y)%Y
using namespace std;

void magic_square (int* arr, int num){
int row = 1, col = num/2-1,count = 1;

for( int i = 0 ; i<num*num ; i++){
//오른쪽 위로
col++;
row--;

//집어 넣으려는 원소가 0이 아니거나 count가 n(n+1)/2 + 1 이면  row++
if((arr[MOD(row,num)*num + MOD(col,num)] != 0)||(count == num*(num+1)/2 + 1)) {
row += 2;
col--;	}

//원소 대입
arr[MOD(row,num)*num + MOD(col,num)] = count++;	}
}
