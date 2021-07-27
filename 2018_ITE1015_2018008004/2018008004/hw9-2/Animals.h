#include <iostream>
using namespace std;

class Animal{
	public:
		Animal(int num,string str);
		int age;
		string name;
		virtual void printInfo();
};

class Zebra : public Animal{
	public:
		Zebra(int num1,int num2,string str);
		int numStripes;
		void printInfo();
};

class Cat : public Animal{
	public:
		Cat(int num1,string str1,string str2);
		string favoriteToy;
		void printInfo();
};
