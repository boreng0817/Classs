#include <iostream>
using namespace std;

class A
{
};
class B : public A
{
};
class C : public B
{
};
int main(){
	int n;
	cout << "input num(0~2):";
	cin >> n;	
		try{
		if(n == 0)
			throw new A;
		else if(n == 1)
			throw new B;
		else if (n ==2)
			throw new C;
		else
			throw string("invalid input");
	}
		catch(C* e){
			cout << "throw C new has been called" << endl;
		}
		catch(B* e){
			cout << "throw B new has been called" << endl;
		}
		catch(A* e){
			cout << "throw A new has been called" << endl;
		}
		catch(string msg){
			cout << msg << endl;
		}


	
	//implement here
	return 1;
}


