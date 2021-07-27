#include <iostream>
using namespace std;

template <class T>
class Node
{
	public:
		T data;
		Node<T>* next;
}; 

template <class T>
class List {
private:
    Node<T> *head;
public:
    List() : head(NULL) {}
    ~List(){
	Node<T>* ptr = head;
	Node<T>* temp = NULL;
	while(ptr->next!=NULL){temp = ptr;ptr = ptr->next;delete temp->next;}
		}
	List(T* arr, int n_nodes){
		head = NULL;
		for(int i = 0 ; i < n_nodes ; ++i) push_back(arr[i]);
		}
	void insert_at(int idx, T data){
		Node<T>* temp1 = head;
		int i;
		for(i = 0 ; i < idx-1&&temp1->next!=NULL ; ++i) temp1=temp1->next;
		if(i!=idx-1&&idx!=0) return;
		else if(idx==0)push_front(data);
		else if(temp1->next==NULL) push_back(data);
		else{
		Node<T>* ins = new Node<T>;
		ins->data = data;
		ins->next = temp1->next;
		temp1->next = ins;
			}
		}
    void remove_at(int idx){
		int i;
		Node<T>* temp = head;
		if(idx==0){
			pop_front();
			return;
			}
		for(i = 0 ; i < idx - 1 && temp->next != NULL ; ++i) temp = temp->next;
		if(i==idx-1){
			Node<T>* check = temp->next;
			if(check->next==NULL){
				delete check;
				temp->next = NULL;
			}
			else{
				temp->next = check->next;
				delete check;
				}
			}
		}
    void pop_back(){
		Node<T>* temp1 = head;
		Node<T>* temp2;
		if(temp1==NULL) return;

		else if(temp1->next==NULL){delete temp1->next;head=NULL;}
		
		else{
		while(true){
			temp2 = temp1->next;
			if(temp2->next==NULL){
				delete temp2;
				temp1->next = NULL;
				break;
				}
			else temp1 = temp1->next;
				}
			}
		}
    void push_back(T val){
		Node<T>* temp = head;
		Node<T>* ins = new Node<T>;
		ins->data = val;
		ins->next = NULL;
		if(temp==NULL) head = ins;
		else{
		while(temp->next!=NULL) temp = temp->next;
		temp->next = ins; 
		}
	}
    void pop_front(){
		Node<T>* temp = head;
		if(temp==NULL) return;
		else if(temp->next==NULL) {delete temp->next;head=NULL;}
		else{
			head = head->next;
			delete temp;
		}
	}
    void push_front(T val){
		Node<T>* ins = new Node<T>;
		Node<T>* temp = head;
		ins->data =val;
		ins->next = NULL;

		if(temp==NULL) head = ins;
		else if(temp->next == NULL) head->next = ins;
		else{
			ins->next = head;
			head = ins;
		}
	}
    friend ostream& operator<<(ostream& out, List& rhs){
		Node<T>* temp = rhs.head;
		if(temp==NULL) return out<<"NULL POINTER EXCEPTION!"<<endl;
		else{
			out << temp->data;
			while(temp->next!=NULL){
				temp = temp->next;
				out<<','<< temp->data;
			}
			return out << endl;
		}
	}

};
