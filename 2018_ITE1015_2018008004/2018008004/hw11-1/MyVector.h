#include <iostream>
using namespace std;
class MyVector
{
	private:
		int max_num;
		double* a;
	public:
		MyVector(const int& num){max_num=num;a = new double[max_num];}
		MyVector(const MyVector& b){max_num=b.max_num;a = new double[max_num];}
		~MyVector(){delete[] a;}
		//Constructor, Destructor
		MyVector operator+(const MyVector& a);
		MyVector operator-(const MyVector& a);
		MyVector operator+(const int a);
		MyVector operator-(const int a);
		friend ostream& operator << (ostream& out, MyVector& b);
		friend istream& operator >> (istream& in, MyVector& b);
};

