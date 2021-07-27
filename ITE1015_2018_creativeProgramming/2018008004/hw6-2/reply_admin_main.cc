#include <iostream>
#include "reply_admin.h"
#include <cstring>
#include <cstdlib>

using namespace std;

int main(){
ReplyAdmin admin;
string line1;
int num[10]={0,},n1 = 8,n2 = 0;
char ch;

while(1){
getline(cin,line1);
char *line = new char[line1.length() + 1];
strcpy(line, line1.c_str());

if(line[0] == '#'){

if(line[1] == 'q') break;

else{
if(line[9]=='-') admin.removeChat( atoi(&line[8]) , atoi(&line[10]));

else if(line[9] == '\0') admin.removeChat(atoi(&line[8]));

else{
while(line[n1]!='\0'){
num[n2] = atoi(&line[n1]);
n1 += 2;
n2++;
}
admin.removeChat(num,n2);
n2 = 0;
n1 = 8;
}}}

else{
admin.addChat(line1);
}

admin.print();
}

admin.~ReplyAdmin();

return 0;
}
