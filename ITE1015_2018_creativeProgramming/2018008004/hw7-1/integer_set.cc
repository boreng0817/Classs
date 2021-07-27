#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include "integer_set.h"
using namespace std;

IntegerSet::IntegerSet () {}
IntegerSet::~IntegerSet () {}

void IntegerSet::AddNumber (int num){
if( numbers_.end() == find(numbers_.begin(), numbers_.end() , num)){
numbers_.push_back(num);
sort(numbers_.begin(),numbers_.end());
}
GetAll();
}

void IntegerSet::DeleteNumber (int num){
if( numbers_.end()  != find(numbers_.begin(), numbers_.end() , num)){
numbers_.erase(find(numbers_.begin(),numbers_.end(), num));
}
GetAll();
}

int IntegerSet::GetItem(int pos) const{
if( (pos >= numbers_.size()) || (pos<0) ) {
cout << "-1" << endl;
return -1; }

else {
cout << numbers_[pos] << endl; 
return numbers_[pos];
}
}

vector<int> IntegerSet::GetAll() const{
for(int i = 0 ; i < numbers_.size() ; i++) cout << numbers_[i] << ' ';
cout << endl;
return numbers_;
}
