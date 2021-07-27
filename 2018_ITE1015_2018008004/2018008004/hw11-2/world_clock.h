#include <iostream>
using namespace std;
class WorldClock {
	public:
		void Tick(int seconds = 1);
		bool SetTime(int hour, int minute, int second); 

		int hour() const{return hh;}
		int minute() const{return mm;}
		int second() const{return ss;}

	private:
		int hh;
		int mm;
		int ss;
};
ostream& operator<<(ostream& os, const WorldClock& c);
istream& operator>>(istream& is, WorldClock& c);

