#include "dynamic_cast.h"
#include <vector>
#include <typeinfo>

int main(){
	vector<B*> v;
	string str;
	while(true){
		cin >> str;
		if(str == "B") v.push_back(new B);
		else if(str == "C") v.push_back(new C);
		else if(str == "D") v.push_back(new D);
		else if(str == "0") break;
		else break;
	}
	for(int i = 0 ; i < v.size() ; i++){
		if(dynamic_cast<C*>(v[i])) dynamic_cast<C*>(v[i])->test_C();
		else if(dynamic_cast<D*>(v[i])) dynamic_cast<D*>(v[i])->test_D();
	}
	for(int i = 0 ; i < v.size() ; i++) delete v[i];
	v.clear();
	return 0;
}
