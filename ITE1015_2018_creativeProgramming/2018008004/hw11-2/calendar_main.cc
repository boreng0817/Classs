#include "calendar.h"

int main(){
	string str;
	int num;
	Date c(0,0,0);
	while(true){
		cin >> str;
		if(str == "quit") break;
		else if(str == "set") cin >> c;
		else if(str == "next_day") {c.NextDay(1);cout << c;}
		else if(str == "next"){cin >> num;	c.NextDay(num);cout <<c;}
	}
	return 0;
}
