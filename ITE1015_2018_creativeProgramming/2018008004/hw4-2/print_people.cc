#include <iostream>
#include <string>

using namespace std;
typedef struct _person{
 string name;
 int age;
}Person;

int main (){
int num;
cin >> num;
Person* p = new Person[num];
for( int i = 0 ; i < num ; i++ )
 cin >> p[i].name >> p[i].age;
for( int i = 0 ; i < num ; i++ )
 cout << "Name:" << p[i].name << ", Age:" << p[i].age << endl;

delete[] p;
return 0;
}

