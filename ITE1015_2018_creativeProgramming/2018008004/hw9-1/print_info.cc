#include "print_info.h"

string A::getTypeInfo(){ return "This is an instance of class A";}
string B::getTypeInfo(){ return "This is an instance of class B";}
string C::getTypeInfo(){ return "This is an instance of class C";}

void printObjectTypeInfo1(A* object){cout << object->getTypeInfo() << endl;}
void printObjectTypeInfo2(A& object){cout << object.getTypeInfo() << endl;}
