using namespace std;

class Shape{
	public:
		Shape(double w,double h) {_w=w;_h=h;}
		virtual double getArea() = 0;
	protected:
		double _w,_h;
};

class Triangle : public Shape{
	public:
		Triangle(double w,double h) : Shape(w,h) {}
		double getArea() {return _w*_h/2;}
};

class Rectangle : public Shape{
	public:
		Rectangle(double w,double h) : Shape(w,h) {}
		double getArea() {return _w*_h;}
};
