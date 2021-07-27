#include <iostream>
#include "simple_account.h"
using namespace std;

int main(){
char ch;
int num[3] = {0,};
master mas;

while(1){
cout<<endl<<"Job?"<<endl;
cin >> ch;

/*if(!(mas.status(ch))){
if((ch=='D')||(ch=='W')){ 
 cin >> num[0] >> num[1];
 if( ((mas.getcount() < num[0])) ) cout << "Account does not exist" << endl;
 continue;
}
else if(ch=='T'){
 cin >> num[0] >> num[1] >> num[2];
 if( (mas.getcount() < num[0]) ||(mas.getcount() < num[1]) ) cout << "Account does not exist" << endl;
 continue;
}
else if(ch=='Q') break;
}
*/
if(ch=='N') mas.status();

else if(ch=='D'){
cin >> num[0] >> num[1];
mas.D(num[0],num[1]);
}

else if(ch=='W'){
cin >> num[0] >> num[1];
mas.W(num[0],num[1]);
}

else if(ch=='T'){
cin >> num[0] >> num[1]>>num[2];
mas.T(num[0],num[1],num[2]);
}

else if(ch=='Q')break;
}

return 0;
}


