#include <iostream>
using namespace std;

template <class T>
class My_shared_ptr
{
	private:
		T* m_obj = NULL;
		int* count = NULL;		
	public:
		My_shared_ptr(){m_obj = NULL;count = NULL;}
		My_shared_ptr(T* obj){m_obj = obj;count = new int;*count = 1;}
		My_shared_ptr(const My_shared_ptr& rhs){
			m_obj = rhs.obj;
			if(rhs.count==NULL){count = new int; *count = 1;}
			else this->increase();
		}

		~My_shared_ptr(){
			this->decrease();
			if(getCount() == 0){ 
				cout << "everything destroyed"<<endl;
				delete m_obj;}

		}

		My_shared_ptr<T>& operator=(const My_shared_ptr<T>& rhs)
		{
			if(count!=NULL) decrease();
			this->m_obj = rhs.m_obj;
			this->count = rhs.count;
			if(this->count==NULL){this->count = new int;*(this->count) = 2;}
			else increase();
			return *this;
		}

		const T* get_m_obj(){ return m_obj;}

		int getCount(){if(count!=NULL) return *count;else return 0;}
		void increase(){*count += 1;}
		void decrease(){*count -= 1;}
};
