#include "rectangle.h"

Rectangle::Rectangle(int width, int height) {w = width; h = height;}
int Rectangle::getArea() {return w*h;}
int Rectangle::getPerimeter() {return 2*(w+h);}

Square::Square(int w) : Rectangle(w,w) {}
void Square::print(){cout<<w<<'x'<<h<<" Square"<<endl<<"Area: "<<getArea() << endl << "Perimeter: "<< getPerimeter() << endl; }

NonSquare::NonSquare(int w, int h) : Rectangle(w,h) {}
void NonSquare::print(){cout<<w<<'x'<<h<<" NonSquare"<<endl<<"Area: "<<getArea() << endl << "Perimeter: "<< getPerimeter() << endl; }
