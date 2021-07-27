#include <iostream>
using namespace std;

class Shape {
public:
	Shape();
	Shape(int canvas_width, int canvas_height, int w, int h);
	double GetArea();
	int GetPerimeter();
	void Draw(int canvas_width, int canvas_height, char ch);
protected:
	int _w;
	int _h;
	int _canvas_width, _canvas_height; // canvas
};

class Rect : public Shape{
public:
	Rect(int w, int h, int c_w, int c_h);
};

class Square : public Shape{
public:
	Square(int wh, int c_w, int c_h);
};

class Diamond : public Shape{
public:
	Diamond(int wh, int c_w, int c_h);
	double GetArea();
	int GetPerimeter();
	void Draw(int canvas_width,int canvas_height, char ch);
};
