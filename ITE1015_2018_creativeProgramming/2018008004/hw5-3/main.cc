#include <iostream>
#include <cstdlib>
#include "magic_square.h"
using namespace std;

int main(int argc, const char** argv){
int num = atoi(argv[1]);

if( (num>=3) && (num%2==1) ) {
int *arr = new int[num*num];
//배열의 원소 모두 0으로 만들기
for(int i = 0 ; i < num ; i++)
	for(int j = 0 ; j < num ; j++) arr[i*num + j] = 0;
//마방진 삽입
magic_square(arr,num);
//마방진 출력
for(int i = 0 ; i < num ; i++) {
	for(int j = 0 ; j < num ; j++)	cout << arr[i*num + j] << " ";
	cout << endl;
}
//배열 삭제
delete[] arr;
}

return 0;
}
