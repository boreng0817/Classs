#include "class_function.h"
#include <vector>

int main(){
	vector<A*> v;
	string str;
	while(true){
		cin >> str;
		if(str=="B") v.push_back(new B);
		else if(str=="C") v.push_back(new C);
		else if(str=="BB") v.push_back(new BB);
		else if(str=="0") break;
		else break;
	}
	for(int i = 0 ; i < v.size() ; i++){
		v[i]->test1();
		v[i]->test2();
	}
	for(int i = 0 ; i < v.size() ; i++) delete v[i];
	v.clear();
	return 0;
}


