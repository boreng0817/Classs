#include "sort.h"

void sort (int a[], int length){
int temp = 0;

for(int i = 0 ; i <length-1 ; i++){
	for(int j = 0 ; j < length - 1 - i ;j++){
		if(a[j]>a[j+1]){
			temp = a[j];
			a[j] = a[j+1];
			a[j+1] = temp;
			}
		}
	}	
}
