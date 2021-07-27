#include "calendar.h"
#include <string>
#include <cstdlib>

int Date::GetDaysInYear(int year){ 
	if(year%4==0&&(!(year%100==0))) return 366;
	else if(year%400==0) return 366;
	else return 365;
}

int Date::ComputeDaysFromYearStart(int year, int month, int day){
	int m[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int totalDay = 0;
	if(GetDaysInYear(year)==366) m[1]++;
	for(int i = 0 ; i < month-1 ; ++i) totalDay += m[i];
	return totalDay + day;
}

void Date::NextDay(int n){
	int days = ComputeDaysFromYearStart(year_,month_,day_);
	int m[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	days += n;
	if(days>0){
		while(true){
		int i = 0;
		if(GetDaysInYear(year_)==366) m[1] = 29;
		else m[1] = 28;
		for(;days>m[i]&&i<12;++i) days -= m[i];
		if(i==12)year_++;
		else{
			month_ = i+1;
			day_ = days;
			break;
			}
		}
	}
	else{
		while(true){
			int i = 11;
			year_--;
			if(GetDaysInYear(year_)==366) m[1] = 29;
			else m[1] = 28;
			for(; days+m[i]<=0&&i>=0 ; --i) days += m[i];
			if(i==-1) continue;
			else{
				month_ = i+1;
				day_ = m[i]+days;
				break;
			}
		}
	}
}
	
bool Date::SetDate(int year,int month, int day){
	int m[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	if(GetDaysInYear(year)==366) m[1]++;
	if( (month>0&&month<=12)&&(day>0&&day<=m[month-1]) ){
		year_ = year; month_ = month; day_ = day;
		return true;
	}
	else return false;
}

ostream& operator<<(ostream& os, const Date& c){
	return os << c.year() <<'.'<<c.month()<<'.'<<c.day()<<endl;
}

istream& operator>>(istream& is, Date& c){
	string str;
	int y,m,d;
	getline(cin,str,'.'); y = atoi( str.c_str());
	getline(cin,str,'.'); m = atoi( str.c_str());
	getline(cin,str); d = atoi(str.c_str());
	if(c.SetDate(y,m,d)) cout << c;
	else cout << "Invalid datd: "<<y<<'.'<<m<<'.'<<d<<endl;
	return is;
}
