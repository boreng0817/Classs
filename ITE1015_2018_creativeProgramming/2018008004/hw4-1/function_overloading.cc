#include <iostream>
#include <string>

using namespace std;
int add(int, int);
string add(string, string);

int main(){
int a , b;
string str1 , str2;
cin >> a >> b >> str1 >> str2;

cout << add(a,b) << endl 
     << add(str1 , str2) << endl;

return 0;
}

int add (int a , int b){
return a + b; }

string add (string a , string b) {
a.append(1, '-');
a.append(b);
return a; }


