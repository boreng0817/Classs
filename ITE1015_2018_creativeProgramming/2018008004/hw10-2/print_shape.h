#include <string>
#define PI 3.141592
using namespace std;

class Shape{
	public:
		virtual double getArea() = 0;
		virtual double getPerimeter() = 0;
		virtual string getTypeString() = 0;
};
class Circle : public Shape{
	public:
		Circle(double radius);
		double getArea();
		double getPerimeter();
		string getTypeString();
	private:
		double r;
};
class Rectangle : public Shape{
	public:
		Rectangle(double width,double height);
		double getArea();
		double getPerimeter();
		string getTypeString();
	private:
		double w,h;
};

