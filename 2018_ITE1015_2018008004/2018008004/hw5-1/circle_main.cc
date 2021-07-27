#include <iostream>
#include "circle.h"

using namespace std;

int main(){
double r;
cin >> r;
cout.precision(6);
cout<<fixed;

cout << "Perimeter: " << getCirclePerimeter(r) << endl << "Area: " <<getCircleArea(r) << endl;

return 0;
}
