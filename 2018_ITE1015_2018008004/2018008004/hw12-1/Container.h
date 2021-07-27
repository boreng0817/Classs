#include <iostream>
using namespace std;

template <typename T>
class MyContainer{
    public:
        MyContainer(){obj_arr = NULL;n_elements = 0;}
        MyContainer(int n){obj_arr = new T[n];n_elements = n;}
        MyContainer(T* arr,int n){obj_arr = new T[n];for(int i=0;i<n;++i)obj_arr[i]=arr[i];n_elements=n;}
        ~MyContainer() {this->clear();}
        void clear(){delete[] obj_arr;n_elements=0;}
        int size(){return n_elements;}
    protected:
        T* obj_arr = NULL;
        int n_elements;
};

template<typename T>
class MyVector : public MyContainer<T>{
	public:
		MyVector() : MyContainer<T>(){capacity = MyContainer<T>::n_elements;}
		MyVector(int n) : MyContainer<T>(n){capacity = MyContainer<T>::n_elements;}
		MyVector(T* arr,int n) : MyContainer<T>(arr,n) {capacity = MyContainer<T>::n_elements;}
		bool empty(){if(MyContainer<T>::n_elements==0)return 1; else return 0;}
		int size(){MyContainer<T>::size();}
		int max_size(){return capacity;}
		void reserve(int new_cap){
			T* ptr = new T[MyContainer<T>::n_elements];
			for(int i = 0 ; i < MyContainer<T>::n_elements ; ++i)
				ptr[i] = MyContainer<T>::obj_arr[i];
			delete[] MyContainer<T>::obj_arr;
			MyContainer<T>::obj_arr = new T[new_cap];
			for(int i = 0 ; i < MyContainer<T>::n_elements ; ++i)
				MyContainer<T>::obj_arr[i] = ptr[i];
			capacity = new_cap;}
		void push_back(T obj){if(MyContainer<T>::n_elements>=capacity) reserve(1>2*capacity?1:2*capacity);MyContainer<T>::obj_arr[MyContainer<T>::n_elements++] = obj;}
		void pop_back(){MyContainer<T>::obj_arr[--MyContainer<T>::n_elements] = 0;}
		T& front(){return MyContainer<T>::obj_arr[0];}
		T& last(){return MyContainer<T>::obj_arr[MyContainer<T>::n_elements-1];}
		T& at(int idx){return MyContainer<T>::obj_arr[idx];}
		T& operator[](const int &i){return MyContainer<T>::obj_arr[i];}
		MyVector<T> operator+(const MyVector<T>& rhs){
			reserve(rhs.n_elements + this->MyContainer<T>::n_elements);
			for(int i = 0 ; i < rhs.n_elements ; ++i) this->MyContainer<T>::obj_arr[i+this->MyContainer<T>::n_elements] = rhs.obj_arr[i];
			this->MyContainer<T>::n_elements = rhs.n_elements + this->MyContainer<T>::n_elements;
			return (*this);}
	private:
		int capacity;
};

