#include <iostream>
#include "message_book.h"
using namespace std;

//CONST, DEST
MessageBook::MessageBook() {}
MessageBook::~MessageBook() {}
//FUNCTION
void MessageBook::AddMessage(int number, const string& message){ messages_[number] = message; }
void MessageBook::DeleteMessage(int number){ messages_.erase(number); }

vector<int> MessageBook::GetNumbers() const{
vector<int> for_return;
map<int,string>::const_iterator iter = messages_.begin();
for(;iter != messages_.end(); iter++) cout<< iter->first <<": "<< iter->second << endl;
return for_return;
}

const string& MessageBook::GetMessage(int number) const{
string str = ":P";
const string& strr = str;
if(messages_.find(number) != messages_.end()) cout << messages_.find(number)->second << endl;
else return strr;
return messages_.find(number)->second;
}
