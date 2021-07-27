#include <iostream>
using namespace std;

class Date {
	public:
		Date(){year_=0; month_ =0; day_ = 0;}
		Date(int year, int month, int day){year_=year;month_=month;day_=day;}

		void NextDay(int n);
		bool SetDate(int year, int month, int day);

		int year() const{return year_;}
		int month() const{return month_;}
		int day() const{return day_;}

	private:
		static int GetDaysInYear(int year);
		static int ComputeDaysFromYearStart(int year, int month, int day);

		int year_, month_, day_;
		bool leap;
};

ostream& operator<<(ostream& os, const Date& c);
istream& operator>>(istream& is, Date& c);
