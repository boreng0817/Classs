#include <iostream>

using namespace std;

int main(){
int num, max, min;
cin >> num;
int *p = new int[num];
for( int i = 0 ; i < num ; i++)
cin >> p[i];

max = p[0];
min = p[0];

for( int i = 0 ; i < num ; i++){
max = max > p[i] ? max:p[i];
min = p[i] > min ? min:p[i]; }

cout << "min: " << min << endl 
     << "max: " << max << endl;

delete[] p;

return 0;
}
