#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include "sorted_array.h"
using namespace std;

int main(){
string str, temp;
SortedArray arr;
vector<string> v;

while(getline(cin, str)){
if(str.substr(0,6) == "ascend") arr.GetSortedAscending();
else if(str.substr(0,7) == "descend") arr.GetSortedDescending();
else if(str.substr(0,3) == "max") arr.GetMax();
else if(str.substr(0,3) == "min") arr.GetMin();
else if(str.substr(0,4) == "quit") break;
else {
istringstream iss(str);
for(;iss >> str; ) v.push_back(str);
while(!v.empty()) {
temp = v[v.size()-1];
arr.AddNumber( atoi( temp.c_str() ) );
v.pop_back();
		}
	}
}
return 0;
}
