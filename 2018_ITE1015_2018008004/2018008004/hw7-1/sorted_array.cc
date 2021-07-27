#include <iostream>
#include <algorithm>
#include "sorted_array.h"
using namespace std;
//CONSTRUCTOR, DESTRUCTOR
SortedArray::SortedArray() {}
SortedArray::~SortedArray() {}

//FUNCTIONS
void SortedArray::AddNumber(int num){
numbers_.push_back(num);
sort(numbers_.begin() , numbers_.end() );
}

vector<int> SortedArray::GetSortedAscending() const{
for(int i = 0; i<numbers_.size() ; i++) cout << numbers_[i] << ' ';
cout << endl;

return numbers_;
}

vector<int> SortedArray::GetSortedDescending() const{
for(int i = numbers_.size() - 1 ; i>-1 ; i--) cout << numbers_[i] << ' ';
cout << endl;

return numbers_;
}

int SortedArray::GetMax() const{
cout << numbers_[numbers_.size() - 1] << endl;
return numbers_[numbers_.size() - 1];
}

int SortedArray::GetMin() const{
cout << numbers_[0] << endl;
return numbers_[0];
}
