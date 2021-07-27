#include <iostream>
#include <sstream>
#include "message_book.h"
using namespace std;

int main(){
MessageBook mes;
string str,temp;
int num;
//start of while
while(1){
getline(cin,str);
istringstream iss(str);
iss >> temp;

if( temp == "add" ){
iss >> num >> str;
while(iss >> temp) str += " " + temp;
mes.AddMessage(num, str); }

else if( temp == "delete" ){
iss >> num;
mes.DeleteMessage(num);
}

else if( temp == "list" ) mes.GetNumbers();
else if( temp == "print" ){
iss >> num;
mes.GetMessage(num);
}
else if( temp == "quit" ) break;
}
//end of while
return 0;
}
