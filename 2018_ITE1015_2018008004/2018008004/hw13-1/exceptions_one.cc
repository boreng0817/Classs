#include <iostream>
using namespace std;

class A
{
	public:
		A(int n)
		{	if(n <= 0) throw "bad allocation";
			//implement something here
			cout << "ID=" << n << ": constructed\n";		
			n_ID = n;
			data = new int[n];
		}
		~A()
		{
			cout << "ID=" << n_ID << ": destroyed\n";
			//implement something here
			delete data;
		}
	private:
		int* data = NULL;
		int n_ID;
};

int main(){
	try{
		A a(3);
		A b(2);
		{
			A c(1);		
			A d(0);
			A e(-1);
		}
	}
	catch(const char* msg){
		cerr << "Caught in the main"<< endl;
	}
	//implement something here
	return 1;	
}

