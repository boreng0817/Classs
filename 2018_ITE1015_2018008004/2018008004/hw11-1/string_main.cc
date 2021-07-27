#include "string.h"

int main(){
	string str;
	char op,r,l;
	MyString a(""),b("");

	while(true){
		cin >> str;
		if(str == "quit") break;
		else if(str == "new"){
			cout << "enter a"<<endl;
			cin >> a;
			cout << "enter b"<<endl;
			cin >> b;
		}
		else{
		cin >> op >> r;
		l = str[0];
		if(op=='+'){
			if(l=='a'&&r=='b'){MyString c(a+b);cout<<c<<endl;}
			else if(l=='a'&&r=='a'){MyString c(a+a);cout<<c<<endl;}
			else if(l=='b'&&r=='a'){MyString c(b+a);cout<<c<<endl;}
			else if(l=='b'&&r=='b'){MyString c(b+b);cout<<c<<endl;}
		}
		else{
			if(l=='a'){MyString c(a*(r-'0'));cout<<c<<endl;}
			else {MyString c(b*(r-'0'));cout<<c<<endl;}
		}
		}
	}
	return 0;
}
