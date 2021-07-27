#include <iostream>

using namespace std;

int main(){
int num;
cin >> num;

int *p = new int[num];

for(int i = 0 ; i < num ; i++)
 p[i] = i;
for(int i = 0 ; i < num-1 ; i++)
 cout << p[i] << ' ';
cout << p[num-1] << endl;

delete[] p;
return 0;
}
