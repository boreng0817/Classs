#include <iostream>
using namespace std;

class A{
	public:
		virtual void test1();
		virtual void test2() = 0;
};
class B : public A{
	public:
		void test2();
};
class C : public A{
	public:
		void test1();
		void test2();
};
class BB : public B{
	public:
		void test1();
};
