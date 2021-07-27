#include <iostream>
#include <string>
using namespace std;

class Canvas{
	public:
		Canvas(size_t row, size_t col);
		~Canvas();
		void Resize(size_t w, size_t h);
		bool DrawPixel(int x, int y, char brush);
		void Print();
		void Clear();
		char** canvas;
		int row,col;
};

class Shape{
	public:
		virtual ~Shape() {}
		virtual void Draw(Canvas* cansvas) {}
		virtual void print() {}
		Shape(int x,int y,int h,char brush);
	protected:
		int x, y;
		int h;
		char brush;		
};

class Rectangle : public Shape{
	public:
		int w;
		Rectangle(int x, int y, int h,int w,char brush);
		~Rectangle() {}
		virtual void Draw(Canvas* canvas);
		void print();
};
class UpTriangle : public Shape{
	public:
		UpTriangle(int x,int y,int h,char brush);
		void Draw(Canvas* canvas);
		void print();
};
class DownTriangle : public Shape{
	public:
		DownTriangle(int x,int y,int h,char brush);
		~DownTriangle() {}
		void Draw(Canvas* canvas);
		void print();
};
class Diamond : public Shape{
	public:
		Diamond(int x,int y,int h,char brush);
		~Diamond() {}
		void Draw(Canvas* canvas);
		void print();
};
