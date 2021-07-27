#include<sstream>
#include<iostream>
#include<string>
#include<cstdlib>
#include"integer_set.h"
using namespace std;

int main(){
IntegerSet v;
string str;
//문자열 처리
while(getline(cin,str)){
if(str[0] == 'q') break;
istringstream iss(str);
iss >> str;
//작동
if(str[0] == 'a') {
iss >> str;
v.AddNumber( atoi(str.c_str())); }

else if(str[0] == 'g') {
iss >> str;
v.GetItem( atoi(str.c_str()) ); }

else if(str[0] == 'd') {
iss >> str;
v.DeleteNumber( atoi(str.c_str()) );
	} 
}
return 0;
}
