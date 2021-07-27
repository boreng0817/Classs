#include "draw_shape.h"
#include <vector>

int main(){
	vector<Shape*> v;
	size_t row,col;
	int num[4];
	char brush;
	cin >> col >> row;
	Canvas canvas(row,col);
	canvas.Clear();
	canvas.Print();

	while(1){
		string str;
		cin >> str;
		if(str == "add"){
			string type;
			cin >> type;
			if(type == "rect"){
			cin >> num[0] >> num[1] >> num[2] >> num[3] >> brush;
			v.push_back(new Rectangle(num[0],num[1],num[2],num[3],brush) );
			}
			else if(type == "tri_up"){
			cin >> num[0] >> num[1] >> num[2] >> brush;
			v.push_back(new UpTriangle(num[0],num[1],num[2],brush) );
			}
			else if(type == "tri_down"){
			cin >> num[0] >> num[1] >> num[2] >> brush;
			v.push_back(new DownTriangle(num[0],num[1],num[2],brush) );
			}
			else if(type == "diamond"){
			cin >> num[0] >> num[1] >> num[2] >> brush;
			v.push_back(new Diamond(num[0],num[1],num[2],brush) );
			}
			else continue;
		}
		else if(str == "draw"){
			canvas.Clear();
			for(int i = 0 ; i < v.size() ; i++) v[i]->Draw(&canvas);
			canvas.Print();
		}
		else if(str == "delete"){
			cin >> num[0];
			if(num[0] >= 0 && v.size() > num[0]){
				//delete v[num[0]];
				v.erase(v.begin() + num[0]);
			}
		}
		else if(str == "dump"){
		for(int i = 0 ; i < v.size() ; i++){ cout << i <<' ';v[i]->print();}
		}
		else if(str == "resize"){
			int row,col;
			cin >> row >> col;
			canvas.Resize(row,col);
			canvas.Clear();
		}
		else break;
	}

	for(int i = 0 ; i < v.size() ; i++) delete v[i];
	v.clear();
	return 0;
}
