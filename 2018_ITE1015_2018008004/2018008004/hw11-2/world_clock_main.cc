#include "world_clock.h"

int main(){
	string str;
	int num;
	WorldClock c;
	while(true){
		cin >> str;
		if(str == "quit") break;
		else if(str == "tick"){
			cin >> num;
			if(num<0) num = (num%604800+604800)%604800;
			c.Tick(num);
			cout << c;
		}
		else if(str == "set") cin >> c;
	}
	return 0;
}
