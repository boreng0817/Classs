#include "rectangle.h"

int main(){
string str;
int n1,n2;

cin >> str;

while(str != "quit"){
cin >> n1;
if(str == "square") {Square s(n1); s.print();}
else{ cin>>n2; NonSquare n(n1,n2); n.print();}
cin >> str;
}

return 0;
}

