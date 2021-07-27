#include <string>
#include <cstdlib>
#include "world_clock.h"

void WorldClock::Tick(int seconds){
	int total = 3600*hh + 60*mm + ss + seconds;
	hh = (total/3600)%24;
	total %= 3600;
	mm = total/60;
	total %= 60;
	ss = total;
}

bool WorldClock::SetTime(int h,int m, int s){
	if((0<=h&&h<=24)&&(0<=m&&m<60)&&(0<=s&&s<60)){
		hh=h;mm=m;ss=s;
		return true;
	}	
	else return false;
}

ostream& operator<<(ostream& os, const WorldClock& c){return os << c.hour() <<':'<<c.minute() << ':' << c.second() << endl;}

istream& operator>>(istream& is, WorldClock& c){
	string str;
	int num[3];
	for(int i = 0 ; i < 2 ; ++i){
		getline(cin,str,':');
		num[i] = atoi(str.c_str());
	}
	getline(cin,str);
	num[2] = atoi(str.c_str());

	if(c.SetTime(num[0],num[1],num[2])) cout << c;
	else cout <<"Invalid time: "<<num[0]<<':'<<num[1]<<':'<<num[2]<<endl;
	return is;
}
