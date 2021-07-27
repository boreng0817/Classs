#include "MyVector.h"

MyVector MyVector::operator+(const MyVector& a){
	
	MyVector n(this->max_num);
	for(int i = 0 ; i < this->max_num ; ++i) this->a[i] += a.a[i];
	return n;
}

MyVector MyVector::operator-(const MyVector& a){
    MyVector n(this->max_num);
    for(int i = 0 ; i < this->max_num ; ++i) this->a[i] -= a.a[i];
    return n;
} 

MyVector MyVector::operator+(const int a){
    MyVector n(this->max_num);
    for(int i = 0 ; i < this->max_num ; ++i) this->a[i] += a;
    return n;
} 

MyVector MyVector::operator-(const int a){
    MyVector n(this->max_num);
    for(int i = 0 ; i < this->max_num ; ++i) this->a[i] -= a;
    return n;
}

ostream& operator << (ostream& out, MyVector& b){
	for(int i = 0 ; i < b.max_num ; ++i) out << b.a[i] << ' ';
	return out << endl;
}

istream& operator >> (istream& in, MyVector& b){
	for(int i = 0 ; i < b.max_num ; ++i) in >> b.a[i];
	return in;
}
