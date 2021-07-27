#include "draw_shape.h"

int main(){
int canvas_w, canvas_h, x, y, w, h;
char ch;
string str;

cin >> canvas_w >> canvas_h;//canvas
cin >> str;
while(str!="quit"){

if(str == "rect"){
cin >> x >> y >> w >> h >> ch;
Rect r(canvas_w, canvas_h,w,h);
cout << "Area: "<<r.GetArea() << endl <<"Perimeter: "<< r.GetPerimeter() << endl;
r.Draw(x,y,ch); }

else if(str == "square"){
cin >> x >> y >> w >> ch;
Square s(canvas_w, canvas_h, w);
cout << "Area: "<<s.GetArea() << endl <<"Perimeter: "<< s.GetPerimeter() << endl;
s.Draw(x,y,ch); }

else if(str == "diamond"){
cin >> x >> y >> w >> ch;
Diamond d(canvas_w, canvas_h, w);
cout << "Area: "<<d.GetArea() << endl <<"Perimeter: "<< d.GetPerimeter() << endl;
d.Draw(x,y,ch); }

cin >> str;
}

return 0;
}
