#include <iostream>
using namespace std;

class B { 
	public: 
		virtual ~B() {} 
}; 
class C : public B { 
	public: 
		void test_C() { cout << "C::test_C()" << endl; } 
}; 
class D : public B { 
	public: void test_D() { cout << "D::test_D()" << endl; } 
};
