#include "draw_shape.h"
#include <string>
#include <cmath>

//class::shape
Shape::Shape() {}
Shape::Shape(int canvas_width, int canvas_height, int w, int h) {_canvas_width = canvas_width;_canvas_height = canvas_height; _w = w; _h = h;}
double Shape::GetArea() {return _w*_h;}
int Shape::GetPerimeter() {return 2*(_w+_h);}
//draw for rect and square
void Shape::Draw(int x, int y, char ch){
string str = " ", number = "0123456789";
for(int i = 0 ; i < _canvas_width ; i++) str += number[i%10];
cout << str << endl;
for(int i = 0 ; i < _canvas_height ; i++){
str = "";
str += number[i%10];
for(int j = 0 ; j < _canvas_width ; j++){
if( ( j>=x && j<x+_h) && ( i>=y && i<y+_w) ) str+=ch;
else str += '.'; }
cout << str << endl;
} }
//constructor
Rect::Rect(int c_w, int c_h, int w, int h) : Shape(c_w,c_h,w,h) {}
Square::Square(int c_w, int c_h, int wh) : Shape(c_w,c_h,wh,wh) {}
Diamond::Diamond(int c_w, int c_h, int wh) : Shape(c_w,c_h,wh,2*wh+1) {}
//Diamond Method
double Diamond::GetArea() {return (double)_h*_h/2;}
int Diamond::GetPerimeter() {return _h*4;}
//draw diamond
void Diamond::Draw(int x, int y, char ch){
string str = " ", number = "0123456789";
int num = 0;
for(int i = 0 ; i < _canvas_width ; i++) str += number[i%10];
cout << str << endl;
//draw
for(int i = 0 ; i < _canvas_height ; i++){
str = "";
str += number[i%10];
for(int j = 0 ; j < _canvas_width ; j++){
//str
if ( ( j>=x-(_w-abs(_w-num)) ) && ( j<=x+ (_w-abs(_w-num)) ) && ( i>=y && i<=y+_w*2) ) str+=ch;
else str += '.'; 
//num
if( (j==_canvas_width-1) && ( (i>=y) && (i<=y+2*_w) ) ) num++;; }
cout << str << endl;
} }
