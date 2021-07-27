#include "print_shape.h"
#include <vector>
#include <iostream>

int main(){
	vector<Shape*> v;
	char ch;
	double n1,n2;
	while(true){
		cin >> ch;
		if(ch=='c'){
			cin >> n1;
			v.push_back(new Circle(n1));
		}
		else if(ch=='r'){
			cin >> n1 >> n2;
			v.push_back(new Rectangle(n1,n2));
		}
		else if(ch=='0') break;
		else continue;
	}

	for(int i = 0 ; i < v.size() ; i++){
		cout << v[i]->getTypeString() << ", "<<v[i]->getArea() << ", " <<v[i]->getPerimeter()<<endl;
		delete v[i];
	}
	v.clear();
	return 0;
}
