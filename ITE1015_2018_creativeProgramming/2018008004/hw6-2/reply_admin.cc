#include <iostream>
#include <string>
#include "reply_admin.h"
using namespace std;

bool ReplyAdmin::addChat(string chat){
if(count==10) return false;

else{
chats[count] = chat;
count++;
}}


bool ReplyAdmin::removeChat(int index){
if(index > count) return false;

else{

if(index == 9){
count--; 
return true;
}

else{
while(index<=count) {
chats[index] = chats[index+1];
index++;
}
count--;
return true;
		}
	}
}

bool ReplyAdmin::removeChat(int* indices,int len){
int y = 0;
for(int i = 0 ; i<len; i++){
if(indices[i]<=count){
removeChat(indices[i]);
for(int j = 0 ; j<len; j++) indices[j]-=1;
y++;
}}
return y;
}


bool ReplyAdmin::removeChat(int start, int end){
if(start<0) start = 0;
if(end>count) end = count;

int n = end - start + 1;
for(int i = 0 ; i < n ; i++ ) removeChat(start);

return n;
}

void ReplyAdmin::print(){
for( int i = 0 ; i<count ; i++) cout << i <<" "<<chats[i]<<endl;
}
