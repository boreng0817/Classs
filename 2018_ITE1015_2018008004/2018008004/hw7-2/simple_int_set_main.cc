#include <set>
#include <iostream>
#include "simple_int_set.h"
using namespace std;

int main(){
set<int> set0,set1,result;
set<int>::iterator iter;
string op;
//start of while
while(1){
if(!InputSet(&set0, &set1, &op)) break;

if(op == "+") print( SetUnion(set0,set1) );
else if(op == "-") print( SetDifference(set0, set1) );
else if(op == "*") print( SetIntersection(set0, set1) );
else break;
set0.clear();
set1.clear();
}
//end of while
return 0;
}
