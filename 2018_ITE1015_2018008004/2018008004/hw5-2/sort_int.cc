#include <iostream>
#include "sort.h"
using namespace std;

int main(){
int num;

cin >> num;

if(num>0){
int* arr = new int[num];
//정렬 전
for(int i = 0 ; i < num ; i++) cin >> arr[i];

sort(arr,num);
//정렬 후
for(int i = 0 ; i < num ; i++) cout<<arr[i]<<" ";
cout<<endl;

delete[] arr;
}

return 0;
}
