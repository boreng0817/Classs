#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc,char**argv){
string a = "";
int sum = 0;

for(int i = 1 ; i<argc ; i++){
if(atoi(argv[i])==0) a += argv[i];
else sum += atoi(argv[i]); }

cout << a << endl << sum << endl;
return 0;
}
