#include <iostream>
#include "simple_shape.h"
using namespace std;

int main(){
char ch;
int num[4] = {0,};
circle C;
rectangular R;

while(1){
cout << endl << "shape?" << endl;
cin>>ch;
if(ch=='C'){
cin >> num[0] >> num[1] >> num[2];
C.setCircle(num[0],num[1],num[2]);
C.print();
}
else if(ch=='R'){
cin >> num[0] >> num[1] >> num[2]>>num[3];
R.setRectangular(num[0],num[1],num[2],num[3]);
R.print();
}
else break;
}
}




