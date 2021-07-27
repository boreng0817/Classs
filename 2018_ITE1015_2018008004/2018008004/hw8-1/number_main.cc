#include "number.h"

int main(){
string str;
int num;

cin >> str;
while(str != "quit"){
cin >> num;

if(str == "number"){
Number n;
n.setNumber(num);
cout<< "getNumber(): "<< n.getNumber() << endl;
}

else if(str == "square"){
Square s;
s.setNumber(num);
cout<< "getNumber(): "<< s.getNumber() << endl;
cout<< "getSquare(): "<< s.getSquare() << endl;
}

else{
Cube c;
c.setNumber(num);
cout<< "getNumber(): "<< c.getNumber() << endl;
cout<< "getSquare(): "<< c.getSquare() << endl;
cout<< "getCube(): "<< c.getCube() << endl;
}
cin>>str;
}

return 0;
}

