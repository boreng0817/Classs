#include <iostream>
#include <string>
using namespace std;

void swapInt(int&, int&);
void swapString(string&,string&);

int main(){
int n1, n2;
string s1, s2;
cin >> n1 >> n2 >> s1 >> s2;

for(int i = 0 ; i < 2 ; i++){
cout << "n1: "<<n1<<", n2: "<<n2<<", s1: "<<s1<<", s2: "<<s2<<endl;
swapInt(n1,n2);
swapString(s1,s2); }

return 0;}

void swapInt(int& a,int& b){
int temp = a;
a = b;
b = temp; }

void swapString(string& a, string& b){
string temp = a;
a = b;
b = temp; }
