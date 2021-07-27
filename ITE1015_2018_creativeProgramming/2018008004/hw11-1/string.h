#include <iostream>
#include <cstring>
using namespace std;

class MyString
{
	private:
		int len;
		char* a;
	public:
		MyString(const char*s = "") {len = strlen(s);a = new char[len+1];strcpy(a,s);}
		MyString(const MyString &s) {len=s.len;a=new char[len+1];strcpy(a,s.a);}
		~MyString(){delete[] a;}
		MyString operator+(const MyString& a);
		MyString operator*(const int a);
		friend ostream& operator << (ostream& out, MyString& b);
		friend istream& operator >> (istream& in, MyString& b);
};

