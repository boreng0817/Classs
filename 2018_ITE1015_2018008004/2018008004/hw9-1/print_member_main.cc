#include "print_member.h"
#include <vector>

int main(){
vector<A*> v;
int num1;
double num2;
string str;
cin >> num1 >> num2 >> str;

v.push_back(new A(num1));
v.push_back(new B(num2));
v.push_back(new C(str));

for(int i = 0 ; i < 3 ; i++) v[i]->print();
for(int i = 0 ; i < 3 ; i++) delete v[i];

return 0;
}
