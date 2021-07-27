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
}
