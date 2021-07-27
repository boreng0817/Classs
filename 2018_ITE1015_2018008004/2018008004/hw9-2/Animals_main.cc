#include "Animals.h"
#include <vector>

int main(){
	string str1,str2;
	char ch;
	int num1,num2;
	vector<Animal*> v;

	while(1){
	cin >> ch;
	if( ch == '0' ) break;
	
	else if(ch == 'z'){
		cin >> str1 >> num1 >> num2;
		v.push_back(new Zebra(num2,num1,str1) );
	}
	else if(ch == 'c'){
		cin >> str1 >> num1 >> str2;
		v.push_back(new Cat(num1,str1,str2) );
	}
	}

	for(int i = 0 ; i < v.size() ; i++) v[i]->printInfo();
	for(int i = 0 ; i < v.size() ; i++) delete v[i];

	return 0;
}

