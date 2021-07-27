#include "MyVector.h"

int main(){
	int n,num;
	string l;
	char r,op;
	cin >> l >> n;
	MyVector a(n),b(n);
	cout << "enter a"<<endl;
	cin >> a;
	cout << "enter b" << endl;
	cin >> b;
	while(true){
		cin >> l;
		if(l=="quit") break;
		else{
			cin >> op >> r;
			if(op=='+'){
			if(l=="a"){
				if(r=='a'){MyVector c(a+a);cout << a;}
				else if(r=='b'){MyVector c(a+b);cout << a;}
				else{const int num = r-'0';MyVector c(a+num);cout<<a;}
			}
			else{
				if(r=='a'){MyVector c(b+a);cout << b;}
				else if(r=='b'){MyVector c(b+b);cout << b;}
				else{const int num = r-'0';MyVector c(b+num);cout<<b;}
			}
		}
			else{
			if(l=="a"){
                if(r=='a'){MyVector c(a-a);cout << a;}
                else if(r=='b'){MyVector c(a-b);cout << a;}
                else{const int num = r-'0';MyVector c(a-num);cout<<a;}
            }
            else{
                if(r=='a'){MyVector c(b-a);cout << b;}
                else if(r=='b'){MyVector c(b-b);cout << b;}
                else{const int num = r-'0';MyVector c(b-num);cout<<b;}
            }
		}
		}
	}
	return 0;
}


