#include "draw_shape.h"
//#include <cstdlib>
//Canvas-----------------------------
Canvas::Canvas(size_t r, size_t c){
	row = r;
	col = c;
	canvas = new char*[row];
	for(int i = 0 ; i < row ; i++) canvas[i] = new char[col];
}
Canvas::~Canvas(){
	for(int i = 0 ; i < row ; i++) delete[] canvas[i];
	delete[] canvas;
}
void Canvas::Resize(size_t w, size_t h){
	for(int i = 0 ; i < row ; i++) delete[] canvas[i];
	delete[] canvas;
	row = h;
	col = w;
	canvas = new char*[row];
	for(int i = 0 ; i < row ; i++) canvas[i] = new char[col];
}
bool Canvas::DrawPixel(int x, int y, char brush){ 
	if( (x>=0 && x<row) && (y>=0 && y<col) ){
		canvas[x][y] = brush; 
		return true;
	}
	else return false;
}

void Canvas::Print(){
	string num = "0123456789";
	string str = " ";
	for(int i = 0 ; i < col ; i++) str += num[i%10];
	cout << str << endl;

	for(int i = 0 ; i < row ; i++){
		cout << num[i%10];
		for(int j = 0 ; j < col ; j++) cout << canvas[i][j];
		cout << endl;
	}
}
void Canvas::Clear(){
	for(int i = 0 ; i < row ; i++)
		for(int j = 0 ; j < col ; j++)
			canvas[i][j] = '.';
}
//---------------SHAPE---------------
Shape::Shape(int _x, int _y, int _h, char _brush){x=_x;y=_y;h=_h;brush=_brush;}
//Rectangle--------------------------
Rectangle::Rectangle(int _x,int _y,int _w,int _h,char _brush) : Shape(_x,_y,_h,_brush) { w = _w; }
void Rectangle::print() { cout<<"rect "<< x <<' '<<y<<' '<<w<<' '<<h<<' '<<brush<<endl;}
void Rectangle::Draw (Canvas* canvas){
	for(int i = 0 ; i < h ; i++)
		for(int j = 0 ; j < w ; j++) canvas->DrawPixel(y+i,x+j,brush);
}
//UpTriangle-------------------------
UpTriangle::UpTriangle(int x,int y,int h,char brush) : Shape(x,y,h,brush) {}
void UpTriangle::print() { cout<<"tri_up "<< x <<' '<<y<<' '<<h<<' '<<brush<<endl;}
void UpTriangle::Draw(Canvas* canvas){
	int k = 0;
	for(int i = 0 ; i < h ; i++){
		for(int j = 0-k ; j<k+1;j++) canvas->DrawPixel(y+i,x+j,brush);
	k++;
	}
}
//DownTriangle------------------------
DownTriangle::DownTriangle(int x,int y,int h,char brush) : Shape(x,y,h,brush) {}
void DownTriangle::print() { cout<<"tri_down "<< x <<' '<<y<<' '<<h<<' '<<brush<<endl;}
void DownTriangle::Draw(Canvas* canvas){
	int k = 0;
	for(int i = 0 ; i >= -h+1 ; i--){
		for(int j = 0-k ; j<k+1;j++) canvas->DrawPixel(y+i,x+j,brush);
			k++;
		}
}
//Diamond-----------------------------
Diamond::Diamond(int x,int y,int h,char brush) : Shape(x,y,h,brush) {}
void Diamond::print() { cout<<"diamond "<< x <<' '<<y<<' '<<h<<' '<<brush<<endl;}
void Diamond::Draw(Canvas* canvas){
	int k = 0;
	//up
	for(int i = 0 ; i <= h ; i++){
		for(int j = 0-k ; j<k+1;j++) canvas->DrawPixel(y+i,x+j,brush);
		k++;
	}
	k = 0;
	//down
	for(int i = 0 ; i >= -h+1 ; i--){
		for(int j = 0-k ; j<k+1;j++) canvas->DrawPixel(y+i+2*h,x+j,brush);
		 k++;
	}
}
