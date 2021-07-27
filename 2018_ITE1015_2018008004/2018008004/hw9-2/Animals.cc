#include "Animals.h"
//Animal-----------------
Animal::Animal(int num, string str) {age = num;name = str;}
void Animal::printInfo() {}
//Zebra------------------
Zebra::Zebra(int num1,int num2,string str) : Animal(num2,str) { numStripes = num1;}
void Zebra::printInfo(){cout << "Zebra, Name: " << name << ", Age: "<<age<<", Number of stripes: "<<numStripes<<endl;}
//Cat--------------------
Cat::Cat(int num1,string str1, string str2) : Animal(num1,str1){favoriteToy = str2;}
void Cat::printInfo(){cout << "Cat, Name: " << name << ", Age: "<<age<<", Favorite toy: "<<favoriteToy<<endl;}

