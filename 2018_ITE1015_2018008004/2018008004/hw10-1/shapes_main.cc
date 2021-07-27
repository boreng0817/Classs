#include "shapes.h"
#include <vector>
#include <iostream>

int main(){
	vector<Shape*> v;
	double n1,n2;
	cin >> n1 >> n2;
	v.push_back(new Triangle(n1,n2));
	cin >> n1 >> n2;
	v.push_back(new Rectangle(n1,n2));
	for(int i = 0 ; i < 2 ; i++){
		cout << v[i]->getArea() << endl;
		delete v[i];
	}
	v.clear();
	return 0;
}
