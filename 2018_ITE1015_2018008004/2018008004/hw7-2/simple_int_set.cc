#include <set>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "simple_int_set.h"
using namespace std;

set<int> SetIntersection(const set<int>& set0, const set<int>& set1){
set<int> inter;
set<int>::iterator it = set1.begin();
for(;it != set1.end(); it++)
 if(set0.find(*it) != set0.end()) inter.insert(*it);
return inter;
}

set<int> SetUnion(const set<int>& set0, const set<int>& set1){
set<int>::iterator it = set1.begin();
set<int> Union = set0;
for(;it != set1.end() ; it++) Union.insert(*it);
return Union;
}

set<int> SetDifference(const set<int>& set0, const set<int>& set1){
set<int>::iterator it = set1.begin();
set<int> diff = set0;
for(;it != set1.end() ; it++)
 if(set0.find(*it) != set0.end()) diff.erase(*it);
return diff;
}

bool InputSet(set<int>* s0, set<int>* s1, string* op){
string str;
getline(cin,str);
istringstream iss(str);
//{ 처리
iss >> str;
if(str != "{") return false;
//s0
while(1){
iss >> str;
if(str == "}") break;
if(!string_test(str)) return false; // for invalid element
s0->insert( atoi(str.c_str()) );
}
//operator
iss >> *op;
iss >> str;
if(str != "{") return false;
//s1
while(1){
iss >> str;
if(str == "}") break;
if(!string_test(str)) return false; // for invalid element
s1->insert( atoi(str.c_str()) );
}
//if there's no problem return true
return true;
}

void print(set<int> a){
set<int>::iterator it = a.begin();
cout << "{ ";
for(;it != a.end();it++) cout << *it << ' ';
cout << '}' << endl;}

bool string_test(string str){
string temp = "";
if( str[0] == '0' && str.length() != 1 ) return false; // element starts with 0 ex) 00001, 00123 
//for|check element whether they are form of integer or not
for( int i = 0 ; i < str.length() ; i++ ){
if( str[0] == '-' || str[0] == '+') continue; //sign doesn't matter
temp += str[i];
if( (atoi(temp.c_str()) == 0) && ( str[i]!='0') ) return false;
temp = "";
}
//end of for|check
return true;
}

