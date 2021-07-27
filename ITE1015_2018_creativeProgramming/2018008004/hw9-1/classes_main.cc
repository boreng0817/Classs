#include "classes.h"
#include <vector>

int main(){
vector<A*> v;
v.push_back(new A);
v.push_back(new B);
v.push_back(new C);

for(int i = 0 ; i < 3 ; i++){
v[i]->test();
delete v[i]; }

return 0;
}




