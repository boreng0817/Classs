#include <cmath>
#define PI 3.14
using namespace std;

class circle{
	private:
int x;
int y;
int r;

	public:
circle() {}
void setCircle(int n1,int n2,int n3){x=n1,y=n2,r=n3;}
double area(){return PI*r*r;}
double perimeter(){ return 2*PI*r;}
void print();
};

class rectangular{
        private:
int x1;
int y1;
int x2;
int y2;
        public:
rectangular() {}
void setRectangular(int n1,int n2,int n3,int n4){x1=n1,y1=n2,x2=n3,y2=n4;}
double area(){return abs(x2-x1)*abs(y2-y1);}
double perimeter(){ return 2*(abs(x2-x1)+abs(y2-y1));}
void print();
};

