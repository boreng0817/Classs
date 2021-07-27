#include "string.h"

MyString MyString::operator+(const MyString& a){
string str1 = this->a,str2 = a.a;
return MyString((str1+str2).c_str());
}

MyString MyString::operator*(const int a){
string str1 = "",str2 = this->a;
for(int i = 0 ; i <a ; ++i) str1+=str2;
return MyString(str1.c_str());
}

ostream& operator<<(ostream& out, MyString& b){
	string str = b.a;
	return out<<str;
}

istream& operator>>(istream& in, MyString& b){
	string str;
	cin >> str;
	delete[] b.a;
	b.len = str.length();
	b.a = new char[b.len];
	strcpy(b.a,str.c_str());
	return in;
}
